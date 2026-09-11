//
// Created by 26432 on 2026/8/30.
//
#include "../xstr/String.h"
#include <stdio.h>
#include <stdlib.h>
#include "grm.h"
#include <string.h>
#include <time.h>
#include "../time/xtime.h"
#include "update.h"
#include "../file/file.h"
#include "../log/xlog.h"

#define CHAR_LENGTH(strs) (sizeof(strs) / sizeof(strs[0]))
#define LOG(log , statue)(printf("[DEBUG]-{%s} %s\n" , (log) , (statue)))

int IS_CONTAIN_KEYS(char *str){
    int length = sizeof(sql_keys) / sizeof(sql_keys[0]);
    for (int i = 0; i < length; ++i) {
        if(strcmp(sql_keys[i] , str) == 0)
            return 1;
    }
    return 0;
}

int IS_CONTAIN_KEYS_PRIMARY(char *str){
    int length = sizeof(sql_keys_primary) / sizeof(sql_keys_primary[0]);
    for (int i = 0; i < length; ++i) {
        if(strcmp(sql_keys_primary[i] , str) == 0)
            return 1;
    }
    return 0;
}

int IS_PRIMARY_KEY(char *str){
    int length = sizeof(primary_keys) / sizeof(primary_keys[0]);
    for (int i = 0; i < length; ++i) {
        if(strcmp(primary_keys[i] , str) == 0){
            return 1;
        }
    }
    return 0;
}

int *replace_reflect_index_to_all(int *reflect_inedxs , int all_length){
    free(reflect_inedxs);
    reflect_inedxs = (int*)malloc(all_length * sizeof(int ));
    for (int i = 0; i < all_length; ++i)
        reflect_inedxs[i] = i;
    return reflect_inedxs;
}

// ["id" , "name" , "age"]
FIELD_INDEXS *get_field_indexs_by_field(TABLE_LIST_NODE *TARGET , String **fields , int field_effective_count){
    //printf("[get_field_indexs_by_field]\n");
    FIELD_INDEXS *fieldIndex = (FIELD_INDEXS*) malloc(sizeof(FIELD_INDEXS));
    fieldIndex->field_indexs = (int *) malloc(field_effective_count * sizeof(int ));
    int count = 0;
    for (int i = 0; i < field_effective_count; ++i) {
        for (int j = 0; j < TARGET->table->length; ++j) {
            if (compare(TARGET->table->FIELD[j] , fields[i]->str))
                fieldIndex->field_indexs[count++] = j;
        }
    }
    fieldIndex->count = count;
    return fieldIndex;
}

TOKENSB *tokens_parse(String *origin){
    String **tokens = (String**)malloc(2000000 * sizeof(String*));
    String *temp = create_string("");
    String *temp_test = create_string("");
    int count = 0;
    for (int i = 0; i < origin->length; ++i) {
        combine_tail_char(temp , origin->str[i]);
        if(IS_CONTAIN_KEYS(temp->str) == 1){
            tokens[count] = create_string("");
            lowercase(temp);
            combine_tail(tokens[count++] , temp->str);
            delete_all(temp);
            delete_all(temp_test);
            continue;
        } else {
            combine_tail_char(temp_test , origin->str[i + 1]);
            if(IS_CONTAIN_KEYS(temp_test->str)){
                tokens[count] = create_string("");
                lowercase(temp);
                combine_tail(tokens[count++] , temp->str);
                delete_all(temp);
            }
            delete_all(temp_test);
            continue;
        }
    }
    //printf("[count]%d\n" , count);
    string_free(temp);
    string_free(temp_test);
    TOKENSB *tokensb = (TOKENSB*)malloc(sizeof(TOKENSB));
    tokensb->tokens = tokens;
    tokensb->count = count;
    return tokensb;
}

