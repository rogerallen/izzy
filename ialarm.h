/***********************************************************************
 *
 * ialarm.h
 *
 * header file for ialarm.c
 *
 ***********************************************************************/

void Alarm(int n, XtIntervalId id);
void PlayAlarm(int n);
void ScheduleAlarm(int n, XtIntervalId id);
void ScheduleDay(char *dayStr, int n);
long AlarmToday(int n, long tloc, int *re_schedule);
long Time2Midnite(long curTime);
long GetAlarmTime(int n);
long GetCurTime(long tloc);
