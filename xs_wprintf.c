/************************************************************
 * xs_wprintf: fprintf-like function for XmLabel widgets
 ***********************************************************/
#include "xs_wprintf.h"

void xs_wprintf(va_alist) 
   va_dcl
{
   Widget	w;
   char		*format;
   va_list	args;
   char		str[1000];
   Arg		wargs[10];
   XmString	xmstr;
   /*
    * init the var len args list
    */
   va_start(args);
   /*
    * get dest widget, make sure a XmLabel type
    */
   w = va_arg(args, Widget);
   if(!XtIsSubclass(w, xmLabelWidgetClass))
      XtError("xs_wprintf() requires a Label Widget");
   /*
    * get format
    */
   format = va_arg(args, char *);
   /*  
    * print it
    */
   vsprintf(str, format, args);
   xmstr = XmStringLtoRCreate(str, XmSTRING_DEFAULT_CHARSET);
   XtSetArg(wargs[0], XmNlabelString, xmstr);
   XtSetValues(w, wargs, 1);

   va_end(args);
}


