/***********************************************************************
 * 
 * ialarm.c
 * 
 * Alarm Routines for Izzy
 *
 ***********************************************************************/
#include "ialarm.h"

void Alarm(n, id)
   int n;
   XtIntervalId id;
{
   PlayAlarm(n);
   /*
    * schedule alarm again (after two seconds to avoid races)
    */
   if(theAlarm->type & REPEATING_ALARM)
      XtAddTimeOut(2000, (XtTimerCallbackProc) ScheduleAlarm, (XtPointer) n);
}

void PlayAlarm(n)
   int n;
{
   char commandStr[255];

   strcpy(commandStr,theAlarm[n].command);
   system(commandStr);

}

void ScheduleAlarm(n, id)
   int n;
   XtIntervalId id;
{
   long         tloc,
		tmp,
		alarm_time;
   char         tmpStr[4];
   int          re_schedule; 

   time(&tloc);
   re_schedule = FALSE;
   switch(theAlarm[n].type) {
      case ONLY_TODAY:
         /* not implemented */
         break;
      case ONLY_MMDDYY:
	 /* not implemented */
         break;
      case EVERY_HOUR:
	 tmpStr[0] = theAlarm[n].time[3];
         tmpStr[1] = theAlarm[n].time[4];
	 tmpStr[2] = '\0';
	 tmp = atoi(tmpStr);
         alarm_time = ((tmp*60 + (3600 - (tloc % 3600))) % 3600) * 1000;
         break;
      case EVERY_DAY:
	 alarm_time = AlarmToday(n, tloc, &re_schedule);
         break;
      case EVERY_MONDAY:
	 ScheduleDay("Mon",n);
         break;
      case EVERY_TUESDAY:
	 ScheduleDay("Tue",n);
         break;
      case EVERY_WEDNESDAY:
	 ScheduleDay("Wed",n);
         break;
      case EVERY_THURSDAY:
	 ScheduleDay("Thu",n);
         break;
      case EVERY_FRIDAY:
	 ScheduleDay("Fri",n);
         break;
      case EVERY_SATURDAY:
	 ScheduleDay("Sat",n);
         break;
      case EVERY_SUNDAY:
 	 ScheduleDay("Sun",n);
         break;
   }

   if(!re_schedule) {
      XtAddTimeOut(alarm_time, (XtTimerCallbackProc) Alarm, (XtPointer) n);
   } else {
      XtAddTimeOut(alarm_time,(XtTimerCallbackProc)ScheduleAlarm,(XtPointer)n);
   }

}

void ScheduleDay(dayStr, n)
     char *dayStr;
     int  n;
{
  long tloc;
  int re_schedule;
  char tmpStr[4];
  long alarm_time;

  strftime(tmpStr, 4, "%A", localtime(&tloc));
  if(!strncmp(dayStr,tmpStr,3)) {
    alarm_time = AlarmToday(n, tloc, &re_schedule);
  } else {
    /* schedule it to be scheduled at 00:00 */
    re_schedule = TRUE;
    alarm_time = Time2Midnite(GetCurTime(tloc));
  }
}

long AlarmToday(n, tloc, re_schedule)
   int n;
   long tloc;
   int *re_schedule;
{
   long   alarmTime,
	  curTime,
          alarm_time;

   /*
    * set up alarmTime to be the time of the alarm in seconds    
    */
   alarmTime = GetAlarmTime(n);
   /*
    * get the current time in seconds
    */
   curTime = GetCurTime(tloc);

   if(alarmTime > curTime) {
      alarm_time = ((alarmTime + (86400 - (curTime % 86400))) % 
                     86400) * 1000;
   } else { 
      /* schedule it to be scheduled at 00:00 */
      *re_schedule = TRUE;
      alarm_time = Time2Midnite(curTime);
   }

   return(alarm_time);

}

long Time2Midnite(curTime)
   long curTime;
{
   long  alarmTime,
	 tmp;
   alarmTime = 24 * 3600; 
   tmp = ((alarmTime + (86400 - (curTime % 86400))) % 86400) * 1000;
   return(tmp);
}

long GetAlarmTime(n)
   int n;
{
   long alarmTime;
   char tmpStr[3];

   tmpStr[0] = theAlarm[n].time[0];
   tmpStr[1] = theAlarm[n].time[1];
   tmpStr[2] = '\0';
   alarmTime = atoi(tmpStr) * 3600;
   tmpStr[0] = theAlarm[n].time[3];
   tmpStr[1] = theAlarm[n].time[4];
   tmpStr[2] = '\0';
   alarmTime += atoi(tmpStr) * 60;

   return(alarmTime);

}

long GetCurTime(tloc)
   long tloc;
{
   long curTime;
   char tmpStr[3];

   strftime( tmpStr, 3, "%H", localtime(&tloc)); /* get current hour */
   curTime = atoi(tmpStr) * 3600;
   strftime( tmpStr, 3, "%M", localtime(&tloc)); /* get current hour */
   curTime += atoi(tmpStr) * 60;
   strftime( tmpStr, 3, "%S", localtime(&tloc)); /* get current hour */
   curTime += atoi(tmpStr);

   return(curTime);
}