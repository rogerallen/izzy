/************************************************************
 * xs_wprintf: fprintf-like function for XmLabel widgets
 ***********************************************************/
#include <stdarg.h>
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/Label.h>

void xs_wprintf(Widget w, char *format, ...);
