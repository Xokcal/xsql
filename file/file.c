//
// Created by 26432 on 2026/8/30.
//
#include <stdio.h>
#include <stdlib.h>
#include "../xstr/String.h"
#include "../log/xlog.h"

String *open_file(char* URL){
    FILE *file = fopen(URL , "r");
    if(file == NULL){
        return NULL;
    }
    String *tokens = create_string("");
    char buf[2];
    while(fgets(buf , sizeof(buf) , file))
        combine_tail_char(tokens , buf[0]);
    fclose(file);
    return tokens;
}

void print_open(char *URL , char *content){
    FILE *file = fopen(URL , "w");
    if(!file){
        log_error_string("fopen error!");
        return;
    }
    fprintf(file , "%s" , content);
    fclose(file);
    return;
}

FILE *get_file_w(char *URL){
    FILE *file = fopen(URL , "w");
    if(!file){
        log_error_string("fopen error!");
        return NULL;
    }
    return file;
}