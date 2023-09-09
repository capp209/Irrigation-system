#include <EEPROM.h>
#include <RtcDS1302.h>

ThreeWire myWire(4,5,3); // IO, SCLK, CE

RtcDS1302<ThreeWire> Rtc(myWire);
RtcDateTime now;
RtcDateTime inicioRiegoManual;

volatile int numeroDePulsos;

float pulsosAVolumen = 0.074;

int pinCondensadorDeFlusho = 2;  
const int pinBomba = 13;  
const int pinSolicitudDeRiegoManual = 7; 
 
const byte posicionMemoriaPuntero = 0;
const byte posicionMemoriaFecha = 2;

bool solicitudDeRiegoManual = false;
bool solicitudDeRiegoProgramado = false;
bool riego = false;

int puntero = 4;
int fechaEnDiasDesdeEl2000 = 0;

int minutoActual = 0;
int horaActual = 0;
bool senialActual = LOW;
bool senialAntigua = LOW;
int estadoDeSwitch = 0;

void setup ()
{
    numeroDePulsos = 0;
    pinMode(pinSolicitudDeRiegoManual,INPUT);
    pinMode(pinBomba, OUTPUT); 
    pinMode(pinCondensadorDeFlusho, INPUT); 
    attachInterrupt(digitalPinToInterrupt(pinCondensadorDeFlusho),contarPulsos,RISING); //(Interrupcion 0(Pin2),funcion,Flanco de subida)
 
    Serial.begin(9600);

    Serial.println(" Fecha de fundacion: ");
    printDateTime(RtcDateTime(__DATE__,__TIME__));
    Serial.println();

    Rtc.Begin();

    RtcDateTime horaDeCompilacion = RtcDateTime(__DATE__,__TIME__);
    Rtc.SetDateTime(horaDeCompilacion);
    Serial.println();

    if (esCorruptoElPuntero()){
      inicializarPuntero();
      }

    if (esCorruptaLaFecha()) {
      inicializarFecha();
    }

    Serial.print("Fecha del reloj en dias desde el 2000: ");
    Serial.println(fechaEnDiasDesdeEl2000);
    
}

void loop ()
{
   solicitudDeRiegoManual = ordenDeRiegoManual();
   solicitudDeRiegoProgramado = esHoraDeRiego();
        
    if (solicitudDeRiegoManual || solicitudDeRiegoProgramado){
      digitalWrite(pinBomba, HIGH);
      riego = true;
      }
      else{
      digitalWrite(pinBomba, LOW);
      if (riego == true) {
         riego = false;
         registrarEnMemoria();
         Serial.print("Ultimo puntero escrito: ");
         Serial.println(puntero);
         Serial.print("la cantidad en litros fue: ");
         Serial.println(EEPROM.read(puntero+2));
        }
       
      }
    
 }


//---Función para obtener los pulsos--------------------------------------------------------

void contarPulsos ()
{ 
  numeroDePulsos++;
} 


//---Función para calcular el dia a escribir en registro--------------------------------------

int diaDesdeLaFundacion(){
  return (now.TotalDays() - fechaEnDiasDesdeEl2000);
  }


//---Función para obtener el conteo en multiplos de 10 minutos de la hora actual--------------

int decaminutoDelDia(){
  return int((now.Hour()*60+now.Minute())/10);
  }


//---Función para obtener el volumen escrito en pares de litros--------------------------------
  
int duoLitros(){
  return int(numeroDePulsos * pulsosAVolumen / 2);
  }


//---Función encargada de registrar en memoria cada evento de riego-----------------------------

void registrarEnMemoria(){
  puntero = obtenerPuntero();
  EEPROM.write(puntero,diaDesdeLaFundacion());
  
  EEPROM.write(puntero + 1, decaminutoDelDia());
  
  EEPROM.write(puntero + 2,duoLitros());
  
  EEPROM.write(posicionMemoriaPuntero, int((puntero+3) / 255));
  EEPROM.write(posicionMemoriaPuntero +1, int((puntero+3) % 255));
  
  borrarContadorDePulsos();
  }


//---Función para reiniciar la cuenta de los pulsos---------------------------------------------

void borrarContadorDePulsos(){
  numeroDePulsos = 0;
  }


//---Función para inicializar el puntero en la posicion de inicio de escritura de datos---------
  
void inicializarPuntero(){
      EEPROM.write(posicionMemoriaPuntero,0);
      EEPROM.write(posicionMemoriaPuntero + 1,4);
  }


//---Función para escribir la fecha de fundacion en la poscicion de memoria ---------------------
  
void inicializarFecha(){
      now = Rtc.GetDateTime();
      EEPROM.write(posicionMemoriaFecha, int(now.TotalDays()/255));
      EEPROM.write(posicionMemoriaFecha + 1, int(now.TotalDays()%255));
      fechaEnDiasDesdeEl2000 = now.TotalDays();
  }


//---Función para comprobar la validez de la fecha--------------------------------------------

bool esCorruptaLaFecha(){
  
  fechaEnDiasDesdeEl2000 = int(EEPROM.read(posicionMemoriaFecha) * 255 + EEPROM.read(posicionMemoriaFecha + 1));

  return !(fechaEnDiasDesdeEl2000 > 22*365 and fechaEnDiasDesdeEl2000 < 32*365);
  }


//---Función para comprobar la validez del puntero-----------------------------------------------
  
bool esCorruptoElPuntero(){
  puntero = obtenerPuntero();
  return (!(puntero > 4 and puntero < 1024));
  }


//---Funcion de orden de riego por flanco de subida de switch-------------------------------------

bool ordenDeRiegoManual(){
  now = Rtc.GetDateTime();

  switch (estadoDeSwitch){
    case 0:
      if (!digitalRead(pinSolicitudDeRiegoManual)){
        inicioRiegoManual = Rtc.GetDateTime();
        estadoDeSwitch = 1;
        return true;
      }
      return false;
    case 1:
      if (!!digitalRead(pinSolicitudDeRiegoManual)){
        estadoDeSwitch = 2;
        }         
       return true;
    case 2:
      if (((now.Minute() - inicioRiegoManual.Minute()) >= 2) or !digitalRead(pinSolicitudDeRiegoManual)){
        estadoDeSwitch = 3;
        return false;
          }
      return true;
    case 3:
    if (!!digitalRead(pinSolicitudDeRiegoManual)){
        estadoDeSwitch = 0;
        }         
       return false;
    default:
      estadoDeSwitch = 0;
      return false; 
    }
}


//---Función para determinar si es es hora de activar la bomba------------------------------------

bool esHoraDeRiego(){
    now = Rtc.GetDateTime();
    minutoActual = now.Minute(); 
    horaActual = now.Hour();

    return ((horaActual == 7 && minutoActual >= 0 && minutoActual < 4) ||
           (horaActual == 13 && minutoActual >= 0 && minutoActual < 4) ||
           (horaActual == 21 && minutoActual >= 0 && minutoActual < 4));
  }


//---Función para obtener el valor del puntero----------------------------------------------------
  
int obtenerPuntero(){
  return (EEPROM.read(posicionMemoriaPuntero) * 255 + EEPROM.read(posicionMemoriaPuntero + 1));
  }
  
//---macro para el calculo del tamaño de un arreglo-----------------------------------------------
#define countof(a) (sizeof(a) / sizeof(a[0]))


//---Función para imprimir la variable de tipo Rtc de la forma correcta---------------------------

void printDateTime(const RtcDateTime dt)
{
  
    char datestring[26];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u  %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.println(datestring);
}
