// OPTIMIZE: describe code that is inefficient and may become a bottleneck
// HACK: describe the use of a questionable (or ingenious) coding practice
// REVIEW: describe code that should be reviewed to confirm implementatio

//TODO: WIFI Logindata über Drehregler WIFI Logindaten eingeben und in den EPROM abspeichern
// main aufräumen: --> RTC Komponenten komplett auslagern!

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

//RotaryEncoder


// IO Pins
#define buzPin D8
//const int LDRpin = A0;
#define LDRpin A0 //TODO: #define analog pin geht auch??
#define LCDdimmPin 2
#define encoderPinA D5
#define encoderPinB D6
#define encoderPinPush D7

int timeZone = 1;     // Central European Time
//const int timeZone = -5;  // Eastern Standard Time (USA)
//const int timeZone = -4;  // Eastern Daylight Time (USA)
//const int timeZone = -8;  // Pacific Standard Time (USA)
//const int timeZone = -7;  // Pacific Daylight Time (USA)
int ntpUpdateTime[2] = {21, 56};    // um wieviel Uhr die Synchronisation mit dem NTP stattfinden soll
#include <clock.h>

#include <ldr_dimm.h>
#include <lcd.h>
unsigned int encoderButtonDebounceTime = 800;
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
  initializeOTA();

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
  ArduinoOTA.handle();
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