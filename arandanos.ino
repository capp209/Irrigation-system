#include <EEPROM.h>
#include <RtcDS1302.h>

ThreeWire myWire(4,5,3); // IO, SCLK, CE

RtcDS1302<ThreeWire> Rtc(myWire);
RtcDateTime now;
volatile int NumPulsos; //variable para la cantidad de pulsos recibidos

float factor_conversion=0.5; //para convertir de frecuencia a caudal

int long pulsos_totales = 0;

int PinSensor = 2;    //Sensor conectado en el pin 2
const int ledPin = 13;  
const int pinRiegoManual = 7;  
const byte posicionMemoriaPuntero0 = 0;
const byte posicionMemoriaFecha = 2;

bool ordenRTC = false;
bool bandera = false;
int puntero = 4;
int fechaEnDias = 0;

void setup ()
{
    pinMode(pinRiegoManual,INPUT);
    pinMode(ledPin, OUTPUT); 
    pinMode(PinSensor, INPUT); 
    attachInterrupt(0,ContarPulsos,RISING); //(Interrupcion 0(Pin2),funcion,Flanco de subida)
 
    Serial.begin(9600);

    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);

    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    Rtc.SetDateTime(compiled);
    printDateTime(compiled);
    
    Serial.println();
    
    RtcDateTime now = Rtc.GetDateTime();

    int fechaEnDias0 = EEPROM.read(posicionMemoriaFecha);
    int fechaEnDias1 = EEPROM.read(posicionMemoriaFecha + 1);
    fechaEnDias = fechaEnDias0 * 255 + fechaEnDias1;

    int puntero0 = EEPROM.read(posicionMemoriaPuntero0);
    int puntero1 = EEPROM.read(posicionMemoriaPuntero0 + 1);
    puntero = puntero0 * 255 + puntero1;

    if (esCorruptaLaFecha()) {
       EEPROM.write(posicionMemoriaFecha, int(now.TotalDays()/255));
       EEPROM.write(posicionMemoriaFecha + 1, int(now.TotalDays()%255));
       fechaEnDias = now.TotalDays();
    }
    Serial.print(fechaEnDias);
}

void loop ()
{
   now = Rtc.GetDateTime();  // Obtener la fecha y hora actual del RTC

    int currentMinute = now.Minute();  // Obtener el minuto actual

    int currentHour = now.Hour();  // Obtener la hora actual

    
    bool swichState = digitalRead(pinRiegoManual);
    
     if ((currentHour == 13 && currentMinute >= 0 && currentMinute < 4) ||
     (currentHour == 13 && currentMinute >= 0 && currentMinute < 4) ||
     (currentHour == 21 && currentMinute >= 1 && currentMinute < 4))
     {
      ordenRTC = true;
      }
      else{
        ordenRTC = false;
        }
      
    if (swichState || ordenRTC){
      digitalWrite(ledPin, HIGH);  // Encender el LED durante el riego
      bandera = true;
      }
      else{
      digitalWrite(ledPin, LOW);  // Encender el LED durante el riego
      if (bandera == true) {
         writeEEPROM();
         bandera = false;
        }
       
      }
      float frecuencia=ObtenerVolumen(); //obtenemos la Frecuencia de los pulsos en Hz
      float caudal_L_m=frecuencia/factor_conversion; //calculamos el caudal en L/m
      float caudal_L_h=caudal_L_m*60; //calculamos el caudal en L/h
//-----Enviamos por el puerto serie---------------
      Serial.print ("FrecuenciaPulsos: "); 
      Serial.print (frecuencia,0); 
      Serial.print ("Hz\tCaudal: "); 
      Serial.print (caudal_L_m,3); 
      Serial.print (" L/m\t"); 
      Serial.print (caudal_L_h,3); 
      Serial.println ("L/h"); 
      Serial.print (pulsos_totales); 
      Serial.println ("pulsos"); 
      
      pulsos_totales += NumPulsos;

      Serial.print (puntero); 
      Serial.println ("puntero"); 
          
   /* printDateTime(now);
    
    Serial.println();

    if (!now.IsValid()){
      
        Serial.println("RTC lost confidence in the DateTime!");
        }
    */
    
 }

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
  
    char datestring[26];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}


void ContarPulsos ()
{ 
  NumPulsos++;  //incrementamos la variable de pulsos
} 


//---Función para obtener frecuencia de los pulsos--------
int ObtenerVolumen() 
{
  int frecuencia;
  NumPulsos = 0;   //Ponemos a 0 el número de pulsos
  interrupts();    //Habilitamos las interrupciones
  delay(1000);   //muestra de 1 segundo
  noInterrupts(); //Desabilitamos las interrupciones
  frecuencia = NumPulsos; //Hz(pulsos por segundo)
  return frecuencia;
}

bool esCorruptaLaFecha(){
  return !(fechaEnDias > 22*365 and fechaEnDias < 32*365);
  }

int formatDay(){
  return (now.TotalDays() - fechaEnDias);
  }
  
int formatHour(){
  return int((now.Hour()*60+now.Minute())/10);
  }
  
int formatVolume(){
  return int(NumPulsos * factor_conversion);
  }

void writeEEPROM(){
  int puntero0 = EEPROM.read(posicionMemoriaPuntero0);
  int puntero1 = EEPROM.read(posicionMemoriaPuntero0 + 1);
  puntero = puntero0 * 255 + puntero1;
  EEPROM.write(puntero,formatDay());
  EEPROM.write(puntero + 1, formatHour());
  EEPROM.write(puntero + 2,formatVolume());
  EEPROM.write(posicionMemoriaPuntero0, int((puntero+3) / 255));
  EEPROM.write(posicionMemoriaPuntero0 +1, int((puntero+3) % 255));
  }
