//
// Created by 26432 on 2026/8/30.
//

#ifndef XOKSQLC语言版_FILE_H
#define XOKSQLC语言版_FILE_H

#include <stdio.h>
#include "../xstr/String.h"
#include "../log/xlog.h"
#include "../list/list.h"

typedef struct DATALINE DATALINE;
typedef struct TABLE_LIST_NODE TABLE_LIST_NODE;
typedef struct table_db_t table_db_t;

String *open_file(char* URL);
void print_open(char *URL , char *content);
FILE *get_file_w(char *URL);
void save_snapPhoto(char *URL , TABLE_LIST_NODE *head);
TABLE_LIST_NODE *get_snapPhoto(char *URL);

#endif //XOKSQLC语言版_FILE_H
