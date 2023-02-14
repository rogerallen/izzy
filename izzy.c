//
// izzy:  A digital clock for X windows
//
// by Roger Allen
//
// Started: 11/15/1991
//
// Copyright (c) 1991-2023 Roger Allen
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
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

// ----------------------------------------------------------------------
// Each digit container.  The value field is encoded so that each bit
// stands for a segment in a digit.  Bits for each digit are stored in
// g_digit_encode[].
typedef struct {
    XPoint origin;
    unsigned int value;
} digitType;

// ----------------------------------------------------------------------
// constants
const int DATE_LEN = 40;
const int TIME_LEN = 5;
const int SEG_LENGTH = 17;
const int SEG_WIDTH = 4;
const int CLOCK_WIDTH = 17 * SEG_WIDTH + 4 * SEG_LENGTH;
const int CLOCK_HEIGHT = 11 * SEG_WIDTH / 2 + 2 * SEG_LENGTH;
const int DATE_HEIGHT = 20;
const int LABEL_HEIGHT = 25;
const int BUTTON_WIDTH = 55;

// ----------------------------------------------------------------------
// declarations
void xs_wprintf(Widget w, char *format, ...);
void InitWidgets(void);
void InitGC(void);
void IMouseUp(Widget w, XEvent *event, String *params, Cardinal *num_params);
void IMouseDown(Widget w, XEvent *event, String *params, Cardinal *num_params);
void TimeCallback(XtPointer d, XtIntervalId *id);
void DrawTime(char *timeBuf, int reverse);
void DrawDigit(digitType digit, GC fg);
void DrawSegment(XPoint origin, int segNum, GC fg);
void UpdateOnExpose(void);
time_t UpdateTime(int write_date);

// ----------------------------------------------------------------------
// Global Variables
//
// The digit segments are arranged like so:
//  o-0-     o=origin
//  |   |
//  1   2
//  |   |
//   -3-
//  |   |
//  4   5
//  |   |
//   -6-
//
// segment direction from its origin
char g_segment_direction[7] = {'h', 'v', 'v', 'h', 'v', 'v', 'h'};
// x,y coord of segment's origin
XPoint g_segment_origin[7] = {{.x = 0, .y = 0},
                              {.x = 0, .y = 0},
                              {.x = 1 * SEG_LENGTH, .y = 0},
                              {.x = 0, .y = 1 * SEG_LENGTH},
                              {.x = 0, .y = 1 * SEG_LENGTH},
                              {.x = 1 * SEG_LENGTH, .y = 1 * SEG_LENGTH},
                              {.x = 0, .y = 2 * SEG_LENGTH}};

// g_digit_encode maps a digit to the bits that encode each segment.
//         Segment Number (bit)
// Digit | 6 5 4.3 2 1 0
// ------+------.--------
//   0   | x x x.  x x x  | 0x77
//   1   |   x  .  x      | 0x24
//   2   | x   x.x x   x  | 0x5d
//   3   | x x  .x x   x  | 0x6d
//   4   |   x  .x x x    | 0x2e
//   5   | x x  .x   x x  | 0x6b
//   6   | x x x.x   x x  | 0x7b
//   7   |   x  .  x   x  | 0x25
//   8   | x x x.x x x x  | 0x7f
//   9   | x x  .x x x x  | 0x6f
//
unsigned int g_digit_encode[10] = {0x77, 0x24, 0x5d, 0x6d, 0x2e,
                                   0x6b, 0x7b, 0x25, 0x7f, 0x6f};

// each digit's origin is set here, the value will be set during runtime
digitType g_digit[4] = {
    {.origin = {.x = 3 * SEG_WIDTH, .y = 2 * SEG_WIDTH}, .value = 0},
    {.origin = {.x = 6 * SEG_WIDTH + SEG_LENGTH, .y = 2 * SEG_WIDTH},
     .value = 0},
    {.origin = {.x = 10 * SEG_WIDTH + 2 * SEG_LENGTH, .y = 2 * SEG_WIDTH},
     .value = 0},
    {.origin = {.x = 14 * SEG_WIDTH + 3 * SEG_LENGTH, .y = 2 * SEG_WIDTH},
     .value = 0}};

Widget g_toplevel_widget, // widgets used in the izzy main screen
    g_background_widget,  // form widget background
    g_clock_face_widget,  // where digits are printed
    g_date_area_widget;   // where date is printed
GC g_draw_GC, g_erase_GC;

