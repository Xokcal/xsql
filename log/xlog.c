#include <stdio.h>

void log_info_string( char *statuement){
    printf("\033[1;32mINFO : %s\033[0m\n" , statuement);
}

void log_error_string(char *statuement){
    printf("ERROR : %s\033[0m\n" , statuement);
}