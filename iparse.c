/************************************************************************
 *
 * iparse.c
 *
 * Routines to parse the izzyrc file
 *
 ***********************************************************************/
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>
#include <Xm/Xm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "izzy.h"
#include "iparse.h"

extern AlarmType	theAlarm[MAX_ALARMS];
extern int		numAlarms;

void ParseAlarmFile()
{
   FILE *alarmFile;
   int  lineCount;
   char aLine[255],
	tmpStr[50],
	alarmFileName[255];

   strcpy(tmpStr,"HOME");
   sprintf(alarmFileName, "%s/.izzyrc", getenv(tmpStr));
/*   strcat(alarmFileName,"/.izzyrc"); */

   if((alarmFile = fopen(alarmFileName,"r")) == NULL)
      fprintf(stderr,"Could not open alarm file %s for reading\n", alarmFileName);
   else {
      lineCount = 0;
      while(fgets(aLine,255,alarmFile) != NULL) {
	 if(aLine[0] != '#')
            ParseLine((char *)aLine, lineCount++);
      }
      fclose(alarmFile);
   }
}

void ParseLine(theLine,theAlarmNumber)
   char theLine[255];
   int theAlarmNumber;
{
   char firstWord[80],
	secondWord[80];
   /*
    * get the first word, should be "only" or "every"
    */
   strcpy(firstWord,strtok(theLine," "));

   if(!strncmp(firstWord,"onl",3)) {
      /* 1st word is ONLY */
      strcpy(secondWord,strtok(NULL," "));

      if(!strncmp(secondWord,"tod",3)) {
	 theAlarm[theAlarmNumber].type = ONLY_TODAY;
         strcpy(theAlarm[theAlarmNumber].date,"today");
      } else {
	 theAlarm[theAlarmNumber].type = ONLY_MMDDYY;
	 strcpy(theAlarm[theAlarmNumber].date,secondWord);
      }

      ParseAtTime(theAlarmNumber);

   } else if(!strncmp(firstWord,"eve",3)) {
      /* 1st word is EVERY */
      strcpy(secondWord,strtok(NULL," "));

      strcpy(theAlarm[theAlarmNumber].date,"<n/a>");

      if(!strncmp(secondWord,"day",3))
         theAlarm[theAlarmNumber].type = EVERY_DAY;
      else if(!strncmp(secondWord,"mon",3))
         theAlarm[theAlarmNumber].type = EVERY_MONDAY;
      else if(!strncmp(secondWord,"tue",3))
         theAlarm[theAlarmNumber].type = EVERY_TUESDAY;
      else if(!strncmp(secondWord,"wed",3))
         theAlarm[theAlarmNumber].type = EVERY_WEDNESDAY;
      else if(!strncmp(secondWord,"thu",3))
         theAlarm[theAlarmNumber].type = EVERY_THURSDAY;
      else if(!strncmp(secondWord,"fri",3))
         theAlarm[theAlarmNumber].type = EVERY_FRIDAY;
      else if(!strncmp(secondWord,"sat",3))
         theAlarm[theAlarmNumber].type = EVERY_SATURDAY;
      else if(!strncmp(secondWord,"sun",3))
         theAlarm[theAlarmNumber].type = EVERY_SUNDAY;
      else if(!strncmp(secondWord,"hou",3))
         theAlarm[theAlarmNumber].type = EVERY_HOUR;

     ParseAtTime(theAlarmNumber);

   } else {
     fprintf(stderr,"there is an error on line %s of the alarm file\n",
	     theLine);
   }

   ParseCommand(theAlarmNumber);
   numAlarms++;
/****
   fprintf(stderr,"Alarm[%u].type = %u\n", theAlarmNumber,
		  theAlarm[theAlarmNumber].type);
   fprintf(stderr,"Alarm[%u].date = %s\n", theAlarmNumber,
		  theAlarm[theAlarmNumber].date);
*****
   fprintf(stderr,"Alarm[%u].time = %s\n", theAlarmNumber,
		  theAlarm[theAlarmNumber].time);
   fprintf(stderr,"Alarm[%u].command = %s\n", theAlarmNumber,
		  theAlarm[theAlarmNumber].command);
*****/
}

void ParseAtTime(theAlarmNumber)
   int  theAlarmNumber;
{
   char thirdWord[80];

   /*
    * get the "at"
    */
   strcpy(thirdWord,strtok(NULL," "));

   /*
    * get the time
    */
   strcpy(theAlarm[theAlarmNumber].time,strtok(NULL," "));
}

void ParseCommand(theAlarmNumber)
   int  theAlarmNumber;
{
    //char thirdWord[80];

   /*
    * get the command
    */
   strcpy(theAlarm[theAlarmNumber].command,strtok(NULL,"\n"));
}
