/************************************************************************
 *
 * iinit.c
 *
 * Routines to initialize the Widgets, etc. for Izzy
 *
 ***********************************************************************/
#include "iinit.h"
#include "imain.h"

void InitWidgets()
{
   Arg		args[32];
   int		n;
   static char translations[] = "\
      <BtnDown>: IMouseDown()\n\
      <BtnUp>: IMouseUp()\n\
   ";

   /*
    * set up curAlarm = 0 
    */
   curAlarm = 0;

   /*
    * initialize the clock face digit structures
    */
   InitDigits();

   /*
    * set up the widgets
    */
   /*
    * background = form widget
    */
   n = 0;
   XtSetArg(args[n], XmNwidth, CLOCK_WIDTH); n++;
   XtSetArg(args[n], XmNheight, 
		CLOCK_HEIGHT + DATE_HEIGHT + ICON_AREA_HEIGHT); n++;
   background = XtCreateManagedWidget("background", xmFormWidgetClass, 
					toplevel, args, n);

   /*
    * clockFace = drawing area widget
    */
   n = 0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNtopOffset, 0); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNwidth, CLOCK_WIDTH); n++;
   XtSetArg(args[n], XmNheight, CLOCK_HEIGHT); n++;
   clockFace = XtCreateManagedWidget("clockFace", xmDrawingAreaWidgetClass,
				     background, args, n);

   XtOverrideTranslations(
      clockFace,
      XtParseTranslationTable(translations)
   );

   /*
    * dateArea = label widget
    */
   n = 0;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNtopOffset, CLOCK_HEIGHT); n++;
   XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNheight, DATE_HEIGHT); n++;
   dateArea = XtCreateManagedWidget("dateArea", xmLabelWidgetClass,
				   background, args, n);

   /*
    * set up the toplevel min/max size
    */
   n = 0;
   XtSetArg(args[n], XmNmaxWidth, CLOCK_WIDTH); n++;
   XtSetArg(args[n], XmNmaxHeight, 
		CLOCK_HEIGHT + DATE_HEIGHT + ICON_AREA_HEIGHT); n++;
   XtSetArg(args[n], XmNminWidth, CLOCK_WIDTH); n++;
   XtSetArg(args[n], XmNminHeight, CLOCK_HEIGHT); n++;
   XtSetValues(toplevel, args, n);

   /*
    * get the alarm file input
    */
   numAlarms = 0;
   ParseAlarmFile();

   /* 
    * add expose event handler 

* will add resize callback here *

    */
   XtAddCallback(clockFace, 
		 (String) XmNexposeCallback, 
		 (XtCallbackProc) Update,
		 (XtPointer) NULL
		);

}

void InitDigits()  
{

   /* 
    * set up the encoding for the digits
    * see izzy.h for explanation
    */
   digitEncode[0] = 0x77;
   digitEncode[1] = 0x12;
   digitEncode[2] = 0x5d;
   digitEncode[3] = 0x5b;
   digitEncode[4] = 0x3a;
   digitEncode[5] = 0x6b;
   digitEncode[6] = 0x6f;
   digitEncode[7] = 0x52;
   digitEncode[8] = 0x7f;
   digitEncode[9] = 0x7b;

   /*
    * set up the segment direction variable   
    * h=horizontal v=vertical
    */
   segmentDir[0] = 'h';
   segmentDir[1] = 'v';
   segmentDir[2] = 'v';
   segmentDir[3] = 'h';
   segmentDir[4] = 'v';
   segmentDir[5] = 'v';
   segmentDir[6] = 'h';

   /*
    * set up the origin of each segment
    */
   segmentOrg[0].x = 0;
   segmentOrg[0].y = 0;
   segmentOrg[1].x = 0;
   segmentOrg[1].y = 0;
   segmentOrg[2].x = 1*SEG_LENGTH;
   segmentOrg[2].y = 0;
   segmentOrg[3].x = 0;
   segmentOrg[3].y = 1*SEG_LENGTH;
   segmentOrg[4].x = 0;
   segmentOrg[4].y = 1*SEG_LENGTH;
   segmentOrg[5].x = 1*SEG_LENGTH;
   segmentOrg[5].y = 1*SEG_LENGTH;
   segmentOrg[6].x = 0;
   segmentOrg[6].y = 2*SEG_LENGTH;

   /*
    * set up the origin of each digit   
    */
   digit[0].origin.x = 3*SEG_WIDTH;
   digit[1].origin.x = 6*SEG_WIDTH + SEG_LENGTH;
   digit[2].origin.x = 10*SEG_WIDTH + 2*SEG_LENGTH;
   digit[3].origin.x = 14*SEG_WIDTH + 3*SEG_LENGTH;
   digit[0].origin.y = 2*SEG_WIDTH;
   digit[1].origin.y = 2*SEG_WIDTH; 
   digit[2].origin.y = 2*SEG_WIDTH;
   digit[3].origin.y = 2*SEG_WIDTH;

}

void InitTime()
{
   /*
    * add the timeout with an initial delay 
    */
   XtAddTimeOut(INIT_DELAY, (XtTimerCallbackProc) DoTime, (XtPointer)NULL);

}

void InitGC()
{
   XGCValues values;
   Arg args[10];
   int n;

   /*
    * create drawing GC
    */
   n = 0;
   XtSetArg( args[n], XtNforeground, &values.foreground); n++;
   XtSetArg( args[n], XtNbackground, &values.background); n++;
   XtGetValues(clockFace, args, n);
   drawGC = XCreateGC(XtDisplay(clockFace), XtWindow(clockFace), 
		      GCForeground | GCBackground, 
		      &values);

   /*
    * create erasing GC
    */
   n = 0;
   XtSetArg( args[n], XtNbackground, &values.foreground); n++;
   XtSetArg( args[n], XtNbackground, &values.background); n++;
   XtGetValues(clockFace, args, n);
   eraseGC = XCreateGC(XtDisplay(clockFace), XtWindow(clockFace), 
		      GCForeground | GCBackground, 
		      &values);

}

