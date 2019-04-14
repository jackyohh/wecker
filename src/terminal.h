
  bool inTerminalEnableAlarm = 0;
  bool inTerminalDisableAlarm = 0;

void showHelp(){
    Serial.println();
    Serial.println("Hilfe :");
    Serial.println();
    Serial.println("h Zeigt diese an Hilfe an");
    Serial.println("----------------------------------");
    Serial.println("a Zeige Weckzeiten an (aus EEPROM)");
    Serial.println("e Weckzeiten anschalten");  
    Serial.println("d Weckzeiten ausschalten");
    Serial.println("n Neue Weckzeit stellen");        //FIXME: funktioniert nicht richtig
    Serial.println("s Übertrage Standardweckzeiten");
    Serial.println("----------------------------------");
    Serial.println("t Zeige NTP und RTC Zeit an");
    Serial.println("c Zeige Sommer-/Winterzeit an");
    Serial.println("u Synchronisiere RTC mit NTP Server");
    Serial.println("----------------------------------");
    Serial.println("r Lösche EEPROM");
    Serial.println();
}

void parseAlarm(struct array z) {   // TODO: for(i = 0; i < sizeof(struct array) / sizeof(int); i++) verwenden
    // Serial.println("Starte Parsing");
    int tag;
    if ( isDigit(z.wert[0])) tag = char2int(z.wert[0]);
    else return;
    int i;
    for (i = 1; i < 7 && isDigit(z.wert[i]); i++)
        tag = tag*10 + char2int(z.wert[i]);
    int stunde = char2int(z.wert[i++]); 
    for ( i; i < 7 && isDigit(z.wert[i]) ; i++){
      stunde = stunde*10 + char2int(z.wert[i]);
    }
    int minute = char2int(z.wert[i++]);
    for ( i; i < 7 && isDigit(z.wert[i]) ; i++){
      minute = minute*10 + char2int(z.wert[i]);
    }
    Serial.println(tag);
    Serial.println(stunde);
    Serial.println(minute);   
    if ( tag > 7 || stunde > 23 || minute > 59 ) {
      Serial.println("falsche Eingabe");  }
    else {
      Serial.print("Neue Weckzeit: ");
      if (tag == 7) Serial.print("Sonntag");    
      else Serial.print(String(daysOfTheWeek[tag]));
      Serial.println(" " + String(stunde) + ":" + String(minute) + " Uhr");

      switch (tag) {
        case 0:   // 0 ist Sonntag
          monday.setAlarm(tag, stunde, minute, 1);
          break;
        case 1:
          monday.setAlarm(tag, stunde, minute, 1);
          break;
        case 2:
          tuesday.setAlarm(tag, stunde, minute, 1);
          break;
        case 3:
          wednesday.setAlarm(tag, stunde, minute, 1);
          break;
        case 4:
          thursday.setAlarm(tag, stunde, minute, 1);
          break;
        case 5:
          friday.setAlarm(tag, stunde, minute, 1);
          break;
        case 6:
          saturday.setAlarm(tag, stunde, minute, 1);
          break;
        case 7:
          sunday.setAlarm(0, stunde, minute, 1);      // 0 ist Sonntag
          break;
      }
      safeAllAlarmsToEEPROM();
      loadEEPROMConfigIntoRAM();
    }
}


void terminalEnableAlarm(){
  if (newSerialInputChar()){
    if ( SerialInputChar == '0' ) {
      monday.enableAlarm(); 
      tuesday.enableAlarm();  
      wednesday.enableAlarm();
      thursday.enableAlarm(); 
      friday.enableAlarm(); 
      saturday.enableAlarm();
      sunday.enableAlarm(); 
      Serial.println("Alle Weckzeiten von Mo bis So aktiviert.");     
    }
    if ( SerialInputChar == '1' ) {
      monday.enableAlarm(); Serial.println("Montag: Alarm AN");}
    else if ( SerialInputChar == '2' ) {
      tuesday.enableAlarm(); Serial.println("Dienstag: Alarm AN");}
    else if ( SerialInputChar == '3' ) {
      wednesday.enableAlarm(); Serial.println("Mittwoch: Alarm AN");}
    else if ( SerialInputChar == '4' ) {
      thursday.enableAlarm(); Serial.println("Donnerstag: Alarm AN");}
    else if ( SerialInputChar == '5' ) {
      friday.enableAlarm(); Serial.println("Freitag: Alarm AN");}
    else if ( SerialInputChar == '6' ) {
      saturday.enableAlarm(); Serial.println("Samstag: Alarm AN");}
    else if ( SerialInputChar == '7' ) {
      sunday.enableAlarm(); Serial.println("Sonntag: Alarm AN");}
    if ( SerialInputChar == 'q' ) {
      inTerminalEnableAlarm = false;
      safeAlarm();
      showHelp();
    }
    Serial.print("> ");
  }
}

void terminalDisableAlarm(){
  //sSerialInputChar = ' ';
  if (newSerialInputChar()){
    if ( SerialInputChar == '0' ) {
      monday.disableAlarm();  
      tuesday.disableAlarm();  
      wednesday.disableAlarm();
      thursday.disableAlarm(); 
      friday.disableAlarm(); 
      saturday.disableAlarm();
      sunday.disableAlarm(); 
      Serial.println("Alle Weckzeiten von Mo bis So deaktiviert.");        
      }
    if ( SerialInputChar == '1' ) {
      monday.disableAlarm(); Serial.println("Montag: Alarm AUS");}
    else if ( SerialInputChar == '2' ) {
      tuesday.disableAlarm(); Serial.println("Dienstag: Alarm AUS");}
    else if ( SerialInputChar == '3' ) {
      wednesday.disableAlarm(); Serial.println("Mittwoch: Alarm AUS");}
    else if ( SerialInputChar == '4' ) {
      thursday.disableAlarm(); Serial.println("Donnerstag: Alarm AUS");}
    else if ( SerialInputChar == '5' ) {
      friday.disableAlarm(); Serial.println("Freitag: Alarm AUS");}
    else if ( SerialInputChar == '6' ) {
      saturday.disableAlarm(); Serial.println("Samstag: Alarm AUS");}
    else if ( SerialInputChar == '7' ) {
      sunday.disableAlarm(); Serial.println("Sonntag: Alarm AUS");}
    if ( SerialInputChar == 'q' ) {
      inTerminalDisableAlarm = false;
      safeAlarm();
      showHelp();
    }
  Serial.print("> ");
  }
}

