// ;build_flags = -DPIO_FRAMEWORK_ARDUINO_LWIP2_LOW_MEMORY
// ;build_flags = -DPIO_FRAMEWORK_ARDUINO_LWIP_HIGHER_BANDWIDTH
// ; -D PIO_FRAMEWORK_ARDUINO_LWIP2_LOW_MEMORY v2 Lower Memory (default)
// ; -D PIO_FRAMEWORK_ARDUINO_LWIP2_HIGHER_BANDWIDTH v2 Higher Bandwidth
// ; -D PIO_FRAMEWORK_ARDUINO_LWIP2_LOW_MEMORY_LOW_FLASH v2 Lower Memory (no features)
// ; -D PIO_FRAMEWORK_ARDUINO_LWIP2_HIGHER_BANDWIDTH_LOW_FLASH v2 Higher Bandwidth (no features)
// ; -D PIO_FRAMEWORK_ARDUINO_LWIP2_IPV6_LOW_MEMORY v2 IPv6 Lower Memory
// ; -D PIO_FRAMEWORK_ARDUINO_LWIP2_IPV6_HIGHER_BANDWIDTH v2 IPv6 Higher Bandwidth
// ; -D PIO_FRAMEWORK_ARDUINO_LWIP_HIGHER_BANDWIDTH v1.4 Higher Bandwidth


#if defined(ESP8266) 
  #define VS1053_CONTROL_SPI_SETTING SPISettings(1000000, MSBFIRST, SPI_MODE0) 
  #define VS1053_DATA_SPI_SETTING SPISettings(20000000, MSBFIRST, SPI_MODE0) 
#else 
  #define VS1053_CONTROL_SPI_SETTING SPISettings(250000, MSBFIRST, SPI_MODE0) 
  #define VS1053_DATA_SPI_SETTING SPISettings(8000000, MSBFIRST, SPI_MODE0) 
#endif

#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <ESP8266WiFi.h>  //TODO: nötig??

#if defined(ARDUINO_ESP8266_NODEMCU)    // oder defined(SP8266)  
    #define VS1053_XRESET  D4        // mit 10kΩ an 3,3V
    #define VS1053_CS      D0        // XCS     VS1053 chip select pin (output)
    #define VS1053_DREQ    D2        //         VS1053 Data request, ideally an Interrupt pin
    #define VS1053_DCS     D1        // XDCS auf VS1053
    #define VS1053_CARDCS  D3        // CS auf VS1053
#else
    #error "Unknow architecture"  
#endif

Adafruit_VS1053 musicPlayer =  Adafruit_VS1053(VS1053_XRESET, VS1053_CS, VS1053_DCS, VS1053_DREQ);

WiFiClient client;
int httpPort = 80;

uint8_t mp3buff[32];   // vs1053 likes 32 bytes at a time

bool play = true;
int currentPlayingRadioStation = 1;
int currentVolume = 20;
#define amountWebRadios 100
String host[amountWebRadios], path[amountWebRadios];
String tmp_radioStationHost, tmp_radioStationPath;

String webRadioURL[] = {"http://scast.regenbogen.de/rr-karlsruhe-64-mp3", "http://mp3.ffh.de/radioffh/hqlivestream.mp3", "http://ice1.somafm.com/u80s-128-mp3" };
// TODO: webRadioURLs via Homepage, EEPROM abrufen

// ########### Parse Web Radio links

void initializeVS1053(){
  SPI.begin();
  Serial.println("\n\nAdafruit VS1053 Feather WiFi Radio");
     
  /************************* INITIALIZE MP3 WING */
  if (! musicPlayer.begin()) { // initialise the music player
      Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
      while (1) delay(10);
  }
  
  Serial.println(F("VS1053 found"));
  musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working
  
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(currentVolume, currentVolume);
}

void splitLinkIntoHostAndPath(String url){
  url.replace("http://", "");      // http:// entfernen
  tmp_radioStationHost = url.substring(0, url.indexOf("/"));    // Host extrahieren
  tmp_radioStationPath = url.substring(url.indexOf("/"));    // Pfad extrahieren

  // Serial.println("host + path:");
  // Serial.println(tmp_radioStationHost);
  // Serial.println(tmp_radioStationPath);
}

void SplitAllWebradioURLIntoHostAndPathName(){
  int radioStations = sizeof(webRadioURL)/sizeof(webRadioURL[0]);  // /sizeof(webRadioURL[0]) ist notwendig, damit die richtige Anzahl an Elementen berechnet werden kann.
  for (int i = 0; i < radioStations; i++){
      splitLinkIntoHostAndPath(webRadioURL[i]);
      host[i] = tmp_radioStationHost;
      path[i] = tmp_radioStationPath;
      // Serial.println(host[i] + " " + path[i]);
  }
}

bool initializeStream(String host, String path){
  play = true;
  Serial.print("Verbinde zu ");  Serial.println(host + path);

  if (!client.connect(host, httpPort)) {
    Serial.println("Verbindung fehlgeschlagen");
    return false;
  }
    // We now create a URI for the request
    // This will send the request to the server
    client.print(String("GET ") + path + " HTTP/1.1\r\n" +
                "Host: " + host + "\r\n" + 
                "Connection: close\r\n\r\n");
  return true;
}

void initializeLastStream(){
  play = true;
  if(client.available())  // TODO: geht??
    client.stop();
  initializeStream(host[currentPlayingRadioStation], path[currentPlayingRadioStation]);
}

void stopStream(){
  play = false;
  client.stop();
}

void nextRadioStation(){
  if (currentPlayingRadioStation < sizeof(webRadioURL)/sizeof(webRadioURL[0])){
    currentPlayingRadioStation++;
    Serial.println("Sendernummer: " + String(currentPlayingRadioStation));
    if(!initializeStream(host[currentPlayingRadioStation-1], path[currentPlayingRadioStation-1])) // falls Verbindung fehlschlägt, wieder auf alte Sendernummer springen
      currentPlayingRadioStation--;
  }
}

void previousRadioStation(){
  if (currentPlayingRadioStation > 1 ){
    currentPlayingRadioStation--;
    Serial.println("Sendernummer: " + String(currentPlayingRadioStation));
    if(!initializeStream(host[currentPlayingRadioStation-1], path[currentPlayingRadioStation-1])) // falls Verbindung fehlschlägt, wieder auf alte Sendernummer springen
      currentPlayingRadioStation++;
  }
}

int getCurrentRadioStationNumber(){
  return currentPlayingRadioStation;
}

int getAllRadioStations(){
  for (int i = 0; i < sizeof(webRadioURL)/sizeof(webRadioURL[0]); i++){
    Serial.print((i+1));
    Serial.print("  ");
    Serial.println(webRadioURL[i]);
  }
}

void testSound(){
  musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working
}

// #### EEPROM ####

// void saveAllWebRadioURLintoEEPROM(){  //TODO: Kommentare entfernen
//   int radioStations = sizeof(webRadioURL)/sizeof(webRadioURL[0]);
//   for (int i = 0; i < amountWebRadios && i < radioStations; i++ ){
//     cfg.hostRadioStation[i] = host[i];
//     cfg.pathRadioStation[i] = path[i];
//   }
//   cfg.valid = 1;  
//   saveToEEPROM();
// }


// void loadWebRadioSettingsFromEEPROM(){
//   //TODO: Check ob cfg vorhanden, bzw erneut laden
//   currentPlayingRadioStation = cfg.lastPlayedRadioStation;
//   currentVolume = cfg.lastVolume;
// }