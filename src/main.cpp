// WIFI Logindata TODO: über Drehregler WIFI Logindaten eingeben und in den EPROM abspeichern

// OPTIMIZE: describe code that is inefficient and may become a bottleneck
// HACK: describe the use of a questionable (or ingenious) coding practice
// REVIEW: describe code that should be reviewed to confirm implementatio

//TODO: 
// #include <cstdint>
// int8_t bla = 15;
//byte verwenden

#include <Arduino.h>
#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <mdns.h>
#include <webServer.h>
#include <Wire.h>
#include <RTClib.h>
  RTC_DS3231 rtc;
  DateTime rtctime;
  String rtcHour;
  String rtcMinute;
  String rtcSecond;
  String rtcDay;
  String rtcMonth;
  String rtcYear;
  String rtcWholeTime;
  String rtcWholeDate;
  String rtcUnixtime;
  const char daysOfTheWeek[7][12] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};

// Wifi Settings
#include "credentials.h"
const char ssid[] = WIFI_SSID;        
const char pass[] = WIFI_PASSWD;

// Variables to save date and time
String formattedDate;

unsigned long oldMillis = millis();
bool oneSecondLater = false;

//RotaryEncoder
unsigned int encoderButtonDebounceTime = 800;

// IO Pins
  #define buzPin D8
  //const int LDRpin = A0;
  #define LDRpin A0 //TODO: #define analog pin geht auch??
  #define LCDdimmPin 2

  #define encoderPinA D5
  #define encoderPinB D6
  #define encoderPinPush D7

//Split Code
#include <ntp.h>
  int ntpUpdateTime[2] = {21, 56};    // um wieviel Uhr die Synchronisation mit dem NTP stattfinden soll
  int splitT; // Position des Buchstabens T im String
  int ntpYear;  //.substring() parst Substring // .toInt() wandelt String in Integer umDateTime rtctime
  int ntpMonth;
  int ntpDay;
  int ntpHour;
  int ntpMinute;
  int ntpSecond;

#include <summertime_eu.h>
#include <ldr_dimm.h>
#include <digitalClock.h>
#include <lcd.h>
  #include <rotaryEncoder.h>
    #include <buzzer.h>
    #include <menu2004.h>
#include <alarm.h>
  alarm monday;
  alarm tuesday;
  alarm wednesday;
  alarm thursday;
  alarm friday;
 	alarm saturday;
 	alarm sunday;
  bool alarmTimeChanged = false;

void setDefaultAlarms(){
  monday.setAlarm(1, 6, 30, 0);   // Alle Weckzeiten sind deaktiviert
  tuesday.setAlarm(2, 6, 30, 0);
  wednesday.setAlarm(3, 6, 30, 0);
  thursday.setAlarm(4, 6, 30, 0);
  friday.setAlarm(5, 6, 30, 0);
  saturday.setAlarm(6, 6, 30, 0);
  sunday.setAlarm(0, 6, 30, 0);     // rtctime.dayOfTheWeek() gibt am Sonntag 0 aus Montag = 1, Dienstag = 2, usw
}

#include <myEeprom.h>
#include <serialInput.h>

// #include <audio.h> //TODO: AUDIO an


void checkForRtcPower(){
  if (rtc.lostPower()) {
    static int connectionTries = 0;
    Serial.println("Stromversorgung der RT Uhr wurde getrennt.");
    Serial.println("Versuche mit NTP zu synchronisieren ");
    while ( connectionTries <= 9 && rtc.lostPower()){  // 10 Versuche und solang rtc.lostPower Bit noch gesetzt ist
      Serial.print(".");
      connectionTries++;
      if (WiFi.status() == WL_CONNECTED){
          Serial.println("Synchronisiere ...");        
          timeClient.update();
          rtc.adjust( DateTime(timeClient.getEpochTime()) );
          // connectionTries = 0;  // wieder zurücksetzen, falls später die RT Uhr erneut ausfällt   //FIXME: Kommentar entfernen ... 
      }
      else if (connectionTries == 10) Serial.println("Keine Wifi Verbindung! RT Uhr konnte nicht gestellt werden.");
    }
  }
}

