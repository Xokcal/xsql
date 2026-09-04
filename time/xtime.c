//
// Created by 26432 on 2026/9/3.
//

#include "../xstr/String.h"
#include <stdio.h>
#include <stdlib.h>
#include "../xsqlg/grm.h"
#include <string.h>
#include <time.h>

String *current_time_format(){
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char buf[70];
    strftime(buf , sizeof(buf) , "%Y-%m-%d %H:%M:%S" , t);
    String *timef = create_string(buf);
    return timef;
}

double run_time_diff(clock_t start , clock_t end){
    return (double)(end - start) / CLOCKS_PER_SEC * 1000.0;
}
