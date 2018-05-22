/***********************************************************************
 *
 * izzy.h:  A digital clock for X windows
 *
 **********************************************************************/

/***********************************************************************
 *
 * the digitType.value field is encoded so that each bit stands for a
 * segment in a digit.  Therefore, for each number the following table
 * shows how each digit is encoded.
 *
 * The digit segments are arranged like so:    0
 *                                            1 2
 *                                             3
 *                                            4 5
 *                                             6
 *       Displayed Digit
 *  Seg# 0 1 2 3 4 5 6 7 8 9
 *  ----+----------
 *    0 |x   x x   x x x x x
 *    1 |x       x x x   x x
 *    2 |x x x x x     x x x
 *    3 |    x x x x x   x x
 *    4 |x   x       x   x
 *    5 |x x   x x x x x x x
 *    6 |x   x x   x x   x x
 *
 **********************************************************************/
typedef struct {
	XPoint			origin;
	unsigned int		value;	/* see preceding table */
} digitType;

typedef struct alarm {
   unsigned int type;
   char 	date[10];
   char		time[10];
   char         command[255];
} AlarmType;

#define REPEATING_ALARM 0x80
#define DATE_LEN 40
#define TIME_LEN 5
#define SEG_LENGTH 17
#define SEG_WIDTH 4
#define CLOCK_WIDTH 17*SEG_WIDTH + 4*SEG_LENGTH
#define CLOCK_HEIGHT 11*SEG_WIDTH/2 + 2*SEG_LENGTH
#define DATE_HEIGHT 20
#define ICON_AREA_HEIGHT 0
#define LABEL_HEIGHT 25
#define BUTTON_WIDTH 55
#define DLG_H_SPC 5
#define DLG_V_SPC 5
#define MAX_ALARMS 50
#define INIT_DELAY 500  	/* time for initial delay */
#define ONLY_TODAY                0x00 /* types of alarms */
#define ONLY_MMDDYY               0x01
#define EVERY_HOUR                0x81
#define EVERY_DAY                 0xa0
#define EVERY_MONDAY              0xa1
#define EVERY_TUESDAY             0xa2
#define EVERY_WEDNESDAY           0xa3
#define EVERY_THURSDAY            0xa4
#define EVERY_FRIDAY              0xa5
#define EVERY_SATURDAY            0xa6
#define EVERY_SUNDAY              0xa7