void checkAndFixNTPtime(){      // Check Time for Errors
    if(timeClient.getEpochTime() < 4000 ){      //TODO: exaktere Überprüfung!!
      Serial.println("Uhrzeit fehlerhaft");
      if (WiFi.status() == WL_CONNECTED){
        Serial.println("Synchronisiere mit NTP");        
        timeClient.update();
        rtc.adjust( DateTime(timeClient.getEpochTime()) );
      }
    else
      Serial.println("Kann nicht mit NTP synchronisieren. WLAN getrennt!");
    }
}


void extractNTPtime(){
    checkAndFixNTPtime();
    // Extract NTP date and time
    // The formattedDate comes with the following format: 2018-05-28T16:00:13Z
    formattedDate = timeClient.getFormattedDate();
    splitT = formattedDate.indexOf("T"); // Position des Buchstabens T im String
    ntpYear = formattedDate.substring(0, 4).toInt();  //.substring() parst Substring // .toInt() wandelt String in Integer um
    ntpMonth = formattedDate.substring(5, 7).toInt();
    ntpDay = formattedDate.substring(8, 10).toInt();
    ntpHour = formattedDate.substring(splitT+1, splitT+3).toInt();
    ntpMinute = formattedDate.substring(splitT+4, splitT+6).toInt();
    ntpSecond = formattedDate.substring(splitT+7, splitT+9).toInt();
}

void checkAndFixTimechange(){    
  if (summertime != summertime_EU(ntpYear, ntpMonth, ntpDay, ntpHour, 1)){ // falls Zeitumstellung    // TODO: testen
          summertime = summertime_EU(ntpYear, ntpMonth, ntpDay, ntpHour, 1);
          if (summertime == 1){
            timeClient.setTimeOffset(timeZone*3600 + 3600); // +1h dazu
            Serial.println("## Sommerzeit");
          }
          else{ 
            timeClient.setTimeOffset(timeZone*3600);
            Serial.println("## Winterzeit");
          }
          rtc.adjust( DateTime(timeClient.getEpochTime()));
    }
}

void getRTCtime(){
    rtctime = rtc.now();    // Muss in dieser Funktion enthalten sein. rtctime ist zwar global deklariert, aber scheinbar wird der Inhalt rtctime nicht über Funktionen hinweg übertragen 
    rtcHour =  String(rtctime.hour(), DEC);
    rtcMinute = printDigits(rtctime.minute());
    rtcSecond = printDigits(rtctime.second());
    rtcDay =  String(rtctime.day(), DEC);
    rtcMonth =  String(rtctime.month(), DEC);
    rtcYear =  String(rtctime.year(), DEC);
    rtcWholeTime = String(rtcHour + ":" + rtcMinute + ":" + rtcSecond);
    rtcWholeDate = String(String(daysOfTheWeek[rtctime.dayOfTheWeek()]) + " " + rtcDay + "." + rtcMonth + "." + rtcYear);
    rtcUnixtime = String(rtctime.unixtime());

    static unsigned int oldSecond = 0;
    if ( rtctime.second() != oldSecond){  // löst jeweils nach 1s 1x aus
      oneSecondLater = true;
      oldSecond = rtctime.second();
    }
    else oneSecondLater = false;
}

void showClockOnLCD(){
  if (!inMenu && oneSecondLater){
    static unsigned int startRow;
    startRow = LCDcenter_startRow(rtcWholeTime, lcd_width);
    lcd.setCursor(startRow, 0);
    lcd.print(rtcHour + ":" + rtcMinute + ":" + rtcSecond);

    startRow = LCDcenter_startRow(rtcWholeDate, lcd_width);
    lcd.setCursor(startRow, 1);
    lcd.print(rtcWholeDate);
  }
}

