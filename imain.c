/***********************************************************************
 * imain.c:  A digital alarm clock for X windows that executes commands
 *           The main loop.
 *
 * Revision History
 * Date		Changes
 *---------------------------------------------------------------------
 * 05/13/93     Just started Izzy
 *
 * Bugs and Suggestions
 * Date	Fixed	Bug Description
 *---------------------------------------------------------------------
 *
 **********************************************************************/
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>
#include <Xm/Xm.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "izzy.h"
#include "ialarm.h"
#include "imain.h"
#include "iinit.h"
#include "xs_wprintf.h"

/***********************************************************************
 *
 * Global Variables
 *
 ***********************************************************************/
unsigned int    digitEncode[10];

digitType       digit[4];

char		segmentDir[7]; 	/* segment direction from its origin */
XPoint		segmentOrg[7];	/* x,y coord of segment's origin */

		/* global widgets used in the izzy main screen */
Widget        	toplevel,
		background,	/* form widget background */
		clockFace,	/* where digits are printed */
		dateArea;	/* where date is printed */

GC		drawGC,
eraseGC;

AlarmType	theAlarm[MAX_ALARMS];
int		numAlarms;
int		curAlarm;


int main (argc, argv)
   int argc;
   char *argv[];
{
   int ac;
   char **av;
   int i;
   static XtActionsRec newActions[] = {
      {(String) "IMouseDown", (XtActionProc) IMouseDown},
      {(String) "IMouseUp", (XtActionProc) IMouseUp}
   };

   ac = argc - 1;
   av = argv + 1;
   while (ac && av[0][0] == '-') {
      switch (av[0][1]) {
         case 'v':
	    printf("Izzy, Version 1.0, by Roger Allen\n");
	    exit(0);
            break;
         case 'h':
         case '?':
	    printf("Look at Izzy's man page for usage info.\n");
	    exit(0);
            break;
      }
      ac--;
      av++;
   }

   toplevel = XtInitialize(argv[0], "Izzy", NULL, 0, &argc, argv);

   XtAddActions(newActions, XtNumber(newActions));

   /*
    * initialize all children
    */
   InitWidgets();

   /*
    * get the initial time and install timeout to update every minute
    */
   InitTime();

   /*
    * schedule all alarms read in from the .izzyrc file
    */
   for(i = 0; i < numAlarms; i++)
       ScheduleAlarm(i, (XtIntervalId)NULL);

   XtRealizeWidget(toplevel);

   /*
    * after realization, initialize the things that needed widgets to be
    * realized.
    */
   InitGC();

   /*
    * loop forever
    */
   XtMainLoop();

}

/******************************
 *
 * DoTime()
 *
 * this routine gets the current time, prints it out, and
 * installs itself as a timeout routine when the next minute will occur.
 *
 ******************************/
void DoTime(id)
   XtIntervalId id;
{
   time_t      	tloc,
		next_minute;
   char		dateBuf[DATE_LEN],
		timeBuf[TIME_LEN];

   /*
    * get the GMT time in time_t format
    */
   time(&tloc);

   /*
    * get the date into dateBuf, and write it to the date area.
    */
   strftime( dateBuf, (int)DATE_LEN, "%A, %B %d", localtime(&tloc));
   xs_wprintf( dateArea, "%s", dateBuf);

   /*
    * get the time into timeBuf, and write it to the time area.
    */
   strftime( timeBuf, (int)DATE_LEN, "%I%M", localtime(&tloc));
   PrintTime(timeBuf, FALSE);

   /*
    * set up interrupt for the next time
    */
   next_minute = (60 - tloc % 60) * 1000;

   /*
    * add the timeout
    */
   XtAddTimeOut(next_minute, (XtTimerCallbackProc) DoTime, (XtPointer)NULL);
}

void PrintTime(timeBuf, reverse)
   char *timeBuf;
   int reverse;
{
   char foo[2];
   int i;

   if(!reverse) {
      /*
       * erase the clock area
       */
      XFillRectangle(XtDisplay(clockFace), XtWindow(clockFace),
		  eraseGC, 0, 0, CLOCK_WIDTH, CLOCK_HEIGHT);
      /*
       * draw the two little dots
       */
      XFillRectangle(XtDisplay(clockFace), XtWindow(clockFace),
		  drawGC,
		  2*SEG_LENGTH + 15*SEG_WIDTH/2,
		  SEG_LENGTH,
		  SEG_WIDTH, SEG_WIDTH);
      XFillRectangle(XtDisplay(clockFace), XtWindow(clockFace),
		  drawGC,
		  2*SEG_LENGTH + 15*SEG_WIDTH/2,
		  3*SEG_WIDTH + SEG_LENGTH,
		  SEG_WIDTH, SEG_WIDTH);

      /*
       * draw each digit
       */
      for( i = 0; i < 4; i++) {
         /*
          * update the value of each segment
          */
         foo[0] = timeBuf[i];
         foo[1] = '\0';
         digit[i].value = digitEncode[atoi(foo)];
         /*
          * draw each digit
          */
         DrawDigit(digit[i], FALSE);
      }
   } else {
      /*
       * erase the clock area
       */
      XFillRectangle(XtDisplay(clockFace), XtWindow(clockFace),
		  drawGC, 0, 0, CLOCK_WIDTH, CLOCK_HEIGHT);
      /*
       * draw the two little dots
       */
      XFillRectangle(XtDisplay(clockFace), XtWindow(clockFace),
		  eraseGC,
		  2*SEG_LENGTH + 15*SEG_WIDTH/2,
		  SEG_LENGTH,
		  SEG_WIDTH, SEG_WIDTH);
      XFillRectangle(XtDisplay(clockFace), XtWindow(clockFace),
		  eraseGC,
		  2*SEG_LENGTH + 15*SEG_WIDTH/2,
		  3*SEG_WIDTH + SEG_LENGTH,
		  SEG_WIDTH, SEG_WIDTH);

      /*
       * draw each digit
       */
      for( i = 0; i < 4; i++) {
         /*
          * update the value of each segment
          */
         foo[0] = timeBuf[i];
         foo[1] = '\0';
         digit[i].value = digitEncode[atoi(foo)];
         /*
          * draw each digit
          */
         DrawDigit(digit[i], TRUE);
      }
   }

}

