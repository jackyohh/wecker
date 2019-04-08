#include <Arduino.h>
String webRadioURL[] = {"http://scast.regenbogen.de/rr-karlsruhe-64-mp3", "http://mp3.ffh.de/radioffh/hqlivestream.mp3", "http://ice1.somafm.com/u80s-128-mp3" };

int currentPlayingRadioStation = 1;
int currentVolume = 20;
#define amountWebRadios 100
String host[amountWebRadios], path[amountWebRadios];


String tmp_radioStationHost, tmp_radioStationPath;

void splitLinkIntoHostAndPath(String url){
  url.replace("http://", "");      // http:// entfernen
  tmp_radioStationHost = url.substring(0, url.indexOf("/"));    // Host extrahieren
  tmp_radioStationPath = url.substring(url.indexOf("/"));    // Pfad extrahieren

  Serial.println("host + path:");
  Serial.println(tmp_radioStationHost);
  Serial.println(tmp_radioStationPath);
}

void SplitAllWebradioURLIntoHostAndPathName(){
  int radioStations = sizeof(webRadioURL)/sizeof(webRadioURL[0]);  // /sizeof(webRadioURL[0]) ist notwendig, damit die richtige Anzahl an Elementen berechnet werden kann.
  for (int i = 0; i < radioStations; i++){
      splitLinkIntoHostAndPath(webRadioURL[i]);
      host[i] = tmp_radioStationHost;
      path[i] = tmp_radioStationPath;
      Serial.println(host[i] + " " + path[i]);
  }
}


void saveAllWebRadioURLintoEEPROM(){  //TODO: Kommentare entfernen
  int radioStations = sizeof(webRadioURL)/sizeof(webRadioURL[0]);
  for (int i = 0; i < amountWebRadios && i < radioStations; i++ ){
    // cfg.hostRadioStation[i] = host[i];
    // cfg.pathRadioStation[i] = path[i];
  }
  // cfg.valid = 1;  
  // saveToEEPROM();
}


void loadWebRadioSettingsFromEEPROM(){
  //TODO: Check ob cfg vorhanden, bzw erneut laden
  currentPlayingWebRadio = cfg.lastPlayedRadioStation;
  currentVolume = cfg.lastVolume;
}


void initializeStream(String host, String path){  // String host und path ok? ursprünglich char *host
  Serial.print("connecting to ");  Serial.println(host);  //TODO: Kommentare entfernen

  // if (!client.connect(host, httpPort)) {
  //   Serial.println("Connection failed");
  //   return;
  // }
    // We now create a URI for the request
    Serial.print("Requesting URL: ");
    Serial.println(path);
    // This will send the request to the server
    // client.print(String("GET ") + path + " HTTP/1.1\r\n" +
    //             "Host: " + host + "\r\n" + 
    //             "Connection: close\r\n\r\n");
}

void initializeLastStream(){
  initializeStream(host[currentPlayingRadioStation], path[currentPlayingRadioStation]);
}

void nextRadioStation(){
  if (currentPlayingRadioStation < amountWebRadios){
    currentPlayingRadioStation++;
    initializeStream(host[currentPlayingRadioStation-1], path[currentPlayingRadioStation-1]);
  }
}

void previousRadioStation(){
  if (currentPlayingRadioStation > 1 ){
    currentPlayingRadioStation--;
    initializeStream(host[currentPlayingRadioStation-1], path[currentPlayingRadioStation-1]);
  }
}

int getCurrentRadioStation(){
  return currentPlayingRadioStation;
}