//
// Created by 26432 on 2026/8/30.
//
#include "../xsqlg/grm.h"
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

TABLE_LIST_NODE *create_TABLE_LIST_NODE(String *name , String** field , int length){
    TABLE_LIST_NODE *table_list_node = (TABLE_LIST_NODE*)malloc(sizeof(TABLE_LIST_NODE));
    table_list_node->next = NULL;
    table_list_node->table = (TABLE*)malloc(sizeof(TABLE));
    table_list_node->table->FIELD = field;
    table_list_node->table->NAME = name;
    table_list_node->table->length = length;

    table_list_node->table->dataline_head = create_DATALINE_NODE(NULL , 0 , 0 , 0);
    return table_list_node;
}

void add_TABLE_LIST_NODE(TABLE_LIST_NODE *head , TABLE_LIST_NODE *new_node){
    if (new_node == NULL) return;
    if (head == NULL) return;
    TABLE_LIST_NODE *temp = head;
    while (temp->next != NULL) temp = temp->next;
    temp->next = new_node;
    return;
}

TABLE_LIST_NODE *get_TABLE_LIST_NODE(TABLE_LIST_NODE *head , char*name){
    TABLE_LIST_NODE *temp = head;
    while (temp != NULL) {
        if (compare(temp->table->NAME, name) == 1) return temp;
        temp = temp->next;
    }
    return NULL;
}

// dataline list
DATALINE_NODE *create_DATALINE_NODE(String **data , int* indexs , int count , int field_count){
    //printf("[ENTRE CREATE!!]\n");
    DATALINE_NODE *dataline_node = (DATALINE_NODE*)malloc(sizeof(DATALINE_NODE));
    dataline_node->next = NULL;
    dataline_node->dataline = (DATALINE*)malloc(sizeof(DATALINE));
    dataline_node->dataline->DATA = (String**)malloc(field_count * sizeof(String*));

    for (int i = 0; i < field_count; ++i) {
        dataline_node->dataline->DATA[i] = create_string("");
    }
    for (int i = 0; i < count; ++i)
        copy_string(data[i] , dataline_node->dataline->DATA[indexs[i]]);

    //printf("[FIELD_COUNT]%d\n" , field_count);

    for (int i = 0; i < field_count; ++i) {
        if(dataline_node->dataline->DATA[i] == NULL){
            //printf("[DATA]NULL\n");
            continue;
        }
        //printf("[DATA]%s\n" , dataline_node->dataline->DATA[i]->str);
    }
    return dataline_node;
}

void add_DATALINE_NODE(DATALINE_NODE *head , DATALINE_NODE *new_node){
    if(head == NULL){
        head = new_node;
        return;
    }
    DATALINE_NODE *temp = head;
    while (temp->next != NULL)temp = temp->next;
    temp->next = new_node;
    return;
}

DATALINE_NODE *get_DATALINE_NODE(DATALINE_NODE *head , char *primary_key){
    if(head == NULL){ printf("[NULL]\n");return NULL;}
    //printf("[ENTRE GET!!]\n");

    DATALINE_NODE *temp = head->next;
    //printf("[temp]%s\n" , temp->dataline->DATA[0]->str);
    while (temp != NULL){
        if(compare(temp->dataline->DATA[0] , primary_key)){
            //printf("[temp  result]%s\n" , temp->dataline->DATA[3]->str);
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}