int CREATE_exe(TABLE_LIST_NODE *head,TOKENSB *tokensb , int curr, FILE *table_file , FILE *data_file,table_db_t *tableDbT){
    clock_t start = clock();
    //printf("[ENTRE_CREATE_EXE]\n");
    String *table_name = create_string("");
    String *xsql = create_string("");
    combine_tail(xsql , tokensb->tokens[curr]->str);
    for (int i = curr; i < tokensb->count; ++i) {
        //combine_tail(xsql , tokensb->tokens[i]->str);
        if(IS_CONTAIN_KEYS(tokensb->tokens[i]->str)&&!compare(tokensb->tokens[i] , " ")){

            //printf("[ENTRE   TABLE   KEY]%s\n" , tokensb->tokens[i]->str);
            if(compare(tokensb->tokens[i] , "table")){
                combine_tail(xsql , tokensb->tokens[i]->str);
                int is_table_name = 0;
                int field_count = 0;
                String *temp_store_field_name = create_string("");
                String *TABLE_NAME = create_string("");
                int is_over_field_name = 0;
                for (int j = i + 1; j < tokensb->count; ++j) {

                    //printf("[ENTRE    FOR    TWE]%s\n" , tokensb->tokens[j]->str);
                    if(is_table_name == 0 && !IS_CONTAIN_KEYS(tokensb->tokens[j]->str)
                    && !compare(tokensb->tokens[j] , " ")){ // name
                        //printf("[TABLE     NAME]%s\n" , tokensb->tokens[j]->str);
                        combine_tail(xsql , tokensb->tokens[j]->str);
                        combine_tail(table_name , tokensb->tokens[j]->str);
                        combine_tail(TABLE_NAME , tokensb->tokens[j]->str);
                        is_table_name = 1;
                        TABLE_LIST_NODE *target_table = get_TABLE_LIST_NODE(head , table_name->str);
                        if(target_table != NULL){
                            log_error_string("table is exist.");
                            return j;    
                        }
                        continue;
                    }if(compare(tokensb->tokens[i] , "(")){
                        /*if(!compare(tokensb->tokens[j + 1] , "\n"))
                            combine_tail(xsql , "\n");*/
                        combine_tail(xsql , "\n");continue;
                    }
                    if (!IS_CONTAIN_KEYS(tokensb->tokens[j]->str)
                    &&is_over_field_name == 0&&!compare(tokensb->tokens[j] , " ")){ // field name
                        combine_tail(xsql , tokensb->tokens[j]->str);
                        //printf("[FIELD   NAME    FIELD]%s\n" , tokensb->tokens[j]->str);
                        combine_tail(temp_store_field_name , tokensb->tokens[j]->str);
                        combine_tail(temp_store_field_name , " ");
                        field_count++;
                        is_over_field_name = 1;
                        continue;
                    }if (is_over_field_name&&IS_CONTAIN_KEYS(tokensb->tokens[j]->str)
                    && compare(tokensb->tokens[j] , ",")){ // ;
                        combine_tail(xsql , tokensb->tokens[j]->str);
                        /*if(!compare(tokensb->tokens[j + 1] , "\n")){
                            printf("is OK!!  %s \n" , tokensb->tokens[j]->str);
                            combine_tail(xsql , "\n");
                        }*/
                        is_over_field_name = 0;
                        continue;
                    }if(IS_CONTAIN_KEYS(tokensb->tokens[j]->str)
                    &&compare(tokensb->tokens[j] , ";")){
                        combine_tail(xsql , tokensb->tokens[j]->str);
                        //printf("[J]%d\n" , j);
                        //printf("[J   TOKEN]%s\n" , tokensb->tokens[j + 3]->str);
                        Splitor *splitor = split(temp_store_field_name , " ");
                        TABLE_LIST_NODE *new = create_TABLE_LIST_NODE(TABLE_NAME , splitor->splits , splitor->count);
                        add_TABLE_LIST_NODE(head , new);
                        for (int k = j; k < tokensb->count; ++k) {
                            if (IS_CONTAIN_KEYS_PRIMARY(tokensb->tokens[k]->str)){
                                //printf("[K]%s\n" , tokensb->tokens[k]->str);
                                printf("[%s] xsql server> %s \n"
                                       , current_time_format()->str
                                       , xsql->str);
                                clock_t end = clock();
                                printf("[%s] (create) table \'%s\' execute in %.2f ms is ok ! \n"
                                       , current_time_format()->str
                                       , table_name->str
                                       , run_time_diff(start , end));
                                if(!compare(tokensb->tokens[i + 1] , "\n"))
                                    combine_tail(xsql , "\n\n");
                                add_tableDbT(tableDbT , xsql);
                                //printf("[CREATE END RETURN I]%s %d" , tokensb->tokens[k]->str , k);
                                return k - 1;
                            } else if (k == tokensb->count - 1){
                               // printf("[K]%s\n" , tokensb->tokens[k]->str);
                                printf("[%s] xsql server> %s \n"
                                        , current_time_format()->str
                                        , xsql->str);
                                clock_t end = clock();
                                printf("[%s] (create) table \'%s\' execute in %.2f ms is ok ! \n"
                                        , current_time_format()->str
                                        , table_name->str
                                        , run_time_diff(start , end));
                                if(!compare(tokensb->tokens[i + 1] , "\n"))
                                    combine_tail(xsql , "\n\n");
                                add_tableDbT(tableDbT , xsql);
                                //printf("[CREATE END RETURN I] %d" , k);
                               // printf("[KKKV]%d\n" , k);
                                return k;
                            }
                        }
                        return j - 1;
                    } else {
                        combine_tail(xsql , tokensb->tokens[j]->str);
                    }
                }
            } else {
                //...
            }
        } else{
            combine_tail(xsql , tokensb->tokens[i]->str);
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

int INSERT_exe(TABLE_LIST_NODE *head,TOKENSB *tokensb , int curr , table_db_t *insertDbT){
    //printf("[ENTRE_K][INSERT]\n");
    clock_t start = clock();
    TABLE_LIST_NODE *TARGET_TABLE_NODE;
    int is_over_table_field_left = 0;
    int is_over_table_field_right = 0;
    String *temp_field = create_string("");
    int return_key_index = 0;
    int is_end = 0;
    String *temp_data = create_string("");
    String *table_name = create_string("");
    String *xsql_row = create_string("");
    for(int i = curr ; i < tokensb->count ; i ++){
        if(compare(tokensb->tokens[i] , ";")){
            combine_tail(xsql_row , tokensb->tokens[i]->str);
            combine_tail(xsql_row , "\n");
            break;}
        combine_tail(xsql_row , tokensb->tokens[i]->str);
    }
    add_tableDbT(insertDbT , xsql_row);
    for (int i = curr; i < tokensb->count; ++i) {
        //printf("[I]%s\n" , tokensb->tokens[i]->str);
        if(!IS_CONTAIN_KEYS(tokensb->tokens[i]->str)&&!compare(tokensb->tokens[i] , " ")
        &&is_over_table_field_left == 0){ // table name
            TARGET_TABLE_NODE = get_TABLE_LIST_NODE(head , tokensb->tokens[i]->str);
           //printf("[insert  table  name ]:%s\n" , TARGET_TABLE_NODE->table->NAME->str);
           //printf("[insert  parse  table  name  ]:%s\n" , tokensb->tokens[i]->str);
            combine_tail(table_name , tokensb->tokens[i]->str);
            continue;
        }else if(compare(tokensb->tokens[i] , "(")&& IS_CONTAIN_KEYS(tokensb->tokens[i]->str)
        &&is_over_table_field_left == 0&&is_over_table_field_right == 0){ // is field "("
            //printf("[is field \"(\"]\n");
            is_over_table_field_left = 1;
            //printf("[is_over_left]%d\n" , is_over_table_field_left);
            continue;
        } else if(is_over_table_field_left == 1&&is_over_table_field_right == 0){ // over "("
            //printf("[over \"(\"]\n");
            // store field in table's index --> TABLE.FIELD[?];  example: [1 , 2 , 3];
            int *field_place_index = (int*)malloc(TARGET_TABLE_NODE->table->length * sizeof(int));
            int field_place_count = 0;

            for (int j = i; j < tokensb->count; ++j) {

                //printf("[J]%s\n" , tokensb->tokens[j]->str);
                if (!IS_CONTAIN_KEYS(tokensb->tokens[j]->str)&&!compare(tokensb->tokens[j] , " ")
                && !compare(tokensb->tokens[j] , ",")){ //
                    //printf("[J   FIELD   NAME]%s\n" , tokensb->tokens[j]->str);
                    combine_tail(temp_field , tokensb->tokens[j]->str);
                    combine_tail(temp_field , " ");
                    //printf("[TEMP_FIELD]%s\n" , temp_field->str);
                    int curr_field_place = select_field_place(
                            TARGET_TABLE_NODE
                            , TARGET_TABLE_NODE->table->length
                            , tokensb->tokens[j]->str
                            );
                    field_place_index[field_place_count++] = curr_field_place;
                    /*for (int k = 0; k < field_place_count; ++k) {
                        printf("[field_place_index]%d\n" , field_place_index[k]);
                    }*/
                    continue;
                } else if (is_over_table_field_left == 1&&is_over_table_field_right == 0
                && IS_CONTAIN_KEYS(tokensb->tokens[j]->str)&& compare(tokensb->tokens[j] , ")")){ // ")"
                    //printf("[end \")\"]\n");
                    is_over_table_field_right = 1;
                    continue;
                } else if(IS_CONTAIN_KEYS(tokensb->tokens[j]->str)&& compare(tokensb->tokens[j] , ":")){
                    // ":"
                    //insert student(id , age , name) : ('12' , '21' , 'XiaoLi');
                    //printf("[is \":\"]\n");
                    combine_tail(temp_field , tokensb->tokens[j]->str);
                    int is_over_insert_left = 0;
                    int is_over_insert_right = 0;
                    int is_open_single_quote = 0;
                    for (int k = j; k < tokensb->count; ++k) {
                        if (IS_CONTAIN_KEYS(tokensb->tokens[k]->str) && compare(tokensb->tokens[k] , "(")
                        && is_over_insert_left == 0 && is_over_insert_right == 0){
                            // "("
                            //printf("[:  \"(\"]\n");
                            is_over_insert_left = 1;
                            continue;
                        } else if(IS_CONTAIN_KEYS(tokensb->tokens[k]->str)&& compare(tokensb->tokens[k] , "\'")){
                            //printf("[:  \"\'\"]%d\n" , is_open_single_quote);
                            if(is_open_single_quote) { // =1
                                is_open_single_quote = 0;
                            }
                            else is_open_single_quote = 1;
                            continue;
                        } else if (IS_CONTAIN_KEYS(tokensb->tokens[k]->str)&&is_open_single_quote == 0
                        && compare(tokensb->tokens[k] , ")")){
                            //printf("[temp   data]%s\n" , temp_data->str);
                            Splitor *splitor = split(temp_data , "^");
                            //printf("[DATA_COUNT]%d\n" , splitor->count);
                            DATALINE_NODE *new_node = create_DATALINE_NODE(
                                    splitor->splits
                                    , field_place_index
                                    , field_place_count
                                    , TARGET_TABLE_NODE->table->length
                            );

                            //printf("{DATA[1] v = }  : %s\n" , new_node->dataline->DATA[1]->str);
                            //printf("[IS CREATE DATALINE!!!]\n");
                            /*for (int l = 0; l < splitor->count; ++l) {
                                printf("[splitor  str]%s\n" , splitor->splits[l]->str);
                            }*/
                            //printf("[INSERT] TARGET_TABLE_NODE->table->dataline_head = %p\n", 
                            //TARGET_TABLE_NODE->table->dataline_head);
                            add_DATALINE_NODE(&TARGET_TABLE_NODE->table->dataline_head , new_node);
                            //printf("{%s}\n" , TARGET_TABLE_NODE->table->dataline_head->dataline->DATA[1]->str);
                            //printf("[IS ADD!!!]\n");
                            delete_all(temp_data);
                            for (int l = k; l < tokensb->count; ++l) {
                                //printf("[L]%s\n" , tokensb->tokens[l]->str);
                                //printf("[LNUM]%d\n" , l);
                                if (compare(tokensb->tokens[l] , ";")){
                                    //printf("[ENTRE LLLL!!!]\n");
                                    for (int m = l; m < tokensb->count; ++m) {
                                        if (IS_PRIMARY_KEY(tokensb->tokens[m]->str)){
                                            printf("[TARGET VALUE]%s\n" , TARGET_TABLE_NODE->table->FIELD[0]->str);
                                            printf("[DATALINE VALUE]%s\n" , TARGET_TABLE_NODE->table->dataline_head->dataline->DATA[1]->str);
                                            clock_t end = clock();
                                            printf("[MM] = %d ; [M STR] = %s\n" , m , tokensb->tokens[m]->str);
                                            /*printf("[%s] xsql server> %s\n"
                                                   , current_time_format()->str , xsql_row->str);*/
                                            printf("[%s] (insert) to \'%s\' 1 row execute in %.2f ms is ok ! \n"
                                                   , current_time_format()->str
                                                   , table_name->str
                                                   , run_time_diff(start , end));
                                            //printf("[INSERT END RETURN M] %d AND %s\n" , m , tokensb->tokens[m]->str);
                                            return m - 1;
                                        } else if (m == tokensb->count - 1){
                                            printf("[TARGET VALUE]%s\n" , TARGET_TABLE_NODE->table->FIELD[0]->str);
                                            printf("[DATALINE VALUE]%s\n" , TARGET_TABLE_NODE->table->dataline_head->dataline->DATA[1]->str);
                                            clock_t end = clock();
                                            printf("[MM] = %d ; [M STR] = %s\n" , m , tokensb->tokens[m]->str);
                                            /*printf("[%s] xsql server> %s\n"
                                                   , current_time_format()->str , xsql_row->str);*/
                                            printf("[%s] (insert) to \'%s\' 1 row execute in %.2f ms is ok ! \n"
                                                    , current_time_format()->str
                                                    , table_name->str
                                                    , run_time_diff(start , end));
                                            printf("[INSERT END RETURN M] %d AND %s\n" , m , tokensb->tokens[m]->str);
                                            return m;
                                        }
                                    }
                                }
                            }
                        }
                        else if(is_open_single_quote == 1){
                            combine_tail(xsql_row , tokensb->tokens[k]->str);
                            combine_tail(temp_data , tokensb->tokens[k]->str);
                            combine_tail(temp_data , "^");
                            continue;
                        } else if(is_open_single_quote == 0&& IS_CONTAIN_KEYS(tokensb->tokens[k]->str)
                        && compare(tokensb->tokens[k] , ";")){
                            combine_tail(xsql_row , tokensb->tokens[k]->str);
                            is_end = 1;
                            break;
                        } else {
                            combine_tail(xsql_row , tokensb->tokens[k]->str);
                        }
                    }
                } else if (compare(tokensb->tokens[j] , "->")){
                    // "->"
                }else if (is_end == 1)break;
            }
        } else if(is_end == 1){break;}
    }
}

// SELECT id , school FROM student WHERE id = '15' AND age = '24' AND name = 'GeemMorl3';

SELECT_CONDITION *create_selectCondition(){
    SELECT_CONDITION *selectCondition = (SELECT_CONDITION*)malloc(sizeof(SELECT_CONDITION));
    selectCondition->content = (String**) malloc(15 * sizeof(String*));
    selectCondition->count = 15;
    for (int i = 0; i < 15; ++i)
        selectCondition->content[i] = create_string("");
    return selectCondition;
}

SELECT_CONDITION *extend_selectCondition(SELECT_CONDITION *old){
    int new_count = old->count * 2;
    SELECT_CONDITION *selectCondition = (SELECT_CONDITION*)malloc(sizeof(SELECT_CONDITION));
    selectCondition->content = (String**) malloc(new_count * sizeof(String*));
    selectCondition->count = new_count;
    for (int i = 0; i < old->count; ++i)
        copy_string(old->content[i] , selectCondition->content[i]);
    for (int i = old->count; i < new_count; ++i)
        selectCondition->content[i] = create_string("");
    free_selectCondition(old);
    return selectCondition;
}

void free_selectCondition(SELECT_CONDITION *selectCondition){
    for (int i = 0; i < selectCondition->count; ++i)
        string_free(selectCondition->content[i]);
    free(selectCondition);
}

int SELECT_exe(TABLE_LIST_NODE *head,TOKENSB *tokensb , int curr){
    clock_t start = clock();
    //printf("[EN SELECT]\n");
    int is_over_key_from = 0;
    int is_over_key_where = 0;
    int select_condition_count = 0;
    TABLE_LIST_NODE *TARGET_TABLE = NULL;
    String *table_name = create_string("");
    SELECT_CONDITION *selectCondition = create_selectCondition();
    WHERE_CONDITION *whereCondition;
    int *reflect_field_index;
    int whereCondition_field_count = 0;
    int whereCondition_data_count = 0;
    int whereCondition_logic_count = 0;
    int where_start_char_effective_count = 0;
    for (int i = curr; i < tokensb->count; ++i) {
        if (IS_CONTAIN_KEYS(tokensb->tokens[i]->str)&& compare(tokensb->tokens[i] , "from")
            &&!is_over_key_where){
            is_over_key_from = 1;
        }else if(!IS_CONTAIN_KEYS(tokensb->tokens[i]->str)&&!compare(tokensb->tokens[i] , " ")
                 &&!is_over_key_where&&is_over_key_from){ // table name
            combine_tail(table_name , tokensb->tokens[i]->str);
            TARGET_TABLE = get_TABLE_LIST_NODE(head , table_name->str);
            if(TARGET_TABLE == NULL){
                log_error_string("table is not create!");
                return i + 1;
            }
            //printf("[select target_table] %s\n" , TARGET_TABLE->table->dataline_head->dataline->DATA[1]->str);
            is_over_key_from = 0;
            continue;
        }
    }
    for (int i = curr; i < tokensb->count; ++i) {
        if((!compare(tokensb->tokens[i] , " ")&&!IS_CONTAIN_KEYS(tokensb->tokens[i]->str)
        &&!is_over_key_from&&!compare(tokensb->tokens[i] , ",")&&!is_over_key_where)|| compare(tokensb->tokens[i] , "*")){ // fields
            if (selectCondition->count == select_condition_count){
                free_selectCondition(selectCondition);
                selectCondition = extend_selectCondition(selectCondition);
            }
            copy_string(tokensb->tokens[i] , selectCondition->content[select_condition_count++]);
            continue;
        } else if (IS_CONTAIN_KEYS(tokensb->tokens[i]->str)&& compare(tokensb->tokens[i] , "from")
        &&!is_over_key_where){
            reflect_field_index = SELECT_exe_SELECT_CONDITION_end_handle
                    (TARGET_TABLE , selectCondition , select_condition_count , &where_start_char_effective_count);
            is_over_key_from = 1;
            continue;
        } else if (is_over_key_from == 1&& IS_CONTAIN_KEYS(tokensb->tokens[i]->str)
        && compare(tokensb->tokens[i] , "where")&&!is_over_key_where){
            is_over_key_where = 1;
            whereCondition = parse_WHERE_CONDITION(head , tokensb
            , i,&whereCondition_field_count,&whereCondition_data_count , &whereCondition_logic_count);
            continue;
        }else if(is_over_key_from && !is_over_key_where && compare(tokensb->tokens[i] , ";")){
            //printf("[;]\n");
            is_over_key_where = 1;
            i--;
            continue;
        } 
        else if(is_over_key_from&&is_over_key_where&&compare(tokensb->tokens[i] , ";")){ // not WHERE , is ";"
            //printf("[where]%d\n" , whereCondition_field_count);
            
            SELECT_exe_DATA_QUERY(TARGET_TABLE , selectCondition , reflect_field_index
                                  , select_condition_count , whereCondition , &whereCondition_field_count
                                  , &whereCondition_data_count , &whereCondition_logic_count
                                  , &where_start_char_effective_count);
            for (int j = i; j < tokensb->count; ++j) {
                if (compare(tokensb->tokens[j] , ";")){
                    for (int k = j; k < tokensb->count; ++k) {
                        if (IS_PRIMARY_KEY(tokensb->tokens[k]->str)){
                            clock_t end = clock();
                            printf("[%s] (select) to \'%s\' 1 row execute in %.2f ms is ok ! \n"
                                    , current_time_format()->str
                                    , table_name->str
                                    , run_time_diff(start , end));
                            return k - 1;
                        } else if (k == tokensb->count - 1){
                            clock_t end = clock();
                            printf("[%s] (select) to \'%s\' 1 row execute in %.2f ms is ok ! \n"
                                    , current_time_format()->str
                                    , table_name->str
                                    , run_time_diff(start , end));
                            return k - 1;
                        }
                    }
                }
            }
        }
        else {
            continue;
        }
    }
}

void SELECT_exe_DATA_QUERY(TABLE_LIST_NODE *TARGET_TABLE,SELECT_CONDITION *selectCondition
                           , int *reflect_field_index, int select_condition_count
                           , WHERE_CONDITION *whereCondition, int *whereCondition_field_count
                           ,int *whereCondition_data_count,int *whereCondition_logic_count
                           ,int *where_start_char_effective_count){
    FIELD_INDEXS *pFieldIndexs;
    if(*whereCondition_field_count != 0)pFieldIndexs = get_field_indexs_by_field(
        TARGET_TABLE , whereCondition->field_name , *whereCondition_field_count);
    int dataline_count = 0;
    datalineArray_calc_param_t datalineArray_calc_param;
    datalineArray_calc_param.target_table = TARGET_TABLE;
    datalineArray_calc_param.select_condition_count = select_condition_count;
    datalineArray_calc_param.whereCondition_field_count = whereCondition_field_count;
    datalineArray_calc_param.field_indexs = pFieldIndexs;
    datalineArray_calc_param.whereCondition = whereCondition;
    datalineArray_calc_param.dataline_count = &dataline_count;
    DATALINE_ARRAY *datalineArray;
    /*LOG("print exe data targetTable" ,TARGET_TABLE->table->NAME->str);
    printf("[DEBUG] %s\n" , TARGET_TABLE->table->dataline_head->dataline->DATA[1]->str);
    printf("auth datalineArray count : %d\n" , dataline_count);*/
    if(*whereCondition_field_count == 0){
        datalineArray = query_dataline_array_all_calc(TARGET_TABLE , &dataline_count);
    }else {/*LOG("" , "!= 0 !!!");*/datalineArray = query_dataline_array_calc(datalineArray_calc_param);}
    //LOG("print arrays" , datalineArray->datalines[1]->DATA[1]->str);
    
    /*for(int i = 0 ; i < TARGET_TABLE->table->length ; i++){
        LOG("table field list" , TARGET_TABLE->table->FIELD[i]->str);
    }
    for(int i = 0; i < dataline_count ; i++){
        for(int j = 0 ; j < TARGET_TABLE->table->length ; j ++){
            LOG("dataline array list" , datalineArray->datalines[i]->DATA[j]->str);
        }
    }*/
    DATALINE_NODE *temp = TARGET_TABLE->table->dataline_head->next;
    if (compare(selectCondition->content[0] , "*")){
        LOG("" , "is * !!");
        select_condition_count = *where_start_char_effective_count;
    }
    int *field_max_len = (int*)malloc(select_condition_count * sizeof(int));
    String *lines = query_upon_down_line_num_calc(field_max_len , TARGET_TABLE , datalineArray 
    , reflect_field_index , select_condition_count , dataline_count);
    int *field_space_nums = query_field_space_nums_calc(field_max_len , TARGET_TABLE 
        , datalineArray , reflect_field_index , select_condition_count);
    printf("+%s+\n" , lines->str);
    for (int i = 0; i < select_condition_count; ++i) {
        String *space = create_string("");
            if(i == select_condition_count - 1){
                printf("%s\n\n" , TARGET_TABLE->table->FIELD[reflect_field_index[i]]->str);
            continue;
        }
        for (int j = 0; j < field_space_nums[i]; j++)combine_tail(space , " ");
        printf("%s%s" , TARGET_TABLE->table->FIELD[reflect_field_index[i]]->str , space->str);
            delete_all(space);
    }
   /*for (int i = 0; i < dataline_count; ++i) {
        for (int j = 0; j < select_condition_count; ++j) {
            if (j == select_condition_count - 1) {
                printf("%s\n", datalineArray->datalines[i]->DATA[reflect_field_index[j]]->str);
                break;
            }
            String *data_space = query_data_field_space(datalineArray->datalines[i]
                ,TARGET_TABLE , reflect_field_index[j] , field_max_len , field_space_nums , j);
            printf("%s%s", datalineArray->datalines[i]->DATA[reflect_field_index[j]]->str,data_space->str);
            string_free(data_space);
        }
    }*/
    printf("+%s+\n" , lines->str);
    printf("query %d rows is ok!>\n" , dataline_count);
}

DATALINE_ARRAY *query_dataline_array_calc(datalineArray_calc_param_t datalineArray_calc_param){
    DATALINE_ARRAY *datalineArray = create_DATALINE_ARRAY(datalineArray_calc_param.target_table);
    //LOG("SELECT ... FROM; table name" , datalineArray_calc_param.target_table->table->NAME->str);
    DATALINE_NODE *temp = datalineArray_calc_param.target_table->table->dataline_head;
    //LOG("end node" , temp->next->dataline->DATA[1]->str);
    /*if(temp->next->next == NULL){
        LOG("" , "no matter!!!!!!!!!!!!!");
    }*/
    while (temp != NULL){
        for (int i = 0; i < *datalineArray_calc_param.whereCondition_field_count; ++i) { // ^ 3
            if (compare(temp->dataline->DATA[datalineArray_calc_param.field_indexs->field_indexs[i]]
                        ,datalineArray_calc_param.whereCondition->data[i]->str)){
                            //LOG("WHERE field match" , temp->dataline->DATA[datalineArray_calc_param.field_indexs->field_indexs[i]]->str);
                            //LOG("AUTH WHERE field" , datalineArray_calc_param.whereCondition->data[i]->str);
                        } 
                        else break;
            if (i == (*datalineArray_calc_param.whereCondition_field_count) - 1
            && compare(temp->dataline->DATA[datalineArray_calc_param.field_indexs->field_indexs[i]]
                       , datalineArray_calc_param.whereCondition->data[i]->str)){
                if (datalineArray->count == *datalineArray_calc_param.dataline_count) // 扩容
                    datalineArray = extend_DATALINE_ARRAY(datalineArray_calc_param.target_table , datalineArray );
                //LOG("dataline Array is store!!" , temp->dataline->DATA[1]->str);
                datalineArray->datalines[(*datalineArray_calc_param.dataline_count)++] = temp->dataline;
                //LOG("process" , "store is ok!!");
            }
        }
        temp = temp->next;
    }
    //LOG("process" , "break array calc");
    return datalineArray;
}

DATALINE_ARRAY *query_dataline_array_all_calc(TABLE_LIST_NODE *target_table , int *dataline_count){
    DATALINE_ARRAY *datalineArray = create_DATALINE_ARRAY(target_table);
    DATALINE_NODE *temp = target_table->table->dataline_head;
    while (temp != NULL){
        if(*dataline_count == datalineArray->count)
            datalineArray = extend_DATALINE_ARRAY(target_table , datalineArray);
        datalineArray->datalines[(*dataline_count)++] = temp->dataline;
        temp = temp->next;
    }
    return datalineArray;
}

String *query_data_field_space(DATALINE*dataline ,TABLE_LIST_NODE *target_table 
    , int curr_field_place_index ,int *field_max_len , int *field_space_nums , int curr_select_condition_index){
    int space_count = 0;String *data_field_space = create_string("");
    int field_space_all = target_table->table->FIELD[curr_field_place_index]->length
    + field_space_nums[curr_select_condition_index];
    space_count = abs(field_space_all  - dataline->DATA[curr_field_place_index]->length);
    for(int i = 0 ; i < space_count; i++)combine_tail(data_field_space , " ");
    return data_field_space;
}

String *query_upon_down_line_num_calc(int *field_max_len ,TABLE_LIST_NODE*target_table 
    ,DATALINE_ARRAY *dataline_array_t , int *query_field_indexs , int authentic_query_field_num 
    ,int authentic_datalines_count ){
    //LOG("EN line" , "");
    String *lines = create_string("");
    int line_num = 0 , max_count = 0;
    for (int i = 0; i < authentic_query_field_num;i++){ // 3
        int temp_field_line_num = 0;
        for (int j = 0; j < authentic_datalines_count;j++){
            if (dataline_array_t->datalines[j]->DATA[query_field_indexs[i]]->length > temp_field_line_num)
                temp_field_line_num = dataline_array_t->datalines[j]->DATA[query_field_indexs[i]]->length;
        }
        field_max_len[max_count++] = temp_field_line_num;
        line_num += temp_field_line_num + 6;
    }
    for(int i = 0 ; i < authentic_query_field_num ; i++)
        line_num += target_table->table->FIELD[query_field_indexs[i]]->length;
    for (int i = 0 ; i < line_num ; i++)
        combine_tail(lines , "-");
    return lines;
}

int *query_field_space_nums_calc(int *field_max_len , TABLE_LIST_NODE*target_table 
    , DATALINE_ARRAY*datalineArray , int *query_field_indexs , int authentic_query_field_num ){
    int *space_nums = (int*)malloc(authentic_query_field_num * sizeof(int));
    int spance_nums_count = 0;
    for(int i = 0 ; i < authentic_query_field_num ; i++){ // 2
        int temp_space_num = 0;
        temp_space_num = target_table->table->FIELD[query_field_indexs[i]]->length 
        < field_max_len[i]   ?   abs(target_table->table->FIELD[query_field_indexs[i]]->
            length - field_max_len[i]) + 6 : 6;
        space_nums[spance_nums_count++] = temp_space_num;
    }
    return space_nums;
}

WHERE_CONDITION *parse_WHERE_CONDITION(
        TABLE_LIST_NODE *head,TOKENSB *tokensb
        , int curr , int *whereCondition_field_count
        ,int *whereCondition_data_count,int *whereCondition_logic_count
        ){
    int is_over_single_quote = 0;
    WHERE_CONDITION *whereCondition = create_whereCondition();
    for (int i = curr; i < tokensb->count; ++i) {
        
        if(!IS_CONTAIN_KEYS(tokensb->tokens[i]->str)&&!compare(tokensb->tokens[i] , " ")
        && !compare(tokensb->tokens[i] , "\'")&&!is_over_single_quote&&!compare(tokensb->tokens[i] , "=")){ // field
            if(*whereCondition_field_count == whereCondition->common_count - 1){
                whereCondition = extend_whereCondition(whereCondition);
                copy_string(tokensb->tokens[i] , whereCondition->field_name[(*whereCondition_field_count)++]);
            }
            copy_string(tokensb->tokens[i] , whereCondition->field_name[(*whereCondition_field_count)++]);
            //printf("[field  is  !!] %s\n" , whereCondition->field_name[*whereCondition_field_count - 1]->str);
            continue;
        } else if(IS_CONTAIN_KEYS(tokensb->tokens[i]->str)&& compare(tokensb->tokens[i] , "\'")){ // curr "\'"
            if(is_over_single_quote)is_over_single_quote = 0; else is_over_single_quote = 1;
            continue;
        } else if(is_over_single_quote){ // value
            if(*whereCondition_data_count == whereCondition->common_count - 1){
                whereCondition = extend_whereCondition(whereCondition);
                copy_string(tokensb->tokens[i] , whereCondition->data[(*whereCondition_data_count)++]);
            }
            copy_string(tokensb->tokens[i] , whereCondition->data[(*whereCondition_data_count)++]);
            continue;
        } else if(!is_over_single_quote&& IS_CONTAIN_KEYS(tokensb->tokens[i]->str)
        &&!compare(tokensb->tokens[i] , " ")&& compare(tokensb->tokens[i] , "and")){ // logic
            if(*whereCondition_logic_count == whereCondition->common_count - 1){
                whereCondition = extend_whereCondition(whereCondition);
                copy_string(tokensb->tokens[i] , whereCondition->logic_condition[(*whereCondition_logic_count)++]);
            }
            copy_string(tokensb->tokens[i] , whereCondition->logic_condition[(*whereCondition_logic_count)++]);
            continue;
        } else if(compare(tokensb->tokens[i] , ";")){
            return whereCondition;
        } else {
            continue;
        }
    }
}

int* SELECT_exe_SELECT_CONDITION_end_handle(TABLE_LIST_NODE *target_table_node,SELECT_CONDITION *selectCondition
                                            , int effective_selectCondition_count , int *where_start_char_effective_count){
    if(compare(selectCondition->content[0] , "*")){
        int *reflect_field_index = (int*)malloc(target_table_node->table->length * sizeof(int));
        for (int i = 0; i < target_table_node->table->length; ++i)
            reflect_field_index[(*where_start_char_effective_count)++] = i;
        (*where_start_char_effective_count)--;
        return reflect_field_index;
    }
    int *reflect_field_index = (int*)malloc(effective_selectCondition_count * sizeof(int));
    int reflect_count = 0;
    for (int i = 0; i < effective_selectCondition_count; ++i) {
        for (int j = 0; j < target_table_node->table->length; ++j) {
            if(compare(selectCondition->content[i] , target_table_node->table->FIELD[j]->str))
                reflect_field_index[reflect_count++] = j;
        }
    }
    return reflect_field_index;
}

WHERE_CONDITION *create_whereCondition(){
    WHERE_CONDITION *whereCondition = (WHERE_CONDITION *) malloc(sizeof(WHERE_CONDITION));
    whereCondition->field_name = (String**) malloc(10 * sizeof(String*));
    whereCondition->data = (String**) malloc(10 * sizeof(String*));
    whereCondition->logic_condition = (String**) malloc(10 * sizeof(String*));
    whereCondition->common_count = 10;
    for (int i = 0; i < 10; ++i)
        whereCondition->field_name[i] = create_string("");
    for (int i = 0; i < 10; ++i)
        whereCondition->data[i] = create_string("");
    for (int i = 0; i < 10; ++i)
        whereCondition->logic_condition[i] = create_string("");
    return whereCondition;
}

WHERE_CONDITION *extend_whereCondition(WHERE_CONDITION *old){
    //printf("EN extend\n");
    int new_count = old->common_count * 2;
    WHERE_CONDITION *whereCondition = (WHERE_CONDITION *) malloc(sizeof(WHERE_CONDITION));
    whereCondition->field_name = (String**) malloc(new_count * sizeof(String*));
    whereCondition->data = (String**) malloc(new_count * sizeof(String*));
    whereCondition->logic_condition = (String**) malloc(new_count * sizeof(String*));
    whereCondition->common_count = new_count;
    for (int i = 0; i < old->common_count; ++i)
        copy_string(old->field_name[i] , whereCondition->field_name[i]);
    for (int i = 0; i < old->common_count; ++i)
        copy_string(old->data[i] , whereCondition->data[i]);
    for (int i = 0; i < old->common_count; ++i)
        copy_string(old->logic_condition[i] , old->logic_condition[i]);

    for (int i = old->common_count; i < new_count; ++i)
        whereCondition->field_name[i] = create_string("");
    for (int i = old->common_count; i < new_count; ++i)
        whereCondition->data[i] = create_string("");
    for (int i = old->common_count; i < new_count; ++i)
        whereCondition->logic_condition[i] = create_string("");

    whereCondition->common_count = new_count;
}

void free_whereCondition(WHERE_CONDITION *whereCondition){
    for (int i = 0; i < whereCondition->common_count; ++i)
        string_free(whereCondition->field_name[i]);
    for (int i = 0; i < whereCondition->common_count; ++i)
        string_free(whereCondition->data[i]);
    for (int i = 0; i < whereCondition->common_count; ++i)
        string_free(whereCondition->logic_condition[i]);
    free(whereCondition->data);
    free(whereCondition->field_name);
    free(whereCondition->logic_condition);
    free(whereCondition);
}

DATALINE_ARRAY *create_DATALINE_ARRAY(TABLE_LIST_NODE *TARGET_TABLE){
    //printf("[EN extend dataline_array]\n");
    DATALINE_ARRAY *pDatalineArray = (DATALINE_ARRAY*) malloc(sizeof(DATALINE_ARRAY));
    pDatalineArray->datalines = (DATALINE **) malloc(10 * sizeof(DATALINE*));
    for (int i = 0; i < 10; ++i)
        pDatalineArray->datalines[i] = NULL;
    pDatalineArray->count = 10;
    return pDatalineArray;
}

DATALINE_ARRAY *extend_DATALINE_ARRAY(TABLE_LIST_NODE *TARGET , DATALINE_ARRAY *old){
    int new_count = old->count * 2;
    DATALINE **datalines = (DATALINE**)malloc(new_count * sizeof(DATALINE*));
    for(int i = 0; i < old->count ; i ++)
        datalines[i] = old->datalines[i];
    for(int i = old->count ; i < new_count ; i ++)
        datalines[i] = NULL;
    free(old->datalines);
    old->datalines = datalines;
    old->count = new_count;
    return old;
}

//free DATALINE_ARRAY
void free_DATALINE_ARRAY(DATALINE_ARRAY *datalineArray , int authentic_table_field_count){
    if (datalineArray == NULL)return;
    if(datalineArray->datalines != NULL)
        free(datalineArray->datalines);
    free(datalineArray);
}

static KEYS keys_to_KEYSTYPE(String *key){
    if(compare(key , "create"))return CREATE_K;
    else if(compare(key , "insert"))return INSERT_K;
    else if(compare(key , "select"))return SELECT_K;
    else if(compare(key , "update"))return UPDATE_K;
    else return NULL_K;
}

void XSQL_RUN(TOKENSB *tokensb, TABLE_LIST_NODE *head , FILE *table_file 
    , FILE *data_file , table_db_t *dbT){
    for (int i = 0; i < tokensb->count; ++i) {
        //printf("[1] i = %d ; str = %s\n" , i , tokensb->tokens[i]->str);
        KEYS keys_enum = keys_to_KEYSTYPE(tokensb->tokens[i]);
        switch (keys_enum) {
            case CREATE_K:
            //printf("EN create\n");
                i = CREATE_exe(head , tokensb , i , table_file , data_file , dbT);
                //printf("[i]%s  %d\n" , tokensb->tokens[i]->str , i);
                //printf("[BREAK CREATE TABLE I] %d  AND token = %s\n" , i , tokensb->tokens[i + 1]->str);
                break;
            case INSERT_K:
                //printf("[DEBUG] INSERT_exe  = %s\n", tokensb->tokens[i]->str);
                i = INSERT_exe(head ,tokensb , i , dbT);
                //printf("[BREAK INSERT TABLE I] %d  AND token = %s\n" , i , tokensb->tokens[i]->str);
                //printf("[INSERT EXE SUCCESS!!!]\n\n\n");
                break;
            case SELECT_K:
                SELECT_exe(head , tokensb , i);
                break;
            case UPDATE_K:
                UPDATE_exe(head , tokensb , i);
                break;
            default:
                break;
        }
    }
}
