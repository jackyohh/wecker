volatile bool encoderFired;
volatile bool encoderButtonFired;
volatile bool encoderUp;

unsigned int nodgeValue = 2;

unsigned int encoderCount = 0;
bool encoderCountChange = 0;   // Zeigt an, ob der Drehregler sich soeben geändert hat --> Wichtig für Menü

static unsigned int encoderButtonDebounceTime = 800;


//Sprünge pro kleinste Drehung
#define nodges 2

void fireRotaryEncoder (){
  if (digitalRead (encoderPinA))
    encoderUp = digitalRead (encoderPinB);
  else
    encoderUp = !digitalRead (encoderPinB);
  encoderFired = true;
}

void fireEncoderButton(){
  if (digitalRead (encoderPinPush))
      encoderButtonFired = true;
}

int NodgeCorrection(){
   if (encoderUp) {
        nodgeValue++;
        if ( nodgeValue == 2*nodges){    // Korrektur: Kleinste Drehung soll 1 statt 2 Signale erzeugen
          encoderCount++;
          nodgeValue = nodges;
        }
   }
   else {
        nodgeValue--;
        if ( nodgeValue == 0 ) {   // Korrektur: Kleinste Drehung soll 1 statt 2 Signale erzeugen
          encoderCount--;
          nodgeValue = nodges;
        }
      }
      encoderFired = false;
      return encoderCount;
}
void recognizeRotary() {
  if (encoderFired) {
    int oldcount = encoderCount;   
    encoderCountChange = 1;  
    if ( NodgeCorrection() != oldcount )  // Korrekturberechnung siehe NodgeCorrection()
      Serial.println(encoderCount);
  }
}