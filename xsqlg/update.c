#include "../xstr/String.h"
#include <stdio.h>
#include <stdlib.h>
#include "grm.h"
#include <string.h>
#include <time.h>
#include "../time/xtime.h"
#include "update.h"

//UPDATE user(username , status) SET ('hajimi' , '0') WHERE id = '2001';
int UPDATE_exe(TABLE_LIST_NODE *head,TOKENSB *tokensb , int curr){
    String *table_name = create_string("");
    int is_over_update_field_left = 0;
    int is_over_update_field_right = 0;
    int is_over_update_set = 0;
    load_container_t *update_fields = create_loadContainerT();
    load_container_t *set_datas = create_loadContainerT();
    int *update_field_table_indexs;
    TABLE_LIST_NODE *target_table;
    update_where_t *updateWhereT;
    int *datalineArray_count = (int*) malloc(sizeof(int));
    *datalineArray_count = 0;
    PUField_p pufield_p;
            pufield_p.tokensb = tokensb , pufield_p.target_table = target_table 
            , pufield_p.update_fields = update_fields,pufield_p
            .update_field_table_indexs = update_field_table_indexs
            ,pufield_p.set_datas = set_datas , pufield_p.updateWhereT = updateWhereT 
            , pufield_p.datalineArray_count = datalineArray_count;
    for(int i = 0 ; i < tokensb->count ; i++){
        if(!is_over_update_field_left && !IS_CONTAIN_KEYS(tokensb->tokens[i]->str)){
            combine_tail(table_name , tokensb->tokens[i]->str);
            target_table = get_TABLE_LIST_NODE(head , table_name->str);
            continue;
        }else if (!is_over_update_field_left && compare(tokensb->tokens[i] , "(")){
            is_over_update_field_left = 1;
            continue;
        }else if(is_over_update_field_left&&!is_over_update_field_right){
            pufield_p.curr = i;
            i = parse_update_field(pufield_p);
            continue;
        }else if(is_over_update_field_left && is_over_update_field_right 
            && compare(tokensb->tokens[i] , "SET")){
                pufield_p.curr = i;
            i = parse_update_set(pufield_p);
            is_over_update_set = 1;
            continue;
        }else if(is_over_update_set&&compare(tokensb->tokens[i] , "WHERE")){
            i = parse_update_where(pufield_p); // ; i - 1
            continue;
        }else if(compare(tokensb->tokens[i] , ";")){
            update_data_core(pufield_p);
            return i - 1;
        }
    }
}

//UPDATE user(username , status) SET ('hajimi' , '0') WHERE id = '2001';
void update_data_core(PUField_p pufield_p){
    DATALINE_ARRAY *datalineArray = select_match_dataline_array(pufield_p);
    for (int i = 0; i < *pufield_p.datalineArray_count; i++){
        for (int j = 0; j < pufield_p.updateWhereT->auth_field_count; j++){
            copy_string(pufield_p.updateWhereT->auth_data_count[j] ,
                 datalineArray->datalines[i]->DATA[pufield_p.updateWhereT->field_indexs[j]]);
        } 
    }
}

DATALINE_ARRAY *select_match_dataline_array(PUField_p pufield_p){
    DATALINE_ARRAY *datalineArray = create_DATALINE_ARRAY(pufield_p.target_table);
    DATALINE_NODE *temp = pufield_p.target_table->table->dataline_head;
    while (temp != NULL){
        for(int i = 0 ; i < pufield_p.target_table->table->length ; i++){
            if(!compare(temp->dataline->DATA[pufield_p.updateWhereT->auth_indexs_count[i]]
                 , pufield_p.updateWhereT->auth_data_count[i]))break;
            if(i == pufield_p.target_table->table->length - 1 
                && compare(temp->dataline->DATA[pufield_p.updateWhereT->auth_indexs_count[i]]
                 , pufield_p.updateWhereT->auth_data_count[i])){
                if(*(pufield_p.datalineArray_count) == datalineArray->count)
                    extend_DATALINE_ARRAY(pufield_p.target_table->table , datalineArray);
                datalineArray->datalines[*(pufield_p.datalineArray_count)++] = temp->dataline;
                continue;
            }
        }
        temp = temp->next;
    }
    return datalineArray;
}

int parse_update_field(PUField_p pufield_p){
    String *temp_field_name = create_string("");
    for(int i = pufield_p.curr; i < pufield_p.tokensb->count ; i++){
        if(!compare(pufield_p.tokensb->tokens[i] , " ")&&!compare(pufield_p.tokensb->tokens[i] , ",")){ //field_name
            combine_tail(temp_field_name , pufield_p.tokensb->tokens[i]);
            continue;
        }else if(compare(pufield_p.tokensb->tokens[i] , ",")){
            combine_tail(pufield_p.update_fields->data[*pufield_p.update_fields->auth_capable++] 
            , pufield_p.tokensb->tokens[i]->str);
            delete_all(temp_field_name);
            continue;
        }else if(compare(pufield_p.tokensb->tokens[i] , ")")){
            parse_update_auth_field_indexs(pufield_p);
            return i - 1;   
        }
    }
}

//SET ('10' , '40')
int parse_update_set(PUField_p pufield_p){
    int is_over_single_quote = 0;
    int is_over_set_left = 0;
    int is_over_set_right = 0;
    String *temp_data = create_string("");
    for (int i = pufield_p.curr ; i < pufield_p.tokensb->count ; i++){
        if(!is_over_set_left&&!is_over_set_right&&compare(pufield_p.tokensb->tokens[i] , "(")){
            is_over_set_left = 1;
            continue;
        }else if(is_over_set_left&&!is_over_single_quote&&!is_over_set_right){
            if(is_over_single_quote){
                if (!compare(temp_data , ""))
                    copy_string(temp_data , pufield_p.set_datas->data[*pufield_p.set_datas->auth_capable++]);
                delete_all(temp_data);
                is_over_single_quote = 0;
            }
            is_over_single_quote = 1;
            continue;
        }else if(is_over_single_quote&&!compare(pufield_p.tokensb->tokens[i] , " ")){
            combine_tail(temp_data , pufield_p.tokensb->tokens[i]);
            continue;
        }else if(is_over_set_left&&!is_over_set_right&&compare(pufield_p.tokensb->tokens[i] , ")")){
            return i - 1;
        }
    }
}

// WHERE id = '2001' AND status = '1';
int parse_update_where(PUField_p pufield_p){
    int is_over_single_quote = 0;
    String *temp_token = create_string("");
    for (int i = pufield_p.curr ; i < pufield_p.tokensb->count ; i++){
        if(!is_over_single_quote&&!compare(pufield_p.tokensb->tokens[i] , " ")){
            copy_string(pufield_p.tokensb->tokens[i] 
                , pufield_p.updateWhereT->field[(*pufield_p.updateWhereT->auth_field_count)++]);
            continue;
        }else if(compare(pufield_p.tokensb->tokens[i] , "\'")){
            if(is_over_single_quote){
                copy_string(pufield_p.tokensb->tokens[i] 
                    , pufield_p.updateWhereT->data[(*pufield_p.updateWhereT->auth_data_count)++]);
                delete_all(temp_token);
                is_over_single_quote = 0;
            }else is_over_single_quote = 1;
            continue;
        }else if(is_over_single_quote) {
            combine_tail(temp_token , pufield_p.tokensb->tokens[i]->str);
            continue;
        }else if(compare(pufield_p.tokensb->tokens[i] , "AND")
        ||compare(pufield_p.tokensb->tokens[i] , "OR")){
            copy_string(pufield_p.tokensb->tokens[i] 
                ,pufield_p.updateWhereT->logic[(*pufield_p.updateWhereT->auth_logic_count)++]);
            continue;
        }else if(compare(pufield_p.tokensb->tokens[i] , ";")){
            return i - 1;
        }
    }
    
}

void parse_update_auth_field_indexs(PUField_p pufield_p){
    TABLE *target_table = pufield_p.target_table;
    int *auth_field_indexs = (int*)malloc((*pufield_p.update_fields->auth_capable) * sizeof(int));
    for(int i = 0; i < *pufield_p.update_fields->auth_capable ; i++){
        for(int j = 0 ; j < target_table->length ; j++){
            if(compare(target_table->FIELD[j] , pufield_p.update_fields->data[i]))
                auth_field_indexs[i] = j;
        }
    }
    pufield_p.update_field_table_indexs = auth_field_indexs;
}

load_container_t *create_loadContainerT(){
    load_container_t *load_container = (load_container_t*)malloc(sizeof(load_container_t));
    load_container->data = (String**)malloc(10 * sizeof(String*));
    for(int i = 0; i < 10 ; i++)
        load_container->data[i] = create_string("");
    load_container->count = 10;
    load_container->auth_capable = (int*) malloc(sizeof(int));
    *load_container->auth_capable = 0;
    return load_container;
}

load_container_t *extend_updateFieldT(load_container_t *old){
    int new_count = old->count * 2;
    String** new_load_data = (String**)malloc(new_count * sizeof(String*));
    for(int i = 0 ; i < old->count ; i++)
        copy_string(old->data[i] , new_load_data[i]);
    for(int i = old->count ; i < new_count ; i++)
        new_load_data[i] = create_string("");
    for(int i = 0 ; i < old->count ; i++)
        string_free(old->data[i]);
    free(old->data);
    old->data = new_load_data;
    return old;
}

void free_updateFieldT(load_container_t * load_container){
    for(int i = 0 ; i < load_container->count ; i++)
        string_free(load_container->data[i]);
    free(load_container->data);
    free(load_container->auth_capable);
    free(load_container);
    return;
}

update_where_t *create_updateWhereT() {
    update_where_t *updateWhereT = (update_where_t*)malloc(sizeof(update_where_t));
    updateWhereT->field = (String**)malloc(10 * sizeof(String*));
    updateWhereT->data = (String**)malloc(10 * sizeof(String*));
    updateWhereT->logic = (String**)malloc(10 * sizeof(String*));
    updateWhereT->field_indexs = (int *)malloc(10 * sizeof(int));
    updateWhereT->auth_logic_count = (int*)malloc(sizeof(int));
    updateWhereT->auth_field_count = (int*)malloc(sizeof(int));
    updateWhereT->auth_data_count = (int*)malloc(sizeof(int));
    updateWhereT->auth_indexs_count = (int*)malloc(sizeof(int));
    
    for (int i = 0; i < 10; i++) {
        updateWhereT->field[i] = create_string("");
        updateWhereT->data[i] = create_string("");
        updateWhereT->logic[i] = create_string(""); 
    }
    for (int i = 0; i < 10; i++) {
        updateWhereT->field_indexs[i] = -1;
    }
    
    *updateWhereT->auth_logic_count = 0; 
    *updateWhereT->auth_field_count = 0;
    *updateWhereT->auth_data_count = 0;
    *updateWhereT->auth_indexs_count = 0;
    updateWhereT->count = 0;
    return updateWhereT;
}

update_where_t *extend_updateWhereT(update_where_t *old) {
    if (old == NULL) return NULL;
    int new_count = old->count * 2;
    if (new_count == 0) new_count = 10;
    
    String **new_field = (String**)malloc(new_count * sizeof(String*));
    String **new_data = (String**)malloc(new_count * sizeof(String*));
    String **new_logic = (String**)malloc(new_count * sizeof(String*)); 
    int *new_indexs = (int*)malloc(new_count * sizeof(int));
    
    if (!new_field || !new_data || !new_logic || !new_indexs) {
        free(new_field); free(new_data); free(new_logic); free(new_indexs);
        return NULL;
    }
    
    for (int i = 0; i < old->count; i++) {
        new_field[i] = create_string(old->field[i]->str);
        new_data[i] = create_string(old->data[i]->str);
        new_logic[i] = create_string(old->logic[i]->str); 
        new_indexs[i] = old->field_indexs[i];
    }
    for (int i = old->count; i < new_count; i++) {
        new_field[i] = create_string("");
        new_data[i] = create_string("");
        new_logic[i] = create_string("");
        new_indexs[i] = -1;
    }
    
    // 释放旧数据
    for (int i = 0; i < old->count; i++) {
        string_free(old->field[i]);
        string_free(old->data[i]);
        string_free(old->logic[i]);
    }
    free(old->field);
    free(old->data);
    free(old->logic);
    free(old->field_indexs);
    
    old->field = new_field;
    old->data = new_data;
    old->logic = new_logic;
    old->field_indexs = new_indexs;
    old->count = new_count;
    return old;
}

void free_updateWhereT(update_where_t *ptr) {
    if (ptr == NULL) return;
    for (int i = 0; i < ptr->count; i++) {
        if (ptr->field[i]) string_free(ptr->field[i]);
        if (ptr->data[i]) string_free(ptr->data[i]);
        if (ptr->logic[i]) string_free(ptr->logic[i]);
    }
    free(ptr->field);
    free(ptr->data);
    free(ptr->logic); 
    free(ptr->field_indexs);
    free(ptr->auth_logic_count); 
    free(ptr->auth_field_count);
    free(ptr->auth_data_count);
    free(ptr->auth_indexs_count);
    free(ptr);
}