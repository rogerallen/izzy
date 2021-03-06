//
// izzy:  A digital clock for X windows
//
// by Roger Allen
//
// Started: 05/13/1993 or was it 11/15/1991 as the old man page says?
// Updated: 05/22/2018
//
// Copyright (c) 1991-2018 Roger Allen
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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

// Global Variables
unsigned int    digitEncode[10];
digitType       digit[4];
char            segmentDir[7];  // segment direction from its origin
XPoint          segmentOrg[7];  // x,y coord of segment's origin
Widget          toplevel,       // widgets used in the izzy main screen
                background,     // form widget background
                clockFace,      // where digits are printed
                dateArea;       // where date is printed
GC              drawGC, eraseGC;

void xs_wprintf(Widget w, char *format, ...)
{
    va_list  args;
    char     str[1000];
    Arg      wargs[10];
    XmString xmstr;
    va_start(args, format);
    vsprintf(str, format, args);
    xmstr = XmStringLtoRCreate(str, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(wargs[0], XmNlabelString, xmstr);
    XtSetValues(w, wargs, 1);
    va_end(args);
}

void InitWidgets()
{
    Arg          args[32];
    int          n;
    static char translations[] = "\
      <BtnDown>: IMouseDown()\n   \
      <BtnUp>:   IMouseUp()\n     \
    ";

    // initialize the clock face digit structures
    InitDigits();

    // set up the widgets
    // background = form widget
    n = 0;
    XtSetArg(args[n], XmNwidth, CLOCK_WIDTH); n++;
    XtSetArg(args[n], XmNheight,
             CLOCK_HEIGHT + DATE_HEIGHT + ICON_AREA_HEIGHT); n++;
    background = XtCreateManagedWidget("background", xmFormWidgetClass,
                                       toplevel, args, n);

    // clockFace = drawing area widget
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

    // dateArea = label widget
    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopOffset, CLOCK_HEIGHT); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNheight, DATE_HEIGHT); n++;
    dateArea = XtCreateManagedWidget("dateArea", xmLabelWidgetClass,
                                     background, args, n);

    // set up the toplevel min/max size
    n = 0;
    XtSetArg(args[n], XmNmaxWidth, CLOCK_WIDTH); n++;
    XtSetArg(args[n], XmNmaxHeight,
             CLOCK_HEIGHT + DATE_HEIGHT + ICON_AREA_HEIGHT); n++;
    XtSetArg(args[n], XmNminWidth, CLOCK_WIDTH); n++;
    XtSetArg(args[n], XmNminHeight, CLOCK_HEIGHT); n++;
    XtSetValues(toplevel, args, n);

    // add expose event handler
    XtAddCallback(clockFace,
                  (String) XmNexposeCallback,
                  (XtCallbackProc) Update,
                  (XtPointer) NULL
        );

}

void InitDigits()
{

    // set up the encoding for the digits
    // see izzy.h for explanation
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

    // set up the segment direction variable
    // h=horizontal v=vertical
    segmentDir[0] = 'h';
    segmentDir[1] = 'v';
    segmentDir[2] = 'v';
    segmentDir[3] = 'h';
    segmentDir[4] = 'v';
    segmentDir[5] = 'v';
    segmentDir[6] = 'h';

    // set up the origin of each segment
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

    // set up the origin of each digit
    digit[0].origin.x = 3*SEG_WIDTH;
    digit[1].origin.x = 6*SEG_WIDTH + SEG_LENGTH;
    digit[2].origin.x = 10*SEG_WIDTH + 2*SEG_LENGTH;
    digit[3].origin.x = 14*SEG_WIDTH + 3*SEG_LENGTH;
    digit[0].origin.y = 2*SEG_WIDTH;
    digit[1].origin.y = 2*SEG_WIDTH;
    digit[2].origin.y = 2*SEG_WIDTH;
    digit[3].origin.y = 2*SEG_WIDTH;

}

void InitGC()
{
    XGCValues values;
    Arg args[10];
    int n;

    // create drawing GC
    n = 0;
    XtSetArg( args[n], XtNforeground, &values.foreground); n++;
    XtSetArg( args[n], XtNbackground, &values.background); n++;
    XtGetValues(clockFace, args, n);
    drawGC = XCreateGC(XtDisplay(clockFace), XtWindow(clockFace),
                       GCForeground | GCBackground,
                       &values);

    // create erasing GC
    n = 0;
    XtSetArg( args[n], XtNbackground, &values.foreground); n++;
    XtSetArg( args[n], XtNforeground, &values.background); n++;
    XtGetValues(clockFace, args, n);
    eraseGC = XCreateGC(XtDisplay(clockFace), XtWindow(clockFace),
                        GCForeground | GCBackground,
                        &values);

}

