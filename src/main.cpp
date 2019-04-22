// OPTIMIZE: describe code that is inefficient and may become a bottleneck
// HACK: describe the use of a questionable (or ingenious) coding practice
// REVIEW: describe code that should be reviewed to confirm implementatio

//TODO: WIFI Logindata über Drehregler WIFI Logindaten eingeben und in den EPROM abspeichern
// main aufräumen: --> RTC Komponenten komplett auslagern!


//TODO:
// Achtung: Da die Funktion millis() einen Wert zurückgibt, der 32 Bit breit ist, kann es je nachdem, wie man die Bedingung formuliert, nach rund 49 Tagen zu Überlaufproblemen kommen.
// Ungünstig:
//  if(millis() < millisMem + …)
// Besser:
//  if(millis() - millisMem > …)


#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <mdns.h>
#include <ota.h>
#include <webServer.h>
#include <Wire.h>

// Wifi Settings
#include "credentials.h"
char ssid[] = WIFI_SSID;        
char pass[] = WIFI_PASSWD;

unsigned long oldMillis = millis();

//const int LDRpin = A0;
#define LDRpin A0 //TODO: #define analog pin geht auch??
#define LCDdimmPin 2
#define encoderPinA D5
#define encoderPinB D6
#define encoderPinPush D7

#include <rotaryEncoder.h>

int timeZone = 1;     // Central European Time; -5 Eastern Standard Time (USA); -4 = Eastern Daylight Time (USA) 
int ntpUpdateTime[2] = {21, 56};    // um wieviel Uhr die Synchronisation mit dem NTP stattfinden soll

#include <clock.h>
#include <ldr_dimm.h>
#include <lcd.h>
#include <buzzer.h>
#include <alarmCl.h>
#include <alarm.h>
#include <myEeprom.h>
#include <terminal.h>

// #include <audio.h> //TODO: AUDIO an

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

void checkAlarm(){  //FIXME: rtctime.*** geht??
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

void setup() {
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
    Serial.println("Weckzeiten in EEPROM sind fehlerhaft. Übertrage Standardweckzeiten");
    setDefaultAlarms();
    safeAllAlarmsToEEPROM();
  }

  //mDNS Dienst
  MDNS.setHostProbeResultCallback(hostProbeResult);
  // Init the (currently empty) host domain string with 'esp8266'
  if ((!MDNSResponder::indexDomain(pcHostDomain, 0, "weckbot")) ||
      (!MDNS.begin(pcHostDomain))) {
    Serial.println(" Error setting up MDNS responder!");
    while (1) { // STOP
      delay(1000);
    }
  }
  Serial.println("MDNS responder started");
  initializeOTA();

  //WebServer
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();

  //Terminalausgabe
  showHelp();
}

void loop(void)
{
  // reconnectToWebRadio(); //TODO: AUDIO an
  // streamWebRadio();
  checkForRtcPower();
  getRTCtime();
  extractNTPtime();

  checkAndFixTimechange();
  dailySyncNTPtoRTC();

  checkAlarm();

  MDNS.update();
  ArduinoOTA.handle();
  server.handleClient();  // TODO: Weckzeiten übertragen
  
  terminal();

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