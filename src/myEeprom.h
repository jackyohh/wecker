#include <EEPROM.h>
#include "myTypes.h"

int cfgStart = 0;
configData_t cfg;

void eraseEEPROM() {
  // Reset EEPROM bytes to '0' for the length of the data structure
  EEPROM.begin(4095);
  for (int i = cfgStart ; i < sizeof(cfg) ; i++) {
    EEPROM.write(i, 0);
  }
  delay(200);
  EEPROM.commit();
  EEPROM.end();
}


void saveToEEPROM() {
  // Save configuration from RAM into EEPROM
  EEPROM.begin(4095);
  EEPROM.put( cfgStart, cfg );
  delay(200);
  EEPROM.commit();                      // Only needed for ESP8266 to get data written
  EEPROM.end();                         // Free RAM copy of structure
}

void loadEEPROMConfigIntoRAM() {
  // Loads configuration from EEPROM into RAM
  EEPROM.begin(4095);
  EEPROM.get( cfgStart, cfg );
  EEPROM.end();
}

int loadAllEEPROMAlarmsToLocalAlarms() {
  if ( cfg.valid ) { 
  // Loads configuration from Ram into Class alarm
    monday.setAlarm(cfg.alarmMon[0], cfg.alarmMon[1], cfg.alarmMon[2], cfg.alarmMon[3]);
    tuesday.setAlarm(cfg.alarmTue[0], cfg.alarmTue[1], cfg.alarmTue[2], cfg.alarmTue[3]);
    wednesday.setAlarm(cfg.alarmWed[0], cfg.alarmWed[1], cfg.alarmWed[2], cfg.alarmWed[3]);
    thursday.setAlarm(cfg.alarmThu[0], cfg.alarmThu[1], cfg.alarmThu[2], cfg.alarmThu[3]);
    friday.setAlarm(cfg.alarmFri[0], cfg.alarmFri[1], cfg.alarmFri[2], cfg.alarmFri[3]);
    saturday.setAlarm(cfg.alarmSat[0], cfg.alarmSat[1], cfg.alarmSat[2], cfg.alarmSat[3]);
    sunday.setAlarm(cfg.alarmSun[0], cfg.alarmSun[1], cfg.alarmSun[2], cfg.alarmSun[3]);
    return true;
  }
  else return false;
}

void safeAllAlarmsToEEPROM(){
  cfg.alarmMon[0] = monday.getDay();
  cfg.alarmMon[1] = monday.getHour();
  cfg.alarmMon[2] = monday.getMinute();
  cfg.alarmMon[3] = monday.getAlarmStatus(); 

  cfg.alarmTue[0] = tuesday.getDay();
  cfg.alarmTue[1] = tuesday.getHour();
  cfg.alarmTue[2] = tuesday.getMinute();
  cfg.alarmTue[3] = tuesday.getAlarmStatus(); 

  cfg.alarmWed[0] = wednesday.getDay();
  cfg.alarmWed[1] = wednesday.getHour();
  cfg.alarmWed[2] = wednesday.getMinute();
  cfg.alarmWed[3] = wednesday.getAlarmStatus(); 

  cfg.alarmThu[0] = thursday.getDay();
  cfg.alarmThu[1] = thursday.getHour();
  cfg.alarmThu[2] = thursday.getMinute();
  cfg.alarmThu[3] = thursday.getAlarmStatus(); 

  cfg.alarmFri[0] = friday.getDay();
  cfg.alarmFri[1] = friday.getHour();
  cfg.alarmFri[2] = friday.getMinute();
  cfg.alarmFri[3] = friday.getAlarmStatus();  

  cfg.alarmSat[0] = saturday.getDay();
  cfg.alarmSat[1] = saturday.getHour();
  cfg.alarmSat[2] = saturday.getMinute();
  cfg.alarmSat[3] = saturday.getAlarmStatus(); 

  cfg.alarmSun[0] = sunday.getDay();
  cfg.alarmSun[1] = sunday.getHour();
  cfg.alarmSun[2] = sunday.getMinute();
  cfg.alarmSun[3] = sunday.getAlarmStatus();
  cfg.valid = 1;  
  saveToEEPROM();
}

void safeAlarm(){
    Serial.println("Speichere geänderte Weckzeit im EEPROM");
    safeAllAlarmsToEEPROM();
    loadEEPROMConfigIntoRAM();
}
// Parameter im Programm benutzen

// Im Sketch muß man nach dem Boot in setup() die Parameter vom EEPROM ins RAM laden:

// loadConfig();

// und kann jetzt auf sie zugreifen und sie ggf. auch modifizieren:

// Serial.print(cfg.ssid); // Ausgeben des Werts auf der Konsole

//   int
//   cfg.valid = 1;
  
// char Array
// strcpy(cfg.ssid, "TEST");
  
// int Array kopieren --> cfg.alarmMon[] } {1, 7 ,30}; geht NICHT! --> weiteres Array erzeugen und mmit memcpy und sizeof kopieren
// int alarmMon2[3] ={ 1, 7, 30 };
// memcpy(cfg.alarmMon, alarmMon2, sizeof(alarmMon2));
  

// Wichtig ist, dass der Parameter aus der Datenstruktur vom Prefix 'cfg.' angeführt wird.






  // EEPROM
    // Serial.println("EEPROM: Variablen übergeben");

    // cfg.valid = 0;
    
    // char Array
    // strcpy(cfg.ssid, "TEST");
    
    // int Array kopieren --> cfg.alarmMon[] } {1, 7 ,30}; geht NICHT! --> weiteres Array erzeugen und mmit memcpy und sizeof kopieren
    // int alarmMon2[3] ={ 5, 3, 20 };
    // memcpy(cfg.alarmMon, alarmMon2, sizeof(alarmMon2));
    
    // memcpy(cfg.ssid, ssid, sizeof(ssid));
    // memcpy(cfg.pass, pass, sizeof(pass));

    // saveConfig();

    // // 
    // loadConfig();
    // Serial.println(cfg.valid);
    // Serial.println(cfg.alarmMon[1]);
    // Serial.println(cfg.alarmMon[2]);
    // Serial.println(cfg.alarmMon[3]);
    // Serial.println(cfg.ssid);
    // Serial.println(cfg.pass);