// ----------------------------------------------------------------------
void xs_wprintf(Widget w, char *format, ...)
{
    va_list args;
    char str[1000];
    Arg wargs[10];
    XmString xmstr;
    va_start(args, format);
    vsprintf(str, format, args);
    xmstr = XmStringLtoRCreate(str, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(wargs[0], XmNlabelString, xmstr);
    XtSetValues(w, wargs, 1);
    va_end(args);
}

// ----------------------------------------------------------------------
void InitWidgets(void)
{
    Arg args[32];
    int n;
    static char translations[] = "\
      <BtnDown>: IMouseDown()\n   \
      <BtnUp>:   IMouseUp()\n     \
    ";

    // set up the widgets
    // background = form widget
    n = 0;
    XtSetArg(args[n], XmNwidth, CLOCK_WIDTH);
    n++;
    XtSetArg(args[n], XmNheight, CLOCK_HEIGHT + DATE_HEIGHT);
    n++;
    g_background_widget = XtCreateManagedWidget("background", xmFormWidgetClass,
                                                g_toplevel_widget, args, n);

    // clockFace = drawing area widget
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);
    n++;
    XtSetArg(args[n], XmNtopOffset, 0);
    n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);
    n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);
    n++;
    XtSetArg(args[n], XmNwidth, CLOCK_WIDTH);
    n++;
    XtSetArg(args[n], XmNheight, CLOCK_HEIGHT);
    n++;
    g_clock_face_widget = XtCreateManagedWidget(
        "clockFace", xmDrawingAreaWidgetClass, g_background_widget, args, n);

    XtOverrideTranslations(g_clock_face_widget,
                           XtParseTranslationTable(translations));

    // dateArea = label widget
    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);
    n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);
    n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);
    n++;
    XtSetArg(args[n], XmNtopOffset, CLOCK_HEIGHT);
    n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);
    n++;
    XtSetArg(args[n], XmNheight, DATE_HEIGHT);
    n++;
    g_date_area_widget = XtCreateManagedWidget("dateArea", xmLabelWidgetClass,
                                               g_background_widget, args, n);

    // set up the toplevel min/max size
    n = 0;
    XtSetArg(args[n], XmNmaxWidth, CLOCK_WIDTH);
    n++;
    XtSetArg(args[n], XmNmaxHeight, CLOCK_HEIGHT + DATE_HEIGHT);
    n++;
    XtSetArg(args[n], XmNminWidth, CLOCK_WIDTH);
    n++;
    XtSetArg(args[n], XmNminHeight, CLOCK_HEIGHT);
    n++;
    XtSetValues(g_toplevel_widget, args, n);

    // add expose event handler
    XtAddCallback(g_clock_face_widget, (String)XmNexposeCallback,
                  (XtCallbackProc)UpdateOnExpose, (XtPointer)NULL);
}

// ----------------------------------------------------------------------
void InitGC(void)
{
    XGCValues values;
    Arg args[10];
    int n;

    // create drawing GC
    n = 0;
    XtSetArg(args[n], XtNforeground, &values.foreground);
    n++;
    XtSetArg(args[n], XtNbackground, &values.background);
    n++;
    XtGetValues(g_clock_face_widget, args, n);
    g_draw_GC =
        XCreateGC(XtDisplay(g_clock_face_widget), XtWindow(g_clock_face_widget),
                  GCForeground | GCBackground, &values);

    // create erasing GC
    n = 0;
    XtSetArg(args[n], XtNbackground, &values.foreground);
    n++;
    XtSetArg(args[n], XtNforeground, &values.background);
    n++;
    XtGetValues(g_clock_face_widget, args, n);
    g_erase_GC =
        XCreateGC(XtDisplay(g_clock_face_widget), XtWindow(g_clock_face_widget),
                  GCForeground | GCBackground, &values);
}

// ----------------------------------------------------------------------
// callback from mouse up
void IMouseUp(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    (void)w, (void)event, (void)params,
        (void)num_params; // fix Wunused-parameter
    UpdateTime(FALSE);
}

// ----------------------------------------------------------------------
// callback from mouse down
void IMouseDown(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    (void)w, (void)event, (void)params,
        (void)num_params; // fix Wunused-parameter
    UpdateTime(TRUE);
}

// ----------------------------------------------------------------------
// this routine gets the current time, prints it out, and
// installs itself as a timeout routine when the next minute will occur.
void TimeCallback(XtPointer d, XtIntervalId *id)
{
    (void)d, (void)id; // fix unused warning

    time_t tloc, next_minute;

    tloc = UpdateTime(FALSE);

    // set up interrupt for the next time
    next_minute = (60 - tloc % 60) * 1000;
    XtAddTimeOut(next_minute, (XtTimerCallbackProc)TimeCallback,
                 (XtPointer)NULL);
}

// ----------------------------------------------------------------------
void DrawTime(char *timeBuf, int reverse)
{
    char foo[2];
    int i;

    GC fg = reverse ? g_erase_GC : g_draw_GC;
    GC bg = reverse ? g_draw_GC : g_erase_GC;

    // the clock area
    XFillRectangle(XtDisplay(g_clock_face_widget),
                   XtWindow(g_clock_face_widget), bg, 0, 0, CLOCK_WIDTH,
                   CLOCK_HEIGHT);
    // draw the two little dots
    XFillRectangle(
        XtDisplay(g_clock_face_widget), XtWindow(g_clock_face_widget), fg,
        2 * SEG_LENGTH + 15 * SEG_WIDTH / 2, SEG_LENGTH, SEG_WIDTH, SEG_WIDTH);
    XFillRectangle(XtDisplay(g_clock_face_widget),
                   XtWindow(g_clock_face_widget), fg,
                   2 * SEG_LENGTH + 15 * SEG_WIDTH / 2,
                   3 * SEG_WIDTH + SEG_LENGTH, SEG_WIDTH, SEG_WIDTH);

    // draw each digit
    for (i = 0; i < 4; i++) {
        foo[0] = timeBuf[i];
        foo[1] = '\0';
        g_digit[i].value = g_digit_encode[atoi(foo)];
        DrawDigit(g_digit[i], fg);
    }
}

