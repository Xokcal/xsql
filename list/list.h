//
// Created by 26432 on 2026/8/30.
//

#ifndef XOKSQLC语言版_LIST_H
#define XOKSQLC语言版_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include "../xsqlg/grm.h"

typedef struct DATALINE{
    String **DATA;
}DATALINE;

typedef struct DATALINE_NODE{
    DATALINE *dataline;
    struct DATALINE_NODE *next;
}DATALINE_NODE;

typedef struct TABLE{
    String *NAME;
    String **FIELD;
    int length;
    DATALINE_NODE *dataline_head;
}TABLE;

typedef struct TABLE_LIST_NODE{
    TABLE *table;
    struct TABLE_LIST_NODE *next;
}TABLE_LIST_NODE;

/*typedef struct Node{
    DATALINE *dataline;
    struct Node *next;
}Node;

Node *creat_DATALINE_list(DATALINE *dataline);

void list_DATALINE_add(Node *head, DATALINE *dataline);
*/
TABLE_LIST_NODE *create_TABLE_LIST_NODE(String *name , String** field , int length);
void add_TABLE_LIST_NODE(TABLE_LIST_NODE *head , TABLE_LIST_NODE *new);
TABLE_LIST_NODE *get_TABLE_LIST_NODE(TABLE_LIST_NODE *head , char *name);
// dataline list
DATALINE_NODE *create_DATALINE_NODE(String **data , int *indexs , int count , int field_count);
void add_DATALINE_NODE(DATALINE_NODE *head , DATALINE_NODE *new_node);
DATALINE_NODE *get_DATALINE_NODE(DATALINE_NODE *head , char *primary_key);

#endif //XOKSQLC语言版_LIST_H
