//
// Created by 26432 on 2026/8/30.
//
#include <stdio.h>
#include <stdlib.h>
#include "../xstr/String.h"

String *open_file(char* URL){
    FILE *file = fopen(URL , "r");
    if(file == NULL){
        return NULL;
    }
    String *tokens = create_string("");
    char buf[2];
    while(fgets(buf , sizeof(buf) , file))
        combine_tail_char(tokens , buf[0]);
    return tokens;
}