// utility for digital clock display: prints preceding colon and leading 0
String printDigits(int digits){
  String zero;
  if(digits < 10){
      String zero = String("0" + String(digits, DEC));
      return zero;
  }
  return String(digits, DEC); 
}