// ----------------------------------------------------------------------
void DrawDigit(digitType digit, GC fg)
{
    int i;

    // draw each enabled segment in the current digit
    for (i = 0; i < 7; i++) {
        if ((1 << i) & digit.value) {
            DrawSegment(digit.origin, i, fg);
        }
    }
}

// ----------------------------------------------------------------------
void DrawSegment(XPoint origin, int segNum, GC fg)
{
    XPoint base, points[7];

    base.x = origin.x + g_segment_origin[segNum].x;
    base.y = origin.y + g_segment_origin[segNum].y;

    // define the segment via the array points
    switch (g_segment_direction[segNum]) {
    case 'h':
        points[0].x = base.x;
        points[0].y = base.y;
        points[1].x = base.x + SEG_WIDTH / 2;
        points[1].y = base.y - SEG_WIDTH / 2;
        points[2].x = base.x + SEG_LENGTH - SEG_WIDTH / 2;
        points[2].y = base.y - SEG_WIDTH / 2;
        points[3].x = base.x + SEG_LENGTH;
        points[3].y = base.y;
        points[4].x = base.x + SEG_LENGTH - SEG_WIDTH / 2;
        points[4].y = base.y + SEG_WIDTH / 2;
        points[5].x = base.x + SEG_WIDTH / 2;
        points[5].y = base.y + SEG_WIDTH / 2;
        points[6].x = base.x;
        points[6].y = base.y;
        break;
    case 'v':
        points[0].x = base.x;
        points[0].y = base.y;
        points[1].x = base.x + SEG_WIDTH / 2;
        points[1].y = base.y + SEG_WIDTH / 2;
        points[2].x = base.x + SEG_WIDTH / 2;
        points[2].y = base.y + SEG_LENGTH - SEG_WIDTH / 2;
        points[3].x = base.x;
        points[3].y = base.y + SEG_LENGTH;
        points[4].x = base.x - SEG_WIDTH / 2;
        points[4].y = base.y + SEG_LENGTH - SEG_WIDTH / 2;
        points[5].x = base.x - SEG_WIDTH / 2;
        points[5].y = base.y + SEG_WIDTH / 2;
        points[6].x = base.x;
        points[6].y = base.y;
        break;
    }

    // fill the segment
    XDrawLines(XtDisplay(g_clock_face_widget), XtWindow(g_clock_face_widget),
               fg, points, 7, CoordModeOrigin);
    XFillPolygon(XtDisplay(g_clock_face_widget), XtWindow(g_clock_face_widget),
                 fg, points, 7, Convex, CoordModeOrigin);
}

// ----------------------------------------------------------------------
void UpdateOnExpose(void) { UpdateTime(FALSE); }

// ----------------------------------------------------------------------
time_t UpdateTime(int reverse)
{
    time_t tloc;
    char dateBuf[DATE_LEN], timeBuf[TIME_LEN];
    // get the GMT time in time_t format
    time(&tloc);

    if (reverse) {
        // write to the date area.
        sprintf(dateBuf, "Izzy, by Roger Allen");
        xs_wprintf(g_date_area_widget, "%s", dateBuf);
    }
    else {
        // get the date into dateBuf, and write it to the date area.
        strftime(dateBuf, (int)DATE_LEN, "%A, %B %d", localtime(&tloc));
        xs_wprintf(g_date_area_widget, "%s", dateBuf);
    }
    // get the time into timeBuf, and write it to the time area.
    strftime(timeBuf, (int)DATE_LEN, "%I%M", localtime(&tloc));
    DrawTime(timeBuf, reverse);

    return tloc;
}

// ----------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int ac;
    char **av;
    static XtActionsRec newActions[] = {
        {(String) "IMouseDown", (XtActionProc)IMouseDown},
        {(String) "IMouseUp", (XtActionProc)IMouseUp}};

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
            printf("Usage suggestion:\n  izzy -fg red -bg black &\n");
            exit(0);
            break;
        }
        ac--;
        av++;
    }

    g_toplevel_widget = XtInitialize(argv[0], "Izzy", NULL, 0, &argc, argv);

    XtAddActions(newActions, XtNumber(newActions));

    InitWidgets();
    // add the timeout with an initial delay
    XtAddTimeOut(500, (XtTimerCallbackProc)TimeCallback, (XtPointer)NULL);
    XtRealizeWidget(g_toplevel_widget);
    InitGC();

    // loop forever
    XtMainLoop();
}
