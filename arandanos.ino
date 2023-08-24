
// CONNECTIONS:
// DS1302 CLK/SCLK --> 5
// DS1302 DAT/IO --> 4
// DS1302 RST/CE --> 2
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND

//#include <LedControl.h>

#include <RtcDS1302.h>

ThreeWire myWire(4,5,2); // IO, SCLK, CE

RtcDS1302<ThreeWire> Rtc(myWire);

const int ledPin = 13;  // Pin del LED

void setup (){
  
    pinMode(ledPin, OUTPUT); 
    
    Serial.begin(57600);

    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);

    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    Rtc.SetDateTime(compiled);
    printDateTime(compiled);
    Serial.println();
    //RtcDateTime newDateTime(2023, 8, 24, 18, 25, 0);  // Nueva fecha y hora
    //Rtc.SetDateTime(newDateTime);

      if (!Rtc.IsDateTimeValid()){
          
          Serial.println("RTC lost confidence in the DateTime!");
          Rtc.SetDateTime(compiled);
          }
  
      if (Rtc.GetIsWriteProtected()){
        
          Serial.println("RTC was write protected, enabling writing now");
          Rtc.SetIsWriteProtected(false);
          }
  
      if (!Rtc.GetIsRunning()){
        
          Serial.println("RTC was not actively running, starting now");
          Rtc.SetIsRunning(true);
          }

    RtcDateTime now = Rtc.GetDateTime();
    
    if (now < compiled){
      
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
        }
    else if (now > compiled){
      
        Serial.println("RTC is newer than compile time. (this is expected)");
        }
    else if (now == compiled){
      
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
        }
}

void loop (){
  
   RtcDateTime now = Rtc.GetDateTime();  // Obtener la fecha y hora actual del RTC

    int currentMinute = now.Minute();  // Obtener el minuto actual

    int currentHour = now.Hour();  // Obtener la hora actual

    
    if ((currentHour == 5 && currentMinute >= 1 && currentMinute <= 7) || (currentHour == 13 && currentMinute >= 1 && currentMinute <= 7) || (currentHour == 21 && currentMinute >= 1 && currentMinute <= 7)){
      
        digitalWrite(ledPin, HIGH);  // Encender el LED durante el riego
        }
        else {
          digitalWrite(ledPin, LOW);   // Apagar el LED si no es el riego
          }

    printDateTime(now);
    
    Serial.println();

    if (!now.IsValid()){
      
        Serial.println("RTC lost confidence in the DateTime!");
        }

    delay(10000); // ten seconds
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt){
  
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

void action(const RtcDateTime& dt){
  
  char datestring[26];
  }
