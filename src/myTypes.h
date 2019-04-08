#ifndef myTypes_h
#define myTypes_h

#include <WString.h>

// Types 'byte' und 'word' doesn't work!
typedef struct {
  int valid;                        // 0=no configuration, 1=valid configuration
  char ssid[31];                    
  char pass[31];         

  int alarmMon[4];               //alarmWed[4] = { Tag , Stunde, Minute, AN/AUS };
  int alarmTue[4];
  int alarmWed[4];              
  int alarmThu[4];
  int alarmFri[4];
  int alarmSat[4];
  int alarmSun[4];

  //WebRadio
  int lastPlayedRadioStation;
  int lastVolume;
  String hostRadioStation[100]; //TODO: Überprüfe, pb Stringwerte gespeichert werden
  String pathRadioStation[100]; //TODO: #define amountWebStations 100 möglich??
  
  // int  useMQTT;                     // 0=Don't use, 1=use MQTT
  // char MQTTHost[31];                // Ip address or hostname of MQTT broker
  // char MQTTPort[11];                // Port of MQTT broker
  // char MQTTpubCH0[61];              // MQTT publish CH0 topic
  // char MQTTpubCH1[61];              // MQTT publish CH1 topic
  // char MQTTsubCH0[61];              // MQTT subscribe CH0 topic
  // char MQTTsubCH1[61];              // MQTT subscripe CH1 topic
} configData_t;

#endif
