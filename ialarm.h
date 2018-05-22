/***********************************************************************
 *
 * ialarm.h
 *
 * header file for ialarm.c
 *
 ***********************************************************************/

void Alarm(long n, XtIntervalId id);
void PlayAlarm(long n);
void ScheduleAlarm(long n, XtIntervalId id);
void ScheduleDay(char *dayStr, int n);
long AlarmToday(long n, long tloc, int *re_schedule);
long Time2Midnite(long curTime);
long GetAlarmTime(long n);
long GetCurTime(long tloc);
