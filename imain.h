/***********************************************************************
 *
 * imain.h
 *
 * header file for imain.c
 *
 **********************************************************************/

void DoTime(XtIntervalId id);
void PrintTime(char *timeBuf, int reverse);
void IMouseUp(Widget w, XEvent *event, String *params, Cardinal *num_params);
void IMouseDown(Widget w, XEvent *event, String *params, Cardinal *num_params);
void DrawDigit(digitType digit, int reverse);
void DrawSegment(XPoint origin, int segNum, int reverse);
void Update(void);
