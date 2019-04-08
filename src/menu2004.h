bool inMenu = 0;
bool mainmenuActive = 0;
bool submenu1Active = 0;
bool submenu2Active = 0;
bool submenu3Active = 0;
bool submenu4Active = 0;
//bool submenu1submenuActive = 0;


void updateMainMenu(){    
    switch(encoderCount){
        case 0:
            encoderCount = 5;
            updateMainMenu();
            break;
        case 1:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("> Wecker stellen");
            lcd.setCursor(0, 1);
            lcd.print("  Radio");
            lcd.setCursor(0, 2);
            lcd.print("  Uhrzeit einstellen");
            lcd.setCursor(0, 3);
            lcd.print("  WLAN einstellen");
            break;
        case 2:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("  Wecker stellen");
            lcd.setCursor(0, 1);
            lcd.print("> Radio");
            lcd.setCursor(0, 2);
            lcd.print("  Uhrzeit einstellen");
            lcd.setCursor(0, 3);
            lcd.print("  WLAN einstellen");
            break;
        case 3:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("  Wecker stellen");
            lcd.setCursor(0, 1);
            lcd.print("  Radio");
            lcd.setCursor(0, 2);
            lcd.print("> Uhrzeit einstellen");
            lcd.setCursor(0, 3);
            lcd.print("  WLAN einstellen");
            break;
        case 4:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("  Wecker stellen");
            lcd.setCursor(0, 1);
            lcd.print("  Radio");
            lcd.setCursor(0, 2);
            lcd.print("  Uhrzeit einstellen");
            lcd.setCursor(0, 3);
            lcd.print("> WLAN einstellen");
            break;
        case 5:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("> Beenden");
            break;
        case 6:
            encoderCount = 1;
            updateMainMenu();
        break;
    }
}

void updateSubMenu3(){    
    switch(encoderCount){
        case 0:
            encoderCount = 3;
            updateSubMenu3();
            break;
        case 1:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("> mit NTP synchron.");
            lcd.setCursor(0, 1);
            lcd.print("  Manuell einstellen");
            lcd.setCursor(0, 2);
            lcd.print("  zur\xF5\ck");
            break;
        case 2:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("  mit NTP synchron.");
            lcd.setCursor(0, 1);
            lcd.print("> Manuell einstellen");
            lcd.setCursor(0, 2);
            lcd.print("  zur\xF5\ck");
            break;
        case 3:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("  mit NTP synchron.");
            lcd.setCursor(0, 1);
            lcd.print("  Manuell einstellen");
            lcd.setCursor(0, 2);
            lcd.print("> zur\xF5\ck");
            break;
        case 4:
            encoderCount = 1;
            updateSubMenu3();
        break;
    }
}


void executeActionSubMenu3(){
    switch(encoderCount){
        case 1: //Mit NTP synchronisieren
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("AAAAkkkschtn");
            break;
        case 2: // Manuell einstellen
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("AAAAkkkschtn");
            break;
        case 3: //zurück
            submenu3Active = 0;
            mainmenuActive = 1;
            encoderCount = 1;
            updateMainMenu();
            break;
    }
}

void executeMainMenu(){
    switch(encoderCount){
        case 1:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("AAAAkkkschtn");
            break;
        case 2:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("AAAAkkkschtn");
            break;
        case 3:
            encoderCount = 1;
            mainmenuActive = 0;
            submenu3Active = 1;
            updateSubMenu3();
            break;
        case 4:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("AAAAkkkschtn");
            break;
        case 5:
            lcd.clear();
            inMenu = 0;
            mainmenuActive = 0;
            break;
    }
}

void drawMenu(){
    if (mainmenuActive)
        updateMainMenu();
    else if (submenu3Active)
        updateSubMenu3();
        
}

void executeAction(){
        if (mainmenuActive) {
            executeMainMenu();
        }
        else if (submenu3Active)          // fällt scheinbar aus der Schleife
            executeActionSubMenu3();
    
}