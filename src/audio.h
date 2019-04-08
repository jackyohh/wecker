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
#include <ESP8266WiFi.h>
  
// char* ssid     = "WlanSid";
// const char* password = "r0ll1ng 570n35";
     

// These are the pins used
#if defined(ARDUINO_ESP8266_NODEMCU)    // oder defined(SP8266)  
  #define VS1053_XRESET  D4        // mit 10kΩ an 3,3V
  #define VS1053_CS      D0        // XCS     VS1053 chip select pin (output)
  #define VS1053_DREQ    D2        //         VS1053 Data request, ideally an Interrupt pin
  #define VS1053_DCS     D1        // XDCS auf VS1053
  #define VS1053_CARDCS  D3        // CS auf VS1053

#else
  #error "Unknow architecture"  
#endif

#define VOLUME_KNOB    A0
#define ON_OFF_SWITCH  4

Adafruit_VS1053 musicPlayer =  Adafruit_VS1053(VS1053_XRESET, VS1053_CS, VS1053_DCS, VS1053_DREQ);  

uint8_t mp3buff[32];   //buffer of mp3 data - vs1053 likes 32 bytes at a time

WiFiClient client;

// http://scast.regenbogen.de/rr-karlsruhe-64-mp3
    const char *host = "scast.regenbogen.de";
    const char *path = "/rr-karlsruhe-64-mp3";

// http://mp3.ffh.de/radioffh/hqlivestream.mp3
  const char *host2 = "mp3.ffh.de";
  const char *path2 = "/radioffh/hqlivestream.mp3";

//  http://ice1.somafm.com/u80s-128-mp3 //funktioniert
  const char *host3 = "ice1.somafm.com";
  const char *path3 = "/u80s-128-mp3";
  const char *path3_2 = "/doomed-128-mp3";

//http://st01.dlf.de/dlf/01/64/mp3/stream.mp3   //Deutschland Radio 64 bit - funktioniert nicht --- vl port falsch
  // const char *host3 = "st01.dlf.de";
  // const char *path3 = "/dlf/01/64/mp3/stream.mp3";

int httpPort = 80;
     
int lastvol = 20;  // je kleiner der Wert, desto lauter!!  




// void setup() {
//   Serial.begin(115200);
//   SPI.begin();

//   Serial.println("\n\nAdafruit VS1053 Feather WiFi Radio");
  
//   /************************* INITIALIZE MP3 WING */
//   if (! musicPlayer.begin()) { // initialise the music player
//       Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
//       while (1) delay(10);
//   }
  
//   Serial.println(F("VS1053 found"));
//   musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working
  
//   // Set volume for left, right channels. lower numbers == louder volume!
//   musicPlayer.setVolume(lastvol, lastvol);
  
//   // don't use an IRQ, we'll hand-feed
  
//   pinMode(ON_OFF_SWITCH, INPUT_PULLUP);

//   Serial.print("Connecting to SSID "); Serial.println(ssid);
//   WiFi.begin(ssid, password);
  
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
     
// Serial.println("WiFi connected");  
// Serial.println("IP address: ");  Serial.println(WiFi.localIP());


// /************************* INITIALIZE STREAM */
// Serial.print("connecting to ");  Serial.println(host);

// if (!client.connect(host, httpPort)) {
//   Serial.println("Connection failed");
//   return;
// }
      
//   // We now create a URI for the request
//   Serial.print("Requesting URL: ");
//   Serial.println(path);
  
//   // This will send the request to the server
//   client.print(String("GET ") + path + " HTTP/1.1\r\n" +
//                "Host: " + host + "\r\n" + 
//                "Connection: close\r\n\r\n");

// }
     

// void loop() {
  
//   if (! digitalRead(ON_OFF_SWITCH)) {
//     yield();
//     return;
//   }
  

//   // wait till mp3 wants more data
//   if (musicPlayer.readyForData()) {
//     // Serial.print("ready ");
    
//     //wants more data! check we have something available from the stream
//     if (client.available() > 0) {
//       // Serial.print("set ");
//       // read up to 32 bytes
//       //uint8_t bytesread = client.read(mp3buff, 32);
//       // push to mp3
//       musicPlayer.playData(mp3buff, client.read(mp3buff, 32));
  
//       // Serial.println("stream!");
//     }
//   }
//   musicPlayer.dumpRegs();
//   musicPlayer.setVolume(50, 50);
//   musicPlayer.dumpRegs();

//   musicPlayer.sineTest(0x44, 500);  


//   // else {
//   //   if (loopcounter >= 1000) {
//   //     loopcounter = 0;
//   //     // adjust volume!
//   //     int vol = 0;
//   //     vol = analogRead(VOLUME_KNOB);
//   //     vol /= 10;
//   //     if (abs(vol - lastvol) > 3) {
//   //       Serial.println(vol);
//   //       lastvol = vol;
//   //       musicPlayer.setVolume(lastvol, lastvol);
//   //     }
//   //   }
//   // }
// }