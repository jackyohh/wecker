#include "alarm.h" // Headerdatei
#include <Arduino.h> //anderenfalls wird "String" und "Serial" nicht erkannt

alarm::alarm()
{

}

void alarm::setAlarm(int alarmDay, int alarmHour, int alarmMinute) 
{ 
    day = alarmDay; 
    hour = alarmHour;
    minute = alarmMinute;
    enable = true;
} 

void alarm::setAlarm(int alarmDay, int alarmHour, int alarmMinute, bool alarmStatus) 
{ 
    day = alarmDay; 
    hour = alarmHour;
    minute = alarmMinute;
    enable = alarmStatus;
} 

void alarm::enableAlarm()
{
    enable = true;
}

void alarm::disableAlarm()
{
    enable = false;
}

int alarm::getHour() // multiple Returnwerte statt Serial.print
{ 
        // if ( enable == true){
            return hour;
        // }
        // else return 99;
} 

int alarm::getMinute() // multiple Returnwerte statt Serial.print
{ 
        // if ( enable == true){
            return minute;
        // }
        // else return 99;
} 

int alarm::getDay() // multiple Returnwerte statt Serial.print
{ 
        // if ( enable == true){
            return day;
        // }
        // else return 99;
} 


int alarm::getAlarmStatus() // multiple Returnwerte statt Serial.print
{ 
    return enable;
} 