// this routine gets the current time, prints it out, and
// installs itself as a timeout routine when the next minute will occur.
void DoTime(id)
    XtIntervalId id;
{
    time_t tloc, next_minute;

    tloc = UpdateTime(FALSE);

    // set up interrupt for the next time
    next_minute = (60 - tloc % 60) * 1000;
    XtAddTimeOut(next_minute, (XtTimerCallbackProc) DoTime, (XtPointer)NULL);
}

void PrintTime(timeBuf, reverse)
    char *timeBuf;
    int reverse;
{
    char foo[2];
    int i;

    GC fg = reverse ? eraseGC : drawGC;
    GC bg = reverse ? drawGC : eraseGC;

    // the clock area
    XFillRectangle(XtDisplay(clockFace), XtWindow(clockFace),
                   bg, 0, 0, CLOCK_WIDTH, CLOCK_HEIGHT);
    // draw the two little dots
    XFillRectangle(XtDisplay(clockFace), XtWindow(clockFace),
                   fg,
                   2*SEG_LENGTH + 15*SEG_WIDTH/2,
                   SEG_LENGTH,
                   SEG_WIDTH, SEG_WIDTH);
    XFillRectangle(XtDisplay(clockFace), XtWindow(clockFace),
                   fg,
                   2*SEG_LENGTH + 15*SEG_WIDTH/2,
                   3*SEG_WIDTH + SEG_LENGTH,
                   SEG_WIDTH, SEG_WIDTH);

    // draw each digit
    for( i = 0; i < 4; i++) {
        foo[0] = timeBuf[i];
        foo[1] = '\0';
        digit[i].value = digitEncode[atoi(foo)];
        DrawDigit(digit[i], fg);
    }

}

// callback from mouse up
void IMouseUp(w, event, params, num_params)
    Widget w;
    XEvent *event;
    String *params;
    Cardinal *num_params;
{
    UpdateTime(FALSE);
}

// callback from mouse down
void IMouseDown(w, event, params, num_params)
    Widget w;
    XEvent *event;
    String *params;
    Cardinal *num_params;
{
    UpdateTime(TRUE);
}

void DrawDigit(digit, fg)
    digitType digit;
    GC fg;
{
    int i;

    // draw each enabled segment in the current digit
    for(i = 0; i < 7; i++) {
        if ( (1 << (6-i)) & digit.value)
            DrawSegment( digit.origin, i, fg);
    }
}

void DrawSegment( origin, segNum, fg)
    XPoint origin;
    int segNum;
    GC fg;
{
    XPoint base, points[7];

    base.x = origin.x + segmentOrg[segNum].x;
    base.y = origin.y + segmentOrg[segNum].y;

    // define the segment via the array points
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

    // fill the segment
    XDrawLines(XtDisplay(clockFace),
               XtWindow(clockFace),
               fg,
               points, 7, CoordModeOrigin
        );
    XFillPolygon(XtDisplay(clockFace),
                 XtWindow(clockFace),
                 fg,
                 points, 7, Convex, CoordModeOrigin
        );
}

void Update() {
    UpdateTime(FALSE);
}

time_t UpdateTime(int reverse)
{
    time_t       tloc;
    char         dateBuf[DATE_LEN], timeBuf[TIME_LEN];
    // get the GMT time in time_t format
    time(&tloc);

    if(reverse) {
        // write to the date area.
        sprintf( dateBuf, "Izzy, by Roger Allen");
        xs_wprintf( dateArea, "%s", dateBuf);
    }
    else {
        // get the date into dateBuf, and write it to the date area.
        strftime( dateBuf, (int)DATE_LEN, "%A, %B %d", localtime(&tloc));
        xs_wprintf( dateArea, "%s", dateBuf);
    }
    // get the time into timeBuf, and write it to the time area.
    strftime( timeBuf, (int)DATE_LEN, "%I%M", localtime(&tloc));
    PrintTime(timeBuf, reverse);

    return tloc;
}

int main (argc, argv)
    int argc;
    char *argv[];
{
    int ac;
    char **av;
    static XtActionsRec newActions[] = {
        {(String) "IMouseDown", (XtActionProc) IMouseDown},
        {(String) "IMouseUp", (XtActionProc) IMouseUp}
    };

    ac = argc - 1;
    av = argv + 1;
    while (ac && av[0][0] == '-') {
        switch (av[0][1]) {
        case 'v':
            printf("Izzy, Version 2.0, by Roger Allen\n");
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

    InitWidgets();
    // add the timeout with an initial delay
    XtAddTimeOut(500, (XtTimerCallbackProc) DoTime, (XtPointer)NULL);
    XtRealizeWidget(toplevel);
    InitGC();

    // loop forever
    XtMainLoop();

}
