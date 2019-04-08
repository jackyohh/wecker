
void showHelp(){
    Serial.println();
    Serial.println("Hilfe :");
    Serial.println();
    Serial.println("h Zeigt diese an Hilfe an");
    Serial.println("a Neue Weckzeit stellen");
    Serial.println("c Zeige Weckzeiten an (aus EEPROM)");
    Serial.println("d Alarm ausschalten");
    Serial.println("e Alarm anschalten");
    Serial.println("r Loesche EEPROM");
    Serial.println("s Uebertrage Standardweckzeiten");
    Serial.println("t Zeige NTP und RTC Zeit an");
    Serial.println();
    Serial.println("> ");
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


void terminal(){
  if (newSerialInput)  {
    if ( SerialInputChar == 'a' ) {   
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

    if ( SerialInputChar == 'c' ) {
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
      SerialInputChar = ' ';  // FIXME: wird als neuer Eingabewert verstanden und überspringt alles weiter ... Serial.available() > 0
      Serial.println();
      Serial.println("An welchem Tag soll der Alarm eingeschaltet werden? Gib den Wochentag als Zahlenwert an.");
      getSerialInputChar();
      if ( SerialInputChar == '1' ) {
        monday.enableAlarm(); Serial.print("Montag: Alarm AN"); }
      else if ( SerialInputChar == '2' ) {
        tuesday.enableAlarm(); Serial.print("Dienstag: Alarm AN"); }
      else if ( SerialInputChar == '3' ) {
        wednesday.enableAlarm(); Serial.print("Mittwoch: Alarm AN"); }
      else if ( SerialInputChar == '4' ) {
        thursday.enableAlarm(); Serial.print("Donnerstag: Alarm AN"); }
      else if ( SerialInputChar == '5' ) {
        friday.enableAlarm(); Serial.print("Freitag: Alarm AN"); }
      else if ( SerialInputChar == '6' ) {
        saturday.enableAlarm(); Serial.print("Samstag: Alarm AN"); }
      else if ( SerialInputChar == '7' ) {
        sunday.enableAlarm(); Serial.print("Sonntag: Alarm AN"); }

      alarmTimeChanged = true;
      SerialInputChar = ' ';

      Serial.println();
    }


    if ( SerialInputChar == 'd' ) {   
      //sSerialInputChar = ' ';
      Serial.println();
      Serial.println("An welchem Tag soll der Alarm ausgeschaltet werden? Gib den Wochentag als Zahlenwert an.");
      getSerialInputChar();
      if ( SerialInputChar == '1' ) {
        monday.disableAlarm(); Serial.print("Montag: Alarm AUS"); }
      else if ( SerialInputChar == '2' ) {
        tuesday.disableAlarm(); Serial.print("Dienstag: Alarm AUS"); }
      else if ( SerialInputChar == '3' ) {
        wednesday.disableAlarm(); Serial.print("Mittwoch: Alarm AUS"); }
      else if ( SerialInputChar == '4' ) {
        thursday.disableAlarm(); Serial.print("Donnerstag: Alarm AUS"); }
      else if ( SerialInputChar == '5' ) {
        friday.disableAlarm(); Serial.print("Freitag: Alarm AUS"); }
      else if ( SerialInputChar == '6' ) {
        saturday.disableAlarm(); Serial.print("Samstag: Alarm AUS"); }
      else if ( SerialInputChar == '7' ) {
        sunday.disableAlarm(); Serial.print("Sonntag: Alarm AUS"); }

      alarmTimeChanged = true;
      SerialInputChar = ' ';

      Serial.println();
    }

    if ( SerialInputChar == 'r' ) {
      SerialInputChar = ' ';
      Serial.println("Loesche EEPROM.");
      eraseEEPROM();
      loadEEPROMConfigIntoRAM();
    }

    if ( SerialInputChar == 's' ) {
      SerialInputChar = ' ';
      Serial.println("Uebertrage Standardweckzeiten.");
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

      if ( SerialInputChar == 'h' ) {
        SerialInputChar = ' ';
        showHelp();
    }
    newSerialInput = false;
    Serial.println("> ");
  }
}