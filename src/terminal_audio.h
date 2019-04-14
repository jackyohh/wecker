bool inTerminalAudio = 0;

void terminalAudioHelp(){
    Serial.println();
    Serial.println("Hilfe:");
    Serial.println();
    Serial.println("h   Zeigt diese Hilfe an");
    Serial.println("l   Zeigt Senderliste an");
    Serial.println("n   nächster Sender");
    Serial.println("v   vorheriger Sender");
    Serial.println("s   Stream stoppen");
    Serial.println("a   Stream starten");
    Serial.println("+   lauter");
    Serial.println("-   leiser");
    Serial.println("--------------------");
    Serial.println("t   Testton ausgeben");
    Serial.println("i   zeige Statusinformationen");
    Serial.println("--------------------");
    Serial.println("q   Zurück/Beenden");
    Serial.println();
}

void terminalAudio(){ // TODO: Testen!
  if (newSerialInputChar())  {
    if ( SerialInputChar == 'h' ) 
        terminalAudioHelp();
    
    // if ( SerialInputChar == 'l' ) 
    //   getAllRadioStations();
    
    // if ( SerialInputChar == 'n' ) {   
    //   Serial.println();
    //   nextRadioStation();
    // }

    // if ( SerialInputChar == 'v' ) {
    //   Serial.println();
    //   previousRadioStation();
    // }

    // if ( SerialInputChar == 's' ) {
    //   Serial.println("Stop");
    //   stopStream();
    // }


    // if ( SerialInputChar == 'a' ) {
    //   Serial.println("Play");
    //   initializeLastStream();
    // }

    // if ( SerialInputChar == '+' ) {
    //   if(currentVolume > 9){
    //     currentVolume-=10;
    //     musicPlayer.setVolume(currentVolume,currentVolume);
    //   }
    //   Serial.println("Lautstärke " + String(currentVolume) );
    // }

    // if ( SerialInputChar == '-' ) {
    //   if(currentVolume < 246){
    //     currentVolume+=10;
    //     musicPlayer.setVolume(currentVolume,currentVolume);
    //   }
    //   Serial.println("Lautstärke " + String(currentVolume) );
    // }

    // if ( SerialInputChar == 't' ) {
    //   testSound();
    //   Serial.println("Testton");
    // }
    // if ( SerialInputChar == 'i' ) {
    //   Serial.println("Zeige Dump");
    //   musicPlayer.dumpRegs();
    // }

    if ( SerialInputChar == 'q' ) {
      inTerminalAudio = false;
      showHelp();
    }
    Serial.print("> ");
  }
}