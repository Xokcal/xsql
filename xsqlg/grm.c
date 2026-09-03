//
// Created by 26432 on 2026/8/30.
//
#include "../xstr/String.h"
#include <stdio.h>
#include <stdlib.h>
#include "grm.h"
#include <string.h>

#define CHAR_LENGTH(strs) (sizeof(strs) / sizeof(strs[0]))

static int IS_CONTAIN_KEYS(char *str){
    int length = sizeof(sql_keys) / sizeof(sql_keys[0]);
    for (int i = 0; i < length; ++i) {
        if(strcmp(sql_keys[i] , str) == 0)
            return 1;
    }
    return 0;
}

static int IS_CONTAIN_KEYS_PRIMARY(char *str){
    int length = sizeof(sql_keys_primary) / sizeof(sql_keys_primary[0]);
    for (int i = 0; i < length; ++i) {
        if(strcmp(sql_keys_primary[i] , str) == 0)
            return 1;
    }
    return 0;
}

static int IS_PRIMARY_KEY(char *str){
    int length = sizeof(primary_keys) / sizeof(primary_keys[0]);
    for (int i = 0; i < length; ++i) {
        if(strcmp(primary_keys[i] , str) == 0){
            return 1;
        }
    }
    return 0;
}

TOKENSB *tokens_parse(String *origin){
    String **tokens = (String**)malloc(1000 * sizeof(String*));
    String *temp = create_string("");
    String *temp_test = create_string("");
    int count = 0;
    for (int i = 0; i < origin->length; ++i) {
        combine_tail_char(temp , origin->str[i]);
        if(IS_CONTAIN_KEYS(temp->str) == 1){
            tokens[count] = create_string("");
            combine_tail(tokens[count++] , temp->str);
            delete_all(temp);
            delete_all(temp_test);
            continue;
        } else {
            combine_tail_char(temp_test , origin->str[i + 1]);
            if(IS_CONTAIN_KEYS(temp_test->str)){
                tokens[count] = create_string("");
                combine_tail(tokens[count++] , temp->str);
                delete_all(temp);
            }
            delete_all(temp_test);
            continue;
        }
    }
    string_free(temp);
    string_free(temp_test);
    TOKENSB *tokensb = (TOKENSB*)malloc(sizeof(TOKENSB));
    tokensb->tokens = tokens;
    tokensb->count = count;
    return tokensb;
}

int CREATE_exe(TABLE_LIST_NODE *head,TOKENSB *tokensb , int curr){
    //printf("[ENTRE_CREATE_EXE]\n");
    for (int i = curr; i < tokensb->count; ++i) {
        if(IS_CONTAIN_KEYS(tokensb->tokens[i]->str)&&!compare(tokensb->tokens[i] , " ")){
            //printf("[ENTRE   TABLE   KEY]%s\n" , tokensb->tokens[i]->str);
            if(compare(tokensb->tokens[i] , "TABLE")){
                int is_table_name = 0;
                int field_count = 0;
                String *temp_store_field_name = create_string("");
                String *TABLE_NAME = create_string("");
                int is_over_field_name = 0;
                for (int j = i; j < tokensb->count; ++j) {
                    //printf("[ENTRE    FOR    TWE]%s\n" , tokensb->tokens[j]->str);
                    if(is_table_name == 0 && !IS_CONTAIN_KEYS(tokensb->tokens[j]->str)
                    && !compare(tokensb->tokens[j] , " ")){ // name
                        //printf("[TABLE     NAME]%s\n" , tokensb->tokens[j]->str);
                        combine_tail(TABLE_NAME , tokensb->tokens[j]->str);
                        is_table_name = 1;
                        continue;
                    }if (!IS_CONTAIN_KEYS(tokensb->tokens[j]->str)
                    &&is_over_field_name == 0&&!compare(tokensb->tokens[j] , " ")){ // field name
                        //printf("[FIELD   NAME    FIELD]%s\n" , tokensb->tokens[j]->str);
                        combine_tail(temp_store_field_name , tokensb->tokens[j]->str);
                        combine_tail(temp_store_field_name , " ");
                        field_count++;
                        is_over_field_name = 1;
                        continue;
                    }if (is_over_field_name&&IS_CONTAIN_KEYS(tokensb->tokens[j]->str)
                    && compare(tokensb->tokens[j] , ",")){ // ;
                        is_over_field_name = 0;
                        continue;
                    }if(IS_CONTAIN_KEYS(tokensb->tokens[j]->str)
                    &&compare(tokensb->tokens[j] , ";")){
                        //printf("[J]%d\n" , j);
                        //printf("[J   TOKEN]%s\n" , tokensb->tokens[j + 3]->str);
                        Splitor *splitor = split(temp_store_field_name , " ");
                        TABLE_LIST_NODE *new = create_TABLE_LIST_NODE(TABLE_NAME , splitor->splits , splitor->count);
                        add_TABLE_LIST_NODE(head , new);
                        for (int k = j; k < tokensb->count; ++k) {
                            if (IS_CONTAIN_KEYS_PRIMARY(tokensb->tokens[k]->str)){
                                //printf("[K]%s\n" , tokensb->tokens[k]->str);
                                return k - 1;
                            }
                        }
                        return j - 1;
                    }
                }
            } else {
                //...
            }
        }
    }
}

