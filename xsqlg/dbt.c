#include "../xstr/String.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../time/xtime.h"
#include "../log/xlog.h"
#include "dbt.h"

table_db_t *create_tableDbT(){
    table_db_t *tableDbT = (table_db_t *)malloc(sizeof(table_db_t));
    tableDbT->table_db_str = (String**)malloc(10 * sizeof(String*));
    for (int i = 0; i < 10; i++)
        tableDbT->table_db_str[i] = NULL;
    tableDbT->count = 10;
    tableDbT->auth_count = (int *)malloc(sizeof(int));
    *tableDbT->auth_count = 0;
    printf("[count]%d\n" , *tableDbT->auth_count);
    return tableDbT;
}

void extend_tableDbT(table_db_t *tableDbT){
    int new_count = tableDbT->count * 2;
    String **new_string = (String**)malloc(new_count * sizeof(String*));
    for(int i = 0 ; i < tableDbT->count ; i++)
        new_string[i] = create_string(tableDbT->table_db_str[i]->str);
    for(int i = tableDbT->count; i < new_count ; i++)
        new_string[i] = NULL;
    for(int i = 0 ; i< tableDbT->count ; i++)
        string_free(tableDbT->table_db_str[i]);
    free(tableDbT->table_db_str);
    tableDbT->table_db_str = new_string;
}

void add_tableDbT(table_db_t *tableDbT , String *data){
    if((*tableDbT->auth_count) == tableDbT->count)
    extend_tableDbT(tableDbT);
    tableDbT->table_db_str[(*tableDbT->auth_count)++] = create_string(data->str);
}

void free_tableDbT(table_db_t *tableDbT){
    for (int i = 0 ; i < tableDbT->count ; i++)
        string_free(tableDbT->table_db_str[i]);
    free(tableDbT->table_db_str);
    free(tableDbT->auth_count);
}
