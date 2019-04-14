#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

int lcd_width = 20;
int lcd_hight = 4;

#include <menu2004.h>

// byte upArrow[8] = {
//   0b00100, //   *
//   0b01110, //  ***
//   0b10101, // * * *
//   0b00100, //   *
//   0b00100, //   *
//   0b00100, //   *
//   0b00100, //   *
//   0b00100  //   *
// };

unsigned int LCDcenter_startRow(String text, int lcdwidth){
    if ( text.length() >= lcdwidth) // Stringlänge größer oder gleich Displaybreite
      return 0;
    unsigned int centerText_startpoint = (lcdwidth - text.length())/2; // Beachte, dass Nachkommastelle abgeschnitten werden aus 6,5 wird 6
    return centerText_startpoint;
}