//
// Created by 26432 on 2026/9/3.
//

#ifndef XOKSQLC语言版_XTIME_H
#define XOKSQLC语言版_XTIME_H


#include <stdlib.h>
#include <time.h>
#include "../xstr/String.h"

String *current_time_format();
double run_time_diff(clock_t start , clock_t end);


#endif //XOKSQLC语言版_XTIME_H
