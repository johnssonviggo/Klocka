/*
 File: klocka.ino
 Author: Viggo Johnsson
 Date: 2023-09-28
 Description: Programmet är en klocka som visas på skärmen samt 
 visar ungefärlig temperatur med sevo.
 */


#include <U8glib.h>
#include <Wire.h>
#include <RtcDS3231.h>
#include <Servo.h>

// initiera RTC modul
RtcDS3231<TwoWire> Rtc(Wire);

int hour;
int minute;
int second;
float tmp;

//Initiera Oled

U8GLIB_SSD1306_128X64 oled(U8G_I2C_OPT_NONE);

//Initiera Servo

Servo myservo;

/* Startar igång allt som behövs vilket inkluderar Skärm samt sätter 
igång kommunikationen med braudhastigheten
Skapar Rtc med kompileringsdatum o sätter det som datum o tid på RTC
Samt anger vilken font som används på skärmen och kopplar servomotorn till pin 9 samt sätter pin 2 som input.*/
void setup() {
  Wire.begin(); // Start I2C

  Serial.begin(9600); // Starta seriell kommunikation med baudhastigheten 9600 aka skärm

  // Skapa ett RtcDateTime-objekt med kompileringsdatum och tid
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  Rtc.SetDateTime(compiled); // Sätt datum och tid på realtidsklockan (RTC)

  oled.setFont(u8g_font_helvB10); // Ange font för OLED-skärmen
  pinMode(2, INPUT); // Sätt pin 2 som ingång

  myservo.attach(9); // Koppla servomotorn till pin 9

}

/*
 Uppdaterar RTC och sätter in String med timma, minut och sekund med aktuell tid
 kontrollerar så att pin2 har hög signal där knappen kopplas in.
 När knappen trycks in uppdaterar skärmen med "(>_<)" och väntar i 500 millisekunder
 */
void loop() {
  updateRTC(); // Uppdatera realtidsklockan (RTC)
  updateOled(String(hour) + ":" + String(minute) + ":" + String(second)); // Uppdatera OLED-skärmen med aktuell tid
  updateServo(); // Uppdatera servomotorns position baserat på variabeln tmp

  // Kontrollera om det finns en hög signal på pin 2
  if (digitalRead(2) == 1) {
    updateOled ("(>_<)"); // Uppdatera OLED-skärmen med emojin "(>_<)"
    delay(500); // sätt delay på skärmen med 500 millisekunder
  }
}

/* Hämtar datum och tid från RTC och uppdaterar det till Hour, minute och second
samt hämtas temperatur från RTC och konverteras till celcius i tmp variabeln*/
void updateRTC () {
  // hämta aktuell datum och tid från RTC o lagra det i en variabel 'now'
  RtcDateTime now = Rtc.GetDateTime();
  hour = now.Hour(); 
  minute = now.Minute(); 
  second = now.Second(); 

  RtcTemperature rtcTemp = Rtc.GetTemperature (); // Hämta temperaturdata från realtidsklockan och lagra i rtcTemp
  tmp = rtcTemp.AsFloatDegC();
  // Konvertera temperaturdatan till Celsius-grader och lagra i variabeln tmp

}

/* Uppdaterar skärmens text, samt placerar ut 
texten på skärmen.
params: text som ska skrivas ut i skärmen av typen string
return: void
  */
void updateOled (String text) {
  oled.firstPage();
  do {
    oled.drawStr(40, 50, text.c_str());
  } while (oled.nextPage());


}
/*Uppdaterar servon, temperaturen bestämmer antal grader
som servo motorn snurra*/
void updateServo() {
  myservo.write(map(tmp, 20, 25, 0, 90));

}