static int select_field_place(TABLE_LIST_NODE *table , int length , char *field_name){
    for (int i = 0; i < length; ++i) {
        if(compare(table->table->FIELD[i] , field_name))return i;
    }
    return 0;
}

// insert student(id , age , name) : ('12' , '21' , 'XiaoLi');

int INSERT_exe(TABLE_LIST_NODE *head,TOKENSB *tokensb , int curr){
    printf("[ENTRE_K][INSERT]\n");
    TABLE_LIST_NODE *TARGET_TABLE_NODE;
    int is_over_table_field_left = 0;
    int is_over_table_field_right = 0;
    String *temp_field = create_string("");
    int return_key_index = 0;
    int is_end = 0;
    String *temp_data = create_string("");
    for (int i = curr; i < tokensb->count; ++i) {
        printf("[I]%s\n" , tokensb->tokens[i]->str);
        if(!IS_CONTAIN_KEYS(tokensb->tokens[i]->str)&&!compare(tokensb->tokens[i] , " ")
        &&is_over_table_field_left == 0){ // table name
            TARGET_TABLE_NODE = get_TABLE_LIST_NODE(head , tokensb->tokens[i]->str);
            printf("[insert  table  name ]:%s\n" , TARGET_TABLE_NODE->table->NAME->str);
            printf("[insert  parse  table  name  ]:%s\n" , tokensb->tokens[i]->str);
            continue;
        }else if(compare(tokensb->tokens[i] , "(")&& IS_CONTAIN_KEYS(tokensb->tokens[i]->str)
        &&is_over_table_field_left == 0&&is_over_table_field_right == 0){ // is field "("
            printf("[is field \"(\"]\n");
            is_over_table_field_left = 1;
            printf("[is_over_left]%d\n" , is_over_table_field_left);
            continue;
        } else if(is_over_table_field_left == 1&&is_over_table_field_right == 0){ // over "("
            printf("[over \"(\"]\n");
            // store field in table's index --> TABLE.FIELD[?];  example: [1 , 2 , 3];
            int *field_place_index = (int*)malloc(TARGET_TABLE_NODE->table->length * sizeof(int));
            int field_place_count = 0;

            for (int j = i; j < tokensb->count; ++j) {
                printf("[J]%s\n" , tokensb->tokens[j]->str);
                if (!IS_CONTAIN_KEYS(tokensb->tokens[j]->str)&&!compare(tokensb->tokens[j] , " ")
                && !compare(tokensb->tokens[j] , ",")){ //
                    printf("[J   FIELD   NAME]%s\n" , tokensb->tokens[j]->str);
                    combine_tail(temp_field , tokensb->tokens[j]->str);
                    combine_tail(temp_field , " ");
                    printf("[TEMP_FIELD]%s\n" , temp_field->str);
                    int curr_field_place = select_field_place(
                            TARGET_TABLE_NODE
                            , TARGET_TABLE_NODE->table->length
                            , tokensb->tokens[j]->str
                            );
                    field_place_index[field_place_count++] = curr_field_place;
                    for (int k = 0; k < field_place_count; ++k) {
                        printf("[field_place_index]%d\n" , field_place_index[k]);
                    }
                    continue;
                } else if (is_over_table_field_left == 1&&is_over_table_field_right == 0
                && IS_CONTAIN_KEYS(tokensb->tokens[j]->str)&& compare(tokensb->tokens[j] , ")")){ // ")"
                    printf("[end \")\"]\n");
                    is_over_table_field_right = 1;
                    continue;
                } else if(IS_CONTAIN_KEYS(tokensb->tokens[j]->str)&& compare(tokensb->tokens[j] , ":")){
                    // ":"
                    // insert student(id , age , name) : ('12' , '21' , 'XiaoLi');
                    printf("[is \":\"]\n");
                    int is_over_insert_left = 0;
                    int is_over_insert_right = 0;
                    int is_open_single_quote = 0;
                    for (int k = j; k < tokensb->count; ++k) {
                        if (IS_CONTAIN_KEYS(tokensb->tokens[k]->str)&& compare(tokensb->tokens[k] , "(")
                        &&is_over_insert_left == 0&&is_over_insert_right == 0){
                            // "("
                            printf("[:  \"(\"]\n");
                            is_over_insert_left = 1;
                            continue;
                        } else if(IS_CONTAIN_KEYS(tokensb->tokens[k]->str)&& compare(tokensb->tokens[k] , "\'")){
                            printf("[:  \"\'\"]%d\n" , is_open_single_quote);
                            if(is_open_single_quote) { // =1
                                is_open_single_quote = 0;
                            }
                            else is_open_single_quote = 1;
                            continue;
                        } else if (IS_CONTAIN_KEYS(tokensb->tokens[k]->str)&&is_open_single_quote == 0
                        && compare(tokensb->tokens[k] , ")")){
                            printf("[temp   data]%s\n" , temp_data->str);
                            Splitor *splitor = split(temp_data , "^");
                            printf("[DATA_COUNT]%d\n" , splitor->count);
                            DATALINE_NODE *new_node = create_DATALINE_NODE(
                                    splitor->splits
                                    , field_place_index
                                    , field_place_count
                                    , TARGET_TABLE_NODE->table->length
                            );
                            printf("[IS CREATE DATALINE!!!]\n");
                            /*for (int l = 0; l < splitor->count; ++l) {
                                printf("[splitor  str]%s\n" , splitor->splits[l]->str);
                            }*/
                            add_DATALINE_NODE(TARGET_TABLE_NODE->table->dataline_head , new_node);
                            printf("[IS ADD!!!]\n");
                            DATALINE_NODE *data1 = get_DATALINE_NODE(TARGET_TABLE_NODE->table->dataline_head , "12");
                            printf("[DATA1  VALUE]%s\n" , data1->dataline->DATA[0]->str);
                            delete_all(temp_data);
                            for (int l = k; l < tokensb->count; ++l) {
                                printf("[L]%s\n" , tokensb->tokens[l]->str);
                                printf("[LNUM]%d\n" , l);
                                if (compare(tokensb->tokens[l] , ";")){
                                    printf("[ENTRE LLLL!!!]\n");
                                    for (int m = l; m < tokensb->count; ++m) {
                                        if (IS_PRIMARY_KEY(tokensb->tokens[m]->str)){
                                            printf("[MM] = %d ; [M STR] = %s\n" , m , tokensb->tokens[m]->str);
                                            return m - 1;
                                        }
                                    }
                                    return l - 1;
                                }
                            }
                        }
                        else if(is_open_single_quote == 1){
                            combine_tail(temp_data , tokensb->tokens[k]->str);
                            combine_tail(temp_data , "^");
                            continue;
                        } else if(is_open_single_quote == 0&& IS_CONTAIN_KEYS(tokensb->tokens[k]->str)
                        && compare(tokensb->tokens[k] , ";")){
                            is_end = 1;
                            break;
                        }
                    }
                } else if (compare(tokensb->tokens[j] , "->")){
                    // "->"
                }else if (is_end == 1)break;
            }
        } else if(is_end == 1)break;
    }
}

