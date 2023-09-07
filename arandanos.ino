#include <RtcDS1302.h>

ThreeWire myWire(4,5,3); // IO, SCLK, CE

RtcDS1302<ThreeWire> Rtc(myWire);
RtcDateTime now;
volatile int NumPulsos; //variable para la cantidad de pulsos recibidos
float factor_conversion=7.5; //para convertir de frecuencia a caudal
int long pulsos_totales = 0;
int PinSensor = 2;    //Sensor conectado en el pin 2
const int ledPin = 13;  
const int pinRiegoManual = 7;  


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
        
}

void loop ()
{
   now = Rtc.GetDateTime();  // Obtener la fecha y hora actual del RTC

    int currentMinute = now.Minute();  // Obtener el minuto actual

    int currentHour = now.Hour();  // Obtener la hora actual

    bool ordenRTC = false;
    bool swichState = digitalRead(pinRiegoManual);
     if ((currentHour == 13 && currentMinute >= 0 && currentMinute < 4) ||
     (currentHour == 13 && currentMinute >= 0 && currentMinute < 4) ||
     (currentHour == 21 && currentMinute >= 1 && currentMinute < 4))
     {
      ordenRTC = true;
      }
      
    if (swichState || ordenRTC){
      digitalWrite(ledPin, HIGH);  // Encender el LED durante el riego
      
      }
      else{
      digitalWrite(ledPin, LOW);  // Encender el LED durante el riego
        
      }
      float frecuencia=ObtenerFrecuencia(); //obtenemos la Frecuencia de los pulsos en Hz
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
int ObtenerFrecuencia() 
{
  int frecuencia;
  NumPulsos = 0;   //Ponemos a 0 el número de pulsos
  interrupts();    //Habilitamos las interrupciones
  delay(1000);   //muestra de 1 segundo
  noInterrupts(); //Desabilitamos las interrupciones
  frecuencia = NumPulsos; //Hz(pulsos por segundo)
  return frecuencia;
}

#include <RtcDS1302.h>

ThreeWire myWire(4,5,3); // IO, SCLK, CE

RtcDS1302<ThreeWire> Rtc(myWire);
RtcDateTime now;
volatile int NumPulsos; //variable para la cantidad de pulsos recibidos
float factor_conversion=7.5; //para convertir de frecuencia a caudal
int long pulsos_totales = 0;
int PinSensor = 2;    //Sensor conectado en el pin 2
const int ledPin = 13;  
const int pinRiegoManual = 7;  


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
        
}

void loop ()
{
   now = Rtc.GetDateTime();  // Obtener la fecha y hora actual del RTC

    int currentMinute = now.Minute();  // Obtener el minuto actual

    int currentHour = now.Hour();  // Obtener la hora actual

    bool ordenRTC = false;
    bool swichState = digitalRead(pinRiegoManual);
     if ((currentHour == 13 && currentMinute >= 0 && currentMinute < 4) ||
     (currentHour == 13 && currentMinute >= 0 && currentMinute < 4) ||
     (currentHour == 21 && currentMinute >= 1 && currentMinute < 4))
     {
      ordenRTC = true;
      }
      
    if (swichState || ordenRTC){
      digitalWrite(ledPin, HIGH);  // Encender el LED durante el riego
      
      }
      else{
      digitalWrite(ledPin, LOW);  // Encender el LED durante el riego
        
      }
      float frecuencia=ObtenerFrecuencia(); //obtenemos la Frecuencia de los pulsos en Hz
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
int ObtenerFrecuencia() 
{
  int frecuencia;
  NumPulsos = 0;   //Ponemos a 0 el número de pulsos
  interrupts();    //Habilitamos las interrupciones
  delay(1000);   //muestra de 1 segundo
  noInterrupts(); //Desabilitamos las interrupciones
  frecuencia = NumPulsos; //Hz(pulsos por segundo)
  return frecuencia;
}

#include <RtcDS1302.h>

ThreeWire myWire(4,5,3); // IO, SCLK, CE

RtcDS1302<ThreeWire> Rtc(myWire);
RtcDateTime now;
volatile int NumPulsos; //variable para la cantidad de pulsos recibidos
float factor_conversion=7.5; //para convertir de frecuencia a caudal
int long pulsos_totales = 0;
int PinSensor = 2;    //Sensor conectado en el pin 2
const int ledPin = 13;  
const int pinRiegoManual = 7;  


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
        
}

void loop ()
{
   now = Rtc.GetDateTime();  // Obtener la fecha y hora actual del RTC

    int currentMinute = now.Minute();  // Obtener el minuto actual

    int currentHour = now.Hour();  // Obtener la hora actual

    bool ordenRTC = false;
    bool swichState = digitalRead(pinRiegoManual);
     if ((currentHour == 13 && currentMinute >= 0 && currentMinute < 4) ||
     (currentHour == 13 && currentMinute >= 0 && currentMinute < 4) ||
     (currentHour == 21 && currentMinute >= 1 && currentMinute < 4))
     {
      ordenRTC = true;
      }
      
    if (swichState || ordenRTC){
      digitalWrite(ledPin, HIGH);  // Encender el LED durante el riego
      
      }
      else{
      digitalWrite(ledPin, LOW);  // Encender el LED durante el riego
        
      }
      float frecuencia=ObtenerFrecuencia(); //obtenemos la Frecuencia de los pulsos en Hz
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
int ObtenerFrecuencia() 
{
  int frecuencia;
  NumPulsos = 0;   //Ponemos a 0 el número de pulsos
  interrupts();    //Habilitamos las interrupciones
  delay(1000);   //muestra de 1 segundo
  noInterrupts(); //Desabilitamos las interrupciones
  frecuencia = NumPulsos; //Hz(pulsos por segundo)
  return frecuencia;
}