void syncNTPtoRTC(){
  //rtctime = rtc.now();   // muss enthalten sein. siehe Kommentar bei getRTCtime()
  //String temp3 =  String(rtctime.dayOfTheWeek(), DEC);
  // Serial.println(rtcHour);
  // Serial.println("_______________");
  //Serial.println(String(rtctime.minute()));
  if (WiFi.status() == WL_CONNECTED && rtctime.hour() == ntpUpdateTime[0] && rtctime.minute() == ntpUpdateTime[1] && rtctime.second() == 0 && oneSecondLater)
  {       
      Serial.println("Versuche NTP zu aktualisieren und RTC zu synchronisieren");                        
      timeClient.update();
      rtc.adjust( DateTime(timeClient.getEpochTime()) );
  }
}

void checkAlarm(){  //FIXME: rtctime.*** geht??
  //Serial.println("checkAlarm");
    //rtctime = rtc.now(); // muss enthalten sein. siehe Kommentar bei getRTCtime()

    if (rtctime.second() == 0 && oneSecondLater){ // Überprüft nur zu jeder vollen Minute 
      if (cfg.alarmMon[3] == true && cfg.alarmMon[0] == rtctime.dayOfTheWeek() && cfg.alarmMon[1] == rtctime.hour() && cfg.alarmMon[2] == rtctime.minute() ) 
          alarmRing(); 
      else if (cfg.alarmTue[3] == true && cfg.alarmTue[0] == rtctime.dayOfTheWeek() && cfg.alarmTue[1] == rtctime.hour() && cfg.alarmTue[2] == rtctime.minute() ) 
          alarmRing(); 
      else if (cfg.alarmWed[3] == true && cfg.alarmWed[0] == rtctime.dayOfTheWeek() && cfg.alarmWed[1] == rtctime.hour() && cfg.alarmWed[2] == rtctime.minute() ) 
          alarmRing();
      else if (cfg.alarmThu[3] == true && cfg.alarmThu[0] == rtctime.dayOfTheWeek() && cfg.alarmThu[1] == rtctime.hour() && cfg.alarmThu[2] == rtctime.minute() ) 
          alarmRing();
      else if (cfg.alarmFri[3] == true && cfg.alarmFri[0] == rtctime.dayOfTheWeek() && cfg.alarmFri[1] == rtctime.hour() && cfg.alarmFri[2] == rtctime.minute() ) 
          alarmRing();
      else if (cfg.alarmSat[3] == true && cfg.alarmSat[0] == rtctime.dayOfTheWeek() && cfg.alarmSat[1] == rtctime.hour() && cfg.alarmSat[2] == rtctime.minute() ) 
          alarmRing();
      else if (cfg.alarmSun[3] == true && cfg.alarmSun[0] == rtctime.dayOfTheWeek() && cfg.alarmSun[1] == rtctime.hour() && cfg.alarmSun[2] == rtctime.minute() ) 
          alarmRing();
    }
}

#include <terminal.h>

