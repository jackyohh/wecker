char SerialInputChar;
bool newSerialInput = false;
bool newSerialInputArray = false;
bool endOfSerialInput = false;


int char2int(char a){
  if ( isDigit(a))
    return (a - 48);
  else return false;
}

void ShowNewSerialInput(){
    if (newSerialInput){
    //Serial.print("Eingabe: ");
    Serial.println(SerialInputChar);
    }
}

void getSerialInputChar(){
    if (Serial.available() > 0 ) { 
    SerialInputChar = Serial.read();
    newSerialInput = true;
    }
    ShowNewSerialInput();
}

void ShowNewSerialInputArray(char serialArray[], int arraySize){
    if (newSerialInput){
        Serial.print("> ");
        for (int n = 0; n < arraySize; n++){
            Serial.print(serialArray[n]);
        Serial.println();
        }
    }
}

// void getSerialInputArray(){
//     char SerialInputCharArray[7] = { ' ', ' ', ' ', ' ', ' ', ' ', ' '} ;
//     int arraySize = sizeof(SerialInputCharArray);
//     if (Serial.available() > 0 ) { 
//         Serial.readBytes(SerialInputCharArray, arraySize);
//         newSerialInputArray = true;
//         ShowNewSerialInputArray(SerialInputCharArray, arraySize);
//     }
// }a

struct array{ char wert[7]; };

struct array getSerialInputArray(){
    char SerialInputCharArray[7] = { ' ', ' ', ' ', ' ', ' ', ' ', ' '} ;
    int arraySize = sizeof(SerialInputCharArray);
    // Serial.println("getSerialInputArray");

    if (Serial.available() > 0) {   
        Serial.readBytesUntil(59, SerialInputCharArray, arraySize); // ";"  //FIXME: wartet bis das ganze Array voll geschrieben ist
        newSerialInputArray = true;

        /* Array als Rückgabewert in einer Struktur verschachtelt */
        struct array new_array;
        for (int i = 0; i < 7; i++)
            new_array.wert[i] = SerialInputCharArray[i];
        ShowNewSerialInputArray(SerialInputCharArray, arraySize);
        endOfSerialInput = true;
        return new_array;
    }
}