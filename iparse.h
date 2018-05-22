/************************************************************************
 *
 * iparse.h
 *
 * header file for iinit.c
 *
 ***********************************************************************/
#include "izzy.h"

void ParseAlarmFile(void);
void ParseLine(char theLine[], int theAlarmNumber);
void ParseAtTime(int theAlarmNumber);
void ParseCommand(int theAlarmNumber);
