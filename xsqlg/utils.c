#include <stdio.h>
#include "../xstr/String.h"
#include <stdlib.h>
#include "utils.h"

XsqlWhereCondi*
create_XsqlWhereCondi()
{
    XsqlWhereCondi *xsqlWhereCondi = (XsqlWhereCondi *) malloc(sizeof(XsqlWhereCondi));
    xsqlWhereCondi->field = (String **) malloc(10 * sizeof(String *));
    xsqlWhereCondi->data = (String **) malloc(10 * sizeof(String *));
    xsqlWhereCondi->logic = (String **) malloc(10 * sizeof(String *));
    xsqlWhereCondi->field_match_indexs = (int*) malloc(10 * sizeof(int));
    xsqlWhereCondi->field_count = 0;
    xsqlWhereCondi->data_count = 0;
    xsqlWhereCondi->logic_count = 0;
    xsqlWhereCondi->index_count = 0;
    xsqlWhereCondi->size = 10;
    for(int i = 0; i < 10 ; i++)
        xsqlWhereCondi->field[i] = create_string("");
    for(int i = 0; i < 10 ; i++)
        xsqlWhereCondi->data[i] = create_string("");
    for(int i = 0; i < 10 ; i++)
        xsqlWhereCondi->logic[i] = create_string("");
    return xsqlWhereCondi;
}

XsqlWhereCondi*
extend_XsqlWhereCondi(XsqlWhereCondi *old)
{
    if (old == NULL) return NULL;

    int new_size = old->size * 2;
    String **new_field = (String**)malloc(new_size * sizeof(String*));
    for (int i = 0; i < old->size; i++) {
        new_field[i] = old->field[i]; 
    }
    for (int i = old->size; i < new_size; i++) {
        new_field[i] = create_string(""); 
    }
    free(old->field); 
    old->field = new_field;
    String **new_data = (String**)malloc(new_size * sizeof(String*));
    for (int i = 0; i < old->size; i++) {
        new_data[i] = old->data[i];
    }
    for (int i = old->size; i < new_size; i++) {
        new_data[i] = create_string("");
    }
    free(old->data);
    old->data = new_data;
    String **new_logic = (String**)malloc(new_size * sizeof(String*));
    for (int i = 0; i < old->size; i++) {
        new_logic[i] = old->logic[i];
    }
    for (int i = old->size; i < new_size; i++) {
        new_logic[i] = create_string("");
    }
    free(old->logic);
    old->logic = new_logic;
    int *new_indexs = (int*)malloc(new_size * sizeof(int));
    for (int i = 0; i < old->size; i++) {
        new_indexs[i] = old->field_match_indexs[i];
    }
    for (int i = old->size; i < new_size; i++) {
        new_indexs[i] = -1;
    }
    free(old->field_match_indexs);
    old->field_match_indexs = new_indexs;
    old->size = new_size;
    return old;
}

void
free_XsqlWhereCondi(XsqlWhereCondi *xsqlWhereCondi)
{
    if (xsqlWhereCondi == NULL) return;
    for (int i = 0; i < xsqlWhereCondi->size; i++) {
        if (xsqlWhereCondi->field[i] != NULL) {
            string_free(xsqlWhereCondi->field[i]);
        }
    }
    for (int i = 0; i < xsqlWhereCondi->size; i++) {
        if (xsqlWhereCondi->data[i] != NULL) {
            string_free(xsqlWhereCondi->data[i]);
        }
    }
    for (int i = 0; i < xsqlWhereCondi->size; i++) {
        if (xsqlWhereCondi->logic[i] != NULL) {
            string_free(xsqlWhereCondi->logic[i]);
        }
    }
    free(xsqlWhereCondi->field);
    free(xsqlWhereCondi->data);
    free(xsqlWhereCondi->logic);
    free(xsqlWhereCondi->field_match_indexs);
    free(xsqlWhereCondi);
}