#ifndef DBT_H
#define DBT_H
#include "../xstr/String.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../file/file.h"

typedef struct table_db_t{
    String **table_db_str;
    int count;
    int *auth_count;
}table_db_t;

table_db_t *create_tableDbT();

void extend_tableDbT(table_db_t *tableDbT);

void add_tableDbT(table_db_t *tableDbT , String *data);

void free_tableDbT(table_db_t *tableDbT);
#endif