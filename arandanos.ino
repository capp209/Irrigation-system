
#include <RtcDS1302.h>

ThreeWire myWire(4,5,2); // IO, SCLK, CE

RtcDS1302<ThreeWire> Rtc(myWire);
RtcDateTime now;
const int ledPin = 13;  
const int pinRiegoManual = 7;  

void setup ()
{
    pinMode(pinRiegoManual,INPUT);
    pinMode(ledPin, OUTPUT); 
    
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
     if ((currentHour == 10 && currentMinute >= 48 && currentMinute < 49) ||
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
    