/** callback from mouse up **/
void IMouseUp(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{

   char timeBuf[TIME_LEN];
   char dateBuf[DATE_LEN];
   time_t tloc;

   /*
    * get the GMT time in time_t format
    */
   time(&tloc);

   strftime( timeBuf, (size_t)DATE_LEN, "%I%M", localtime(&tloc));

   /*
    * get the date into dateBuf, and write it to the date area.
    */
   strftime( dateBuf, (size_t)DATE_LEN, "%A, %B %d", localtime(&tloc));
   xs_wprintf( dateArea, "%s", dateBuf);

   /*
    * print the time in the clock area
    */
   PrintTime(timeBuf, FALSE);

}
/** callback from mouse down **/
void IMouseDown(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{

   char timeBuf[TIME_LEN];
   char dateBuf[DATE_LEN];
   //char foo[2];
   //int bar;
   //int i;
   time_t tloc;
   //time_t newTloc;

   /*
    * get the GMT time in time_t format
    */
   time(&tloc);

   strftime( timeBuf, (int)DATE_LEN, "%I%M", localtime(&tloc));

   /*
    * print the time in the clock area
    */
   PrintTime(timeBuf, TRUE);

   /*
    * write to the date area.
    */
   sprintf( dateBuf, "Izzy, by Roger Allen");
   xs_wprintf( dateArea, "%s", dateBuf);

}

void DrawDigit(digit, reverse)
   digitType digit;
   int reverse;
{
   int i;

   /*
    * draw each enabled segment in the current digit
    */
    for(i = 0; i < 7; i++) {
       if ( (1 << (6-i)) & digit.value)
	  DrawSegment( digit.origin, i, reverse);
    }
}

void DrawSegment( origin, segNum, reverse)
   XPoint origin;
   int segNum;
   int reverse;
{
   XPoint 	base,
		points[7];


   base.x = origin.x + segmentOrg[segNum].x;
   base.y = origin.y + segmentOrg[segNum].y;

   /*
    * define the segment via the array points
    */
   switch(segmentDir[segNum]) {
      case 'h': points[0].x = base.x;
                points[0].y = base.y;
                points[1].x = base.x + SEG_WIDTH/2;
                points[1].y = base.y - SEG_WIDTH/2;
                points[2].x = base.x + SEG_LENGTH - SEG_WIDTH/2;
                points[2].y = base.y - SEG_WIDTH/2;
                points[3].x = base.x + SEG_LENGTH;
                points[3].y = base.y;
                points[4].x = base.x + SEG_LENGTH - SEG_WIDTH/2;
                points[4].y = base.y + SEG_WIDTH/2;
                points[5].x = base.x + SEG_WIDTH/2;
                points[5].y = base.y + SEG_WIDTH/2;
                points[6].x = base.x;
                points[6].y = base.y;
		break;
      case 'v': points[0].x = base.x;
		points[0].y = base.y;
                points[1].x = base.x + SEG_WIDTH/2;
		points[1].y = base.y + SEG_WIDTH/2;
                points[2].x = base.x + SEG_WIDTH/2;
		points[2].y = base.y + SEG_LENGTH - SEG_WIDTH/2;
                points[3].x = base.x;
		points[3].y = base.y + SEG_LENGTH;
                points[4].x = base.x - SEG_WIDTH/2;
		points[4].y = base.y + SEG_LENGTH - SEG_WIDTH/2;
                points[5].x = base.x - SEG_WIDTH/2;
		points[5].y = base.y + SEG_WIDTH/2;
                points[6].x = base.x;
		points[6].y = base.y;
		break;
   }

   /*
    * fill the segment
    */
   if(!reverse) {
      XDrawLines(XtDisplay(clockFace),
         XtWindow(clockFace),
         drawGC,
         points, 7, CoordModeOrigin
      );
      XFillPolygon(XtDisplay(clockFace),
         XtWindow(clockFace),
         drawGC,
         points, 7, Convex, CoordModeOrigin
      );
   } else {
      XDrawLines(XtDisplay(clockFace),
         XtWindow(clockFace),
         eraseGC,
         points, 7, CoordModeOrigin
      );
      XFillPolygon(XtDisplay(clockFace),
         XtWindow(clockFace),
         eraseGC,
         points, 7, Convex, CoordModeOrigin
      );
   }

}

void Update()
{
   time_t      	tloc;
   char		dateBuf[DATE_LEN],
		timeBuf[TIME_LEN];
   /*
    * redraw the screen
    */
   /*
    * get the GMT time in time_t format
    */
   time(&tloc);

   /*
    * get the date into dateBuf, and write it to the date area.
    */
   strftime( dateBuf, (int)DATE_LEN, "%A, %B %d", localtime(&tloc));
   xs_wprintf( dateArea, "%s", dateBuf);

   /*
    * get the time into timeBuf, and write it to the time area.
    */
   strftime( timeBuf, (int)DATE_LEN, "%I%M", localtime(&tloc));
   PrintTime(timeBuf, FALSE);
}
