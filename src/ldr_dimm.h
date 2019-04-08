#include <Arduino.h>

int minimalBrightness = 50;
int lastActionTime = millis();

int readLDR() {
  return analogRead(LDRpin);
}

void LCDdimmBacklight(int light){
  int dimm = light/4 + minimalBrightness;
    if (dimm < 256) 
    {
      Serial.print(dimm);
      analogWrite(LCDdimmPin, dimm);
      }
    else 
    {
      analogWrite(LCDdimmPin, 255);
      analogWrite(LCDdimmPin, dimm);
    } 
}

void LCDmaxBacklight(){   //ausführen, falls ein Knopf gedrückt wird
  lastActionTime = millis();
  analogWrite(LCDdimmPin, 255);
}

void LCDdimmOnIdle(){ // ständig in main() ausführen, um die Helligkeit des Displays nach 30s Inaktivität herunter zu regeln
  if (millis() - lastActionTime > 30000) { //nur alle 30s erneuern
    LCDdimmBacklight(readLDR());
  }
}