static KEYS keys_to_KEYSTYPE(String *key){
    if(compare(key , "CREATE"))return CREATE_K;
    else if(compare(key , "INSERT"))return INSERT_K;
}

void XSQL_RUN(TOKENSB *tokensb){
    TABLE_LIST_NODE *head = create_TABLE_LIST_NODE(create_string("XSQL") , NULL , 0);
    for (int i = 0; i < tokensb->count; ++i) {
        printf("[1] i = %d ; str = %s\n" , i , tokensb->tokens[i]->str);
        KEYS keys_enum = keys_to_KEYSTYPE(tokensb->tokens[i]);
        switch (keys_enum) {
            case CREATE_K:
                i = CREATE_exe(head , tokensb , i);
                TABLE_LIST_NODE *table_get = get_TABLE_LIST_NODE(head , "student");
                String** table_msg = table_get->table->FIELD;
                printf("[COUNT][TABLE]%d\n" , table_get->table->length);
                for (int j = 0; j < table_get->table->length; ++j)
                    printf("[FOR][STUDENT   TEST   TABLE   FIELD]%s\n" , table_msg[j]->str);
                printf("[55]%s\n" , table_msg[4]->str);
                break;
            case INSERT_K:
                i = INSERT_exe(head , tokensb , i);
                printf("[INSERT EXE SUCCESS!!!]\n\n\n");
                break;
            default:
                continue;

        }
    }
}