void setup() 
{
  //Drehregler + Interrupt
  pinMode (encoderPinA, INPUT_PULLUP);
  pinMode (encoderPinB, INPUT_PULLUP); 
  pinMode (encoderPinPush, INPUT_PULLUP); 
  attachInterrupt (encoderPinA, fireRotaryEncoder, CHANGE);  
  attachInterrupt (encoderPinPush, fireEncoderButton, CHANGE);  

  //LCD
  lcd.begin(lcd_width, lcd_hight);
  lcd.backlight();

  //Serial
  Serial.begin(9600);
  
  //RTC Teil 1
  if (! rtc.begin()) {
    Serial.println("Kann RTC nicht finden.");
    while (1);
  }
  

  // WIFI + NTP Sync
  Serial.print("Verbinde mit ");
  Serial.print(ssid);
  WiFi.begin(ssid, pass);
  // Wifi Verbindungsversuche auf 30s beschränken
  for (int i = 0; i<60 && WiFi.status() != WL_CONNECTED; i++){
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED){
    Serial.println();
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.println("Starte NTP-Abgleich");
    timeClient.begin();
    timeClient.update();
    timeClient.setTimeOffset(timeZone*3600);    // Winterzeit als Standardzeit
    Serial.println("Stelle RT Uhr.");
    rtc.adjust( DateTime(timeClient.getEpochTime()));
  }
  else Serial.print("Keine Wifiverbindung.");
 
  // EEPROM
  loadEEPROMConfigIntoRAM();
  
  // cfg.valid = 0;
  if (loadAllEEPROMAlarmsToLocalAlarms() == false) { //falls cfg.valid = 0
    Serial.println("Weckzeiten in EEPROM sind fehlerhaft. Ubertrage Standardweckzeiten");
    setDefaultAlarms();
    safeAllAlarmsToEEPROM();
  }

  //Terminalausgabe
  showHelp();

  //mDNS Dienst
  MDNS.setHostProbeResultCallback(hostProbeResult);   //TODO: mDNS: code verschlanken + Ausgaben unterdrücken
  // Init the (currently empty) host domain string with 'esp8266'
  if ((!MDNSResponder::indexDomain(pcHostDomain, 0, "weckbot")) ||
      (!MDNS.begin(pcHostDomain))) {
    Serial.println(" Error setting up MDNS responder!");
    while (1) { // STOP
      delay(1000);
    }
  }
  Serial.println("MDNS responder started");

  //WebServer
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();


  // Audio  //TODO: AUDIO an
  // delay(3000);

  // // This can be set in the IDE no need for ext library
  // // system_update_cpu_freq(160);
      
  // Serial.println("\n\nSimple Radio Node WiFi Radio");
      
  // SPI.begin();
  // player.begin();
  // player.switchToMp3Mode();
  // player.setVolume(VOLUME);
  // Serial.print("connecting to ");  Serial.println(host);
  // if (!client.connect(host, httpPort)) {
  //   Serial.println("Connection failed");
  //   return;
  // }
  // Serial.print("Requesting stream: ");
  // Serial.println(path);
  // client.print(String("GET ") + path + " HTTP/1.1\r\n" +
  //               "Host: " + host + "\r\n" + 
  //               "Connection: close\r\n\r\n");

}

void loop(void)
{
  // reconnectToWebRadio(); //TODO: AUDIO an
  // streamWebRadio();
  checkForRtcPower();
  getSerialInputChar();
  extractNTPtime();
  getRTCtime();
  checkAndFixTimechange();
  syncNTPtoRTC();
  checkAlarm();

  MDNS.update();
  server.handleClient();  // TODO: Weckzeiten übertragen

  terminal();

  if (alarmTimeChanged ){
    Serial.println("Speichere geänderte Weckzeit im EEPROM");
    safeAllAlarmsToEEPROM();
    loadEEPROMConfigIntoRAM();
    alarmTimeChanged = false;
  }

  showClockOnLCD();

  recognizeRotary();

  if (encoderButtonFired && !inMenu && millis() - oldMillis > encoderButtonDebounceTime){ // TODO: Debounce zentral in rotaryEncoder.h vornehmen ... Nötig damit das Menü beim Beenden nicht sofort wieder öffnet
    oldMillis = millis();
    encoderButtonFired = false;
    encoderCount = 1;  //damit  das Menü ganz oben beginnt
    inMenu = 1;
    mainmenuActive = 1;
    updateMainMenu();
  }

  if (inMenu){
    if (encoderCountChange){
      drawMenu();
      encoderCountChange = 0;
    }
    if (encoderButtonFired && millis() - oldMillis > encoderButtonDebounceTime){
      oldMillis = millis();
      encoderButtonFired = false;
      executeAction();
    }
  }
}