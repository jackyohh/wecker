#include <TimeLib.h>
#include <RTClib.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <summertime_eu.h>
#include <digitalClock.h>

RTC_DS3231 rtc;
DateTime rtctime;
String rtcHour, rtcMinute, rtcSecond, rtcDay, rtcMonth, rtcYear, rtcWholeTime, rtcWholeDate, rtcUnixtime;
const char daysOfTheWeek[7][12] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};

String formattedDate; // Variables to save date and time

int splitT; // Position des Buchstabens T im String
int ntpYear, ntpMonth, ntpDay, ntpHour, ntpMinute, ntpSecond;
bool oneSecondLater = false;

WiFiUDP ntpUDP;

// By default 'pool.ntp.org' is used with 60 seconds update interval and
// no offset
// NTPClient timeClient(ntpUDP);
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", timeZone * 3600);

// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
// NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

void syncNTPtoRTC(){
  //rtctime = rtc.now();   // muss enthalten sein. siehe Kommentar bei getRTCtime()
  //String temp3 =  String(rtctime.dayOfTheWeek(), DEC);
  // Serial.println(rtcHour);
  // Serial.println("_______________");
  //Serial.println(String(rtctime.minute()));
  if (WiFi.status() == WL_CONNECTED){
    Serial.println("Hole Zeit vom NTP Server ...");                        
    if(!timeClient.update()){
      Serial.println("FEHLER");
      return;
    }
    Serial.println("Synchronisiere RTC-Modul ...");
    rtcUnixtime = String(rtctime.unixtime());
    uint32_t before = rtctime.unixtime();
    rtc.adjust( DateTime(timeClient.getEpochTime()) );
    int secondsOffset = rtctime.unixtime() - before;
    Serial.println("Versatz: "+ String(secondsOffset) + "s");
  }
  else Serial.println("Keine Wifi Verbindung! RT Uhr konnte nicht gestellt werden.");
}

void checkForRtcPower(){  
  if (rtc.lostPower()) {  //TODO: oneSecondLater
    static int connectionTries = 0;
    while ( connectionTries <= 9 && rtc.lostPower()){  // 10 Versuche und solang rtc.lostPower Bit noch gesetzt ist
      Serial.println("Stromversorgung der RT Uhr wurde getrennt.");
      connectionTries++;
      syncNTPtoRTC();
    }
  }
}

void checkAndFixNTPtime(){
    if(timeClient.getEpochTime() < 4000 ){      //TODO: exaktere Überprüfung!!
      Serial.println("Uhrzeit fehlerhaft");
      syncNTPtoRTC();
    }
}

void extractNTPtime(){
    checkAndFixNTPtime();
    formattedDate = timeClient.getFormattedDate(); // The formattedDate comes with the following format: 2018-05-28T16:00:13Z
    splitT = formattedDate.indexOf("T"); // Position des Buchstabens T im String
    ntpYear = formattedDate.substring(0, 4).toInt();  //.substring() parst Substring // .toInt() wandelt String in Integer um
    ntpMonth = formattedDate.substring(5, 7).toInt();
    ntpDay = formattedDate.substring(8, 10).toInt();
    ntpHour = formattedDate.substring(splitT+1, splitT+3).toInt();
    ntpMinute = formattedDate.substring(splitT+4, splitT+6).toInt();
    ntpSecond = formattedDate.substring(splitT+7, splitT+9).toInt();
}

bool isSummertime(){
  // extractNTPtime();
  return summertime_EU(ntpYear, ntpMonth, ntpDay, ntpHour, 1);
}

void checkAndFixTimechange(){    
  if (oneSecondLater && summertime != isSummertime()){ // 1s später + falls Zeitumstellung
      Serial.println("Zeitumstellung");
      summertime = isSummertime();
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

void dailySyncNTPtoRTC(){
  if (rtctime.hour() == ntpUpdateTime[0] && rtctime.minute() == ntpUpdateTime[1] && rtctime.second() == 0 && oneSecondLater)
    syncNTPtoRTC();
}