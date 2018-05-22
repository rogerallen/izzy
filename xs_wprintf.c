/************************************************************
 * xs_wprintf: fprintf-like function for XmLabel widgets
 ***********************************************************/
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/Label.h>
#include <stdarg.h>
#include <stdio.h>
#include "xs_wprintf.h"

void xs_wprintf(Widget w, char *format, ...)
{
   va_list	args;
   char		str[1000];
   Arg		wargs[10];
   XmString	xmstr;
   /*
    * init the var len args list
    */
   va_start(args, format);
   /*
    * print it
    */
   vsprintf(str, format, args);
   xmstr = XmStringLtoRCreate(str, XmSTRING_DEFAULT_CHARSET);
   XtSetArg(wargs[0], XmNlabelString, xmstr);
   XtSetValues(w, wargs, 1);

   va_end(args);
}
