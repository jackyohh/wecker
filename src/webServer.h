#include <WiFiClient.h>   //TODO: nötig??
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

String INDEX_HTML ="<!DOCTYPE html><html><head>"
"<style>"
"main { font-family: sans-serif;  }"
"#tage { width: 90px; border: 1px dashed green;}"
"#zeit { width: 90px;  border: 1px dashed green;}"
"#alarmstatus { width: 30px; border: 1px dashed green;}"
"#save{height: 200px; border: 1px dashed green;}"
"#tage > p { text-align: right; margin-top: 12px; margin-bottom: 17px;}"
"#zeit > p { text-align: center; margin-top: 8px; margin-bottom: 12px;}"
"#alarmstatus > p { margin-top: 10px; margin-bottom: 11px;}"
"#save > button { margin-top: 230px;}"
"#übersicht div{ vertical-align: top; display: inline-block;}"
"</style>"
"<title>WeckBot</title></head>"
    "<body><main>"
            "<h1>WeckBot</h1>"
            "<section id=\"übersicht\">"
            "<h2>Weckzeiten einstellen</h2>" 
            "<div>"
                "<form action=\"/\" method=\"get\" autocomplete=\"off\">"
              "<div id=\"tage\">"
                    "<p>Montag:</p><p>Dienstag:</p><p>Mittwoch:</p><p>Donnerstag:</p><p>Freitag:</p><p>Samstag:</p><p>Sonntag:</p>"
                "</div>" 
                "<div id=\"zeit\">"
                    "<p><input class=\"alarmTime\" type=\"time\" name=\"mondayAlarmTime\" value=\"<!--MoH-->:<!--MoM-->\"></p>"
                    "<p><input class=\"alarmTime\" type=\"time\" name=\"tuesdayAlarmTime\" value=\"<!--TuH-->:<!--TuM-->\"></p>"
                    "<p><input class=\"alarmTime\" type=\"time\" name=\"wednesdayAlarmTime\" value=\"<!--WdH-->:<!--WdM-->\"></p>"
                    "<p><input class=\"alarmTime\" type=\"time\" name=\"thursdayAlarmTime\" value=\"<!--ThH-->:<!--ThM-->\"></p>"
                    "<p><input class=\"alarmTime\" type=\"time\" name=\"fridayAlarmTime\" value=\"<!--FrH-->:<!--FrM-->\"></p>"
                    "<p><input class=\"alarmTime\" type=\"time\" name=\"saturdayAlarmTime\" value=\"<!--SaH-->:<!--SaM-->\"></p>"
                    "<p><input class=\"alarmTime\" type=\"time\" name=\"sundayAlarmTime\" value=\"<!--SuH-->:<!--SuM-->\"></p>"
                "</div>"
                "<div id=\"alarmstatus\">"
                    "<p><input class=\"alarmStatus\" type=\"checkbox\" name=\"mondayAlarmStatus\" @@MoSta@@></p>"
                    "<p><input class=\"alarmStatus\" type=\"checkbox\" name=\"tuesdayAlarmStatus\" @@TuSta@@></p>"
                    "<p><input class=\"alarmStatus\"sd type=\"checkbox\" name=\"wednesdayAlarmStatus\" @@WdSta@@></p>"
                    "<p><input class=\"alarmStatus\" type=\"checkbox\" name=\"thursdayAlarmStatus\" @@ThSta@@></p>"
                    "<p><input class=\"alarmStatus\" type=\"checkbox\" name=\"fridayyAlarmStatus\" @@FrSta@@></p>"
                    "<p><input class=\"alarmStatus\" type=\"checkbox\" name=\"saturdayAlarmStatus\" @@SaSta@@></p>"
                    "<p><input class=\"alarmStatus\" type=\"checkbox\" name=\"sundayAlarmStatus\" @@SuSta@@></p>"        
                "</div>"
                "<div id=\"save\">"
                    "<button>Speichern</button>"
                "</form>"
                "</div>"
            "</div>"
            "</section></main>"
"</body></html>";

String weeknames[] = {"mondayAlarmTime","tuesdayAlarmTime","wednesdayAlarmTime","thursdayAlarmTime","fridayAlarmTime","saturdayAlarmTime","sundayAlarmTime"};
String htmlTimeH[] = {"<!--MoH-->","<!--TuH-->","<!--WdH-->","<!--ThH-->","<!--FrH-->","<!--SaH-->","<!--SuH-->"};
String htmlTimeM[] = {"<!--MoM-->","<!--TuM-->","<!--WdM-->","<!--ThM-->","<!--FrM-->","<!--SaM-->","<!--SuM-->"};

String dayAlarmStatus[] = {"mondayAlarmStatus","tuesdayAlarmStatus","wednesdayAlarmStatus","thursdayAlarmStatus","fridayyAlarmStatus","saturdayAlarmStatus","sundayAlarmStatus"};
String htmlAlarmStatus[] = {"@@MoSta@@","@@TuSta@@","@@WdSta@@","@@ThSta@@","@@FrSta@@","@@SaSta@@","@@SuSta@@"};


void handleRoot()
{
  bool siteChanged = false;
  String tmpPage = INDEX_HTML;
  for ( int i = 0; i < 7; i++){   // Alle Weckzeiten abfragen
    if (server.hasArg(weeknames[i])){
      String clk = server.arg(weeknames[i]);   
      String h = clk.substring(0,2);    
      String m = clk.substring(3,5);
//      Serial.println(h);
//      Serial.println(m);
      tmpPage.replace(htmlTimeH[i], h);
      tmpPage.replace(htmlTimeM[i], m);
      siteChanged = true;
    }
  }
    
  for ( int i = 0; i < 7; i++){   // Status (an/aus) aller Weckzeiten abfragen
    if (server.hasArg(dayAlarmStatus[i]) && server.arg(dayAlarmStatus[i]) == "on"){
//      String sta = server.arg(dayAlarmStatus[i]);
//      Serial.println(sta);
      tmpPage.replace(htmlAlarmStatus[i], "checked");
      siteChanged = true;
    }
  }

  if (siteChanged) server.send(200, "text/html", tmpPage);
  else {
    server.send(200, "text/html", INDEX_HTML);
  }
}


void returnOK()
{
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "OK\r\n");
}


void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}