void terminal(){
  if (inTerminalEnableAlarm){
    terminalEnableAlarm();
    return;
  }
  if (inTerminalDisableAlarm){
    terminalDisableAlarm();
    return;
  }

  if (newSerialInputChar())  {
    if ( SerialInputChar == 'a' ) {
      SerialInputChar = ' ';
      Serial.println();
      Serial.println("CFG.Alarm-Settings:");
      Serial.println("Gültige Einstellungen :" + String(cfg.valid));
      Serial.println("Montag:     " + String(cfg.alarmMon[1]) + ":" + String(cfg.alarmMon[2]) + " - AN:"+ String(cfg.alarmMon[3]));
      Serial.println("Dienstag:   " + String(cfg.alarmTue[1]) + ":" + String(cfg.alarmTue[2]) + " - AN:"+ String(cfg.alarmTue[3]));
      Serial.println("Mittwoch:   " + String(cfg.alarmWed[1]) + ":" + String(cfg.alarmWed[2]) + " - AN:"+ String(cfg.alarmWed[3]));
      Serial.println("Donnerstag: " + String(cfg.alarmThu[1]) + ":" + String(cfg.alarmThu[2]) + " - AN:"+ String(cfg.alarmThu[3]));
      Serial.println("Freitag:    " + String(cfg.alarmFri[1]) + ":" + String(cfg.alarmFri[2]) + " - AN:"+ String(cfg.alarmFri[3]));
      Serial.println("Samstag:    " + String(cfg.alarmSat[1]) + ":" + String(cfg.alarmSat[2]) + " - AN:"+ String(cfg.alarmSat[3]));
      Serial.println("Sonntag:    " + String(cfg.alarmSun[1]) + ":" + String(cfg.alarmSun[2]) + " - AN:"+ String(cfg.alarmSun[3]));
      Serial.println();
    }

    if ( SerialInputChar == 'e' ) {
      Serial.println();
      Serial.println("An welchem Tag soll der Alarm eingeschaltet werden? Gib den Wochentag als Zahlenwert an (0 = alle Tage). q zum Verlassen.");
      inTerminalEnableAlarm = true;
      terminalEnableAlarm();
    }

    if ( SerialInputChar == 'd' ) { 
      Serial.println();
      Serial.println("An welchem Tag soll der Alarm ausgeschaltet werden? Gib den Wochentag als Zahlenwert an (0 = alle Tage). q zum Verlassen."); 
      inTerminalDisableAlarm = true;
      terminalDisableAlarm();
    }

    if ( SerialInputChar == 'n' ) {   
      //SerialInputChar = ' ';
      Serial.println();
      Serial.println("Gib eine neue Weckzeit ein und beende mit ';' <Tag> <Stunde> <Minute>;");
      
      //
      unsigned long oldMillis = millis();
      struct array tmp;
      Serial.setTimeout(2000); // anderenfalls würde nach 1000ms die Eingabe unterbrochen
      while(!endOfSerialInput && millis() - oldMillis < 30*1000) {//Timeout 30 Sekunden
        tmp = getSerialInputArray(); 
        oldMillis = millis();
      }
      endOfSerialInput = false; 
      parseAlarm(tmp);
    }

    if ( SerialInputChar == 's' ) {
      SerialInputChar = ' ';
      Serial.println("Übertrage Standardweckzeiten.");
      setDefaultAlarms();
      safeAllAlarmsToEEPROM();
    }

    if ( SerialInputChar == 't' ) {
      SerialInputChar = ' ';
      Serial.println("NTP:");
      Serial.println("NTP-EpochTime: " + String(timeClient.getEpochTime()) + " s seit 1. Januar 1970");
      Serial.println("NTP-Time     : " + timeClient.getFormattedTime());
      Serial.println("NTP-Date     : " + String(ntpDay) + "." + String(ntpMonth) + "." + String(ntpYear));  
      Serial.println("------------------------------");
      Serial.println("RTC:");
      Serial.println("RTC-Time     : " + rtcWholeTime);
      Serial.println("RTC-Date     : " + rtcWholeDate);
      Serial.println("RTC-Unixtime : " + rtcUnixtime);
      Serial.println();
    }

    if ( SerialInputChar == 'c' ) {   
      SerialInputChar = ' ';
      if(isSummertime())
        Serial.println("Sommerzeit");
      else Serial.println("Winterzeit");
      Serial.println();
    }

    if ( SerialInputChar == 'u' ) {
      SerialInputChar = ' ';
      syncNTPtoRTC();
      Serial.println(); //TODO: zeitlichen Versatz ausgeben
    }

    if ( SerialInputChar == 'r' ) {
      SerialInputChar = ' ';
      Serial.println("Loesche EEPROM.");
      eraseEEPROM();
      loadEEPROMConfigIntoRAM();
    }

    if ( SerialInputChar == 'h' ) {
        SerialInputChar = ' ';
        showHelp();
    }
    Serial.print("> ");
  }
}