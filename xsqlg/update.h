#ifndef XOKSQLC语言版_UPDATE_H
#define XOKSQLC语言版_UPDATE_H

#include "../xstr/String.h"
#include <stdio.h>
#include <stdlib.h>
#include "grm.h"
#include <string.h>
#include <time.h>
#include "list/list.h"
#include "../time/xtime.h"

typedef struct load_container_t{
    String **data;
    int count;
    int *auth_capable;
}load_container_t;

typedef struct update_set_t{
    String **set_datas;
    int count;
    int *auth_count;
}update_set_t;

typedef struct update_where_t{
    int count;
    String **field;
    String **data;
    String **logic;
    int *field_indexs;
    int *auth_logic_count;
    int *auth_field_count;
    int *auth_data_count;
    int *auth_indexs_count;
}update_where_t;



/*----- param_t------*/
typedef struct PUField_p{
    TABLE *target_table;
    TOKENSB *tokensb;
    int curr;
    load_container_t *update_fields;
    load_container_t *set_datas;
    int *update_field_table_indexs;
    update_where_t *updateWhereT;
}PUField_p;

/*-------- exe --------*/
int UPDATE_exe(TABLE_LIST_NODE *head,TOKENSB *tokensb , int curr);
int parse_update_field(PUField_p pufield_p);
void parse_update_auth_field_indexs(PUField_p pufield_p);
int parse_update_set(PUField_p pufield_p);
/*---- update_field_t -------*/
load_container_t *create_updateFieldT();
load_container_t *extend_updateFieldT(load_container_t *old);
void free_updateFieldT(load_container_t * load_container);
/*-------update_where_t--------- */
update_where_t *create_updateWhereT();
update_where_t *extend_updateWhereT(update_where_t *old);
void free_updateWhereT(update_where_t *ptr);

#endif //XOKSQLC语言版_UPDATE_H