alarm monday, tuesday, wednesday, thursday, friday, saturday, sunday;

void setDefaultAlarms(){
  monday.setAlarm(1, 6, 30, 0);   // Alle Weckzeiten sind deaktiviert
  tuesday.setAlarm(2, 6, 30, 0);
  wednesday.setAlarm(3, 6, 30, 0);
  thursday.setAlarm(4, 6, 30, 0);
  friday.setAlarm(5, 6, 30, 0);
  saturday.setAlarm(6, 6, 30, 0);
  sunday.setAlarm(0, 6, 30, 0);     // rtctime.dayOfTheWeek() gibt am Sonntag 0 aus Montag = 1, Dienstag = 2, usw
}