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
    TABLE *target_table;

    PUField_p pufield_p;
            pufield_p.tokensb = tokensb , pufield_p.target_table = target_table 
            , pufield_p.update_fields = update_fields,pufield_p
            .update_field_table_indexs = update_field_table_indexs
            ,pufield_p.set_datas = set_datas;
    for(int i = 0 ; i < tokensb->count ; i++){
        if(!is_over_update_field_left && !IS_CONTAIN_KEYS(tokensb->tokens[i]->str)){
            combine_tail(table_name , tokensb->tokens[i]->str);
            target_table = (head , table_name->str);
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
            i = parse_update_where();
        }
    }
}

int parse_update_field(PUField_p pufield_p){
    String *temp_field_name = create_string("");
    for(int i = pufield_p.curr; i < pufield_p.tokensb->count ; i++){
        if(!compare(pufield_p.tokensb->tokens[i] , " ")&&!compare(pufield_p.tokensb->tokens[i] , ",")){ //field_name
            combine_tail(temp_field_name , pufield_p.tokensb->tokens[i]);
            continue;
        }else if(compare(pufield_p.tokensb->tokens[i] , ",")){
            combine_tail(pufield_p.update_fields->data[*pufield_p.update_fields->auth_capable++] 
            , pufield_p.tokensb->tokens[i]);
            delete_all(temp_field_name);
            continue;
        }else if(compare(pufield_p.tokensb->tokens[i] , ")")){
            parse_update_auth_field_indexs(pufield_p);
            return i;        
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
            return i;
        }
    }
}

int parse_update_where(PUField_p pufield_p){
    //...
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
        load_container->data = create_string("");
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