class alarm 
{ 
  // privat Bereich: kann mittels abc.day nicht aufgerufen werden
      int day;
      int hour;
      int minute;
      bool onlyonce;
      bool enable;

  public: 
    alarm();            // der Default-Konstruktor
    // alarm(int a=0);     // weiterer Konstruktor mit Parameter und Defaultwert
    // ~alarm();           // der Destruktor
    // alarm() : day(0), hour(0), minute(0), enable(1), onlyonce(0) { }   // TODO: nach tiefere Bedeutung bzw. Nutzen suchen
    
    void setAlarm(int alarmDay, int alarmHour, int alarmMinute);

    void setAlarm(int alarmDay, int alarmHour, int alarmMinute, bool alarmStatus);

    void enableAlarm();

    void disableAlarm();

    int getDay();

    int getHour();

    int getMinute();

    int getAlarmStatus();
};

