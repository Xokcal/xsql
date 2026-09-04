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
    clock_t start = clock();
    //printf("[ENTRE_CREATE_EXE]\n");
    String *table_name = create_string("");
    String *xsql = create_string("");
    combine_tail(xsql , tokensb->tokens[curr]->str);
    for (int i = curr; i < tokensb->count; ++i) {
        //combine_tail(xsql , tokensb->tokens[i]->str);
        if(IS_CONTAIN_KEYS(tokensb->tokens[i]->str)&&!compare(tokensb->tokens[i] , " ")){

            //printf("[ENTRE   TABLE   KEY]%s\n" , tokensb->tokens[i]->str);
            if(compare(tokensb->tokens[i] , "TABLE")){
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
                        continue;
                    }if (!IS_CONTAIN_KEYS(tokensb->tokens[j]->str)
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
                                string_free(table_name);
                                //printf("[CREATE END RETURN I] %d" , k);
                                return k - 1;
                            } else if (k == tokensb->count - 1){
                                //printf("[K]%s\n" , tokensb->tokens[k]->str);
                                printf("[%s] xsql server> %s \n"
                                        , current_time_format()->str
                                        , xsql->str);
                                clock_t end = clock();
                                printf("[%s] (create) table \'%s\' execute in %.2f ms is ok ! \n"
                                        , current_time_format()->str
                                        , table_name->str
                                        , run_time_diff(start , end));
                                string_free(table_name);
                                //printf("[CREATE END RETURN I] %d" , k);
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

int INSERT_exe(TABLE_LIST_NODE *head,TOKENSB *tokensb , int curr){
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
    for (int i = curr; i < tokensb->count; ++i) {
        combine_tail(xsql_row , tokensb->tokens[i]->str);
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
                    for (int k = 0; k < field_place_count; ++k) {
                        //printf("[field_place_index]%d\n" , field_place_index[k]);
                    }
                    continue;
                } else if (is_over_table_field_left == 1&&is_over_table_field_right == 0
                && IS_CONTAIN_KEYS(tokensb->tokens[j]->str)&& compare(tokensb->tokens[j] , ")")){ // ")"
                    //printf("[end \")\"]\n");
                    is_over_table_field_right = 1;
                    continue;
                } else if(IS_CONTAIN_KEYS(tokensb->tokens[j]->str)&& compare(tokensb->tokens[j] , ":")){
                    // ":"
                    // insert student(id , age , name) : ('12' , '21' , 'XiaoLi');
                    //printf("[is \":\"]\n");
                    combine_tail(temp_field , tokensb->tokens[j]->str);
                    int is_over_insert_left = 0;
                    int is_over_insert_right = 0;
                    int is_open_single_quote = 0;
                    for (int k = j; k < tokensb->count; ++k) {
                        if (IS_CONTAIN_KEYS(tokensb->tokens[k]->str)&& compare(tokensb->tokens[k] , "(")
                        &&is_over_insert_left == 0&&is_over_insert_right == 0){
                            combine_tail(xsql_row , tokensb->tokens[k]->str);
                            // "("
                            //printf("[:  \"(\"]\n");
                            is_over_insert_left = 1;
                            continue;
                        } else if(IS_CONTAIN_KEYS(tokensb->tokens[k]->str)&& compare(tokensb->tokens[k] , "\'")){
                            //printf("[:  \"\'\"]%d\n" , is_open_single_quote);
                            combine_tail(xsql_row , tokensb->tokens[k]->str);
                            if(is_open_single_quote) { // =1
                                is_open_single_quote = 0;
                            }
                            else is_open_single_quote = 1;
                            continue;
                        } else if (IS_CONTAIN_KEYS(tokensb->tokens[k]->str)&&is_open_single_quote == 0
                        && compare(tokensb->tokens[k] , ")")){
                            combine_tail(xsql_row , tokensb->tokens[k]->str);
                            //printf("[temp   data]%s\n" , temp_data->str);
                            Splitor *splitor = split(temp_data , "^");
                            //printf("[DATA_COUNT]%d\n" , splitor->count);
                            DATALINE_NODE *new_node = create_DATALINE_NODE(
                                    splitor->splits
                                    , field_place_index
                                    , field_place_count
                                    , TARGET_TABLE_NODE->table->length
                            );
                            //printf("[IS CREATE DATALINE!!!]\n");
                            /*for (int l = 0; l < splitor->count; ++l) {
                                printf("[splitor  str]%s\n" , splitor->splits[l]->str);
                            }*/
                            add_DATALINE_NODE(TARGET_TABLE_NODE->table->dataline_head , new_node);
                            //printf("[IS ADD!!!]\n");
                            DATALINE_NODE *data1 = get_DATALINE_NODE(TARGET_TABLE_NODE->table->dataline_head , "12");
                            //printf("[DATA1  VALUE]%s\n" , data1->dataline->DATA[0]->str);
                            delete_all(temp_data);
                            for (int l = k; l < tokensb->count; ++l) {
                                //printf("[L]%s\n" , tokensb->tokens[l]->str);
                                //printf("[LNUM]%d\n" , l);
                                if (compare(tokensb->tokens[l] , ";")){
                                    combine_tail(xsql_row , tokensb->tokens[l]->str);
                                    //printf("[ENTRE LLLL!!!]\n");
                                    for (int m = l; m < tokensb->count; ++m) {
                                        if (IS_PRIMARY_KEY(tokensb->tokens[m]->str)){
                                            clock_t end = clock();
                                            //printf("[MM] = %d ; [M STR] = %s\n" , m , tokensb->tokens[m]->str);
                                            /*printf("[%s] xsql server> %s\n"
                                                   , current_time_format()->str , xsql_row->str);*/
                                            printf("[%s] (insert) to \'%s\' 1 row execute in %.2f ms is ok ! \n"
                                                   , current_time_format()->str
                                                   , table_name->str
                                                   , run_time_diff(start , end));
                                            printf("[INSERT END RETURN M] %d AND %s\n" , m , tokensb->tokens[m]->str);
                                            return m - 1;
                                        } else if (m == tokensb->count - 1){
                                            clock_t end = clock();
                                            //printf("[MM] = %d ; [M STR] = %s\n" , m , tokensb->tokens[m]->str);
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
    //printf("[ENTRE SELECT!!]\n");
    int is_over_key_from = 0;
    int is_over_key_where = 0;
    int select_condition_count = 0;
    TABLE_LIST_NODE *TARGET_TABLE;
    String *table_name = create_string("");
    SELECT_CONDITION *selectCondition = create_selectCondition();
    WHERE_CONDITION *whereCondition;
    int *reflect_field_index;
    int whereCondition_field_count = 0;
    int whereCondition_data_count = 0;
    int whereCondition_logic_count = 0;
    for (int i = curr; i < tokensb->count; ++i) {
        if(compare(tokensb->tokens[i] , " ")&&!IS_CONTAIN_KEYS(tokensb->tokens[i]->str)
        &&!is_over_key_from&&!compare(tokensb->tokens[i] , ",")){
            if (selectCondition->count == select_condition_count){
                free_selectCondition(selectCondition);
                selectCondition = extend_selectCondition(selectCondition);
            }
            copy_string(tokensb->tokens[i] , selectCondition->content[select_condition_count++]);
            continue;
        } else if (IS_CONTAIN_KEYS(tokensb->tokens[i]->str)&& compare(tokensb->tokens[i] , "FROM")){
            reflect_field_index = SELECT_exe_SELECT_CONDITION_end_handle
                    (TARGET_TABLE , selectCondition , select_condition_count);
            is_over_key_from = 1;
            continue;
        } else if(!IS_CONTAIN_KEYS(tokensb->tokens[i]->str)&&!compare(tokensb->tokens[i] , " ")){ // table name
            combine_tail(table_name , tokensb->tokens[i]->str);
            TARGET_TABLE = get_TABLE_LIST_NODE(head , table_name->str);
            continue;
        } else if (is_over_key_from == 1&& IS_CONTAIN_KEYS(tokensb->tokens[i]->str)
        && compare(tokensb->tokens[i] , "WHERE")){
            is_over_key_where = 1;
            whereCondition = parse_WHERE_CONDITION(head , tokensb
            , i,&whereCondition_field_count,&whereCondition_data_count , &whereCondition_logic_count);
            continue;
        } else if(is_over_key_from&& compare(tokensb->tokens[i] , ";")){ // not WHERE , is ";"
            SELECT_exe_DATA_QUERY(selectCondition , whereCondition);
        }
        else {
            continue;
        }
    }
}

// SELECT id , school FROM student WHERE id = '15' AND age = '24' AND name = 'GeemMorl3';

WHERE_CONDITION *parse_WHERE_CONDITION(
        TABLE_LIST_NODE *head,TOKENSB *tokensb
        , int curr , int *whereCondition_field_count
        ,int *whereCondition_data_count,int *whereCondition_logic_count
        ){
    int is_over_single_quote = 0;
    WHERE_CONDITION *whereCondition = create_whereCondition();
    for (int i = curr; i < tokensb->count; ++i) {
        if(!IS_CONTAIN_KEYS(tokensb->tokens[i]->str)&&!compare(tokensb->tokens[i] , " ")
        && !compare(tokensb->tokens[i] , "\'")&&!is_over_single_quote){
            if(*whereCondition_field_count == whereCondition->common_count - 1){
                whereCondition = extend_whereCondition(whereCondition);
                copy_string(tokensb->tokens[i] , whereCondition->field_name[*whereCondition_field_count++]);
            }
            copy_string(tokensb->tokens[i] , whereCondition->field_name[*whereCondition_field_count++]);
            continue;
        } else if(IS_CONTAIN_KEYS(tokensb->tokens[i]->str)&& compare(tokensb->tokens[i] , "\'")){
            if(is_over_single_quote)is_over_single_quote = 0; else is_over_single_quote = 1;
            continue;
        } else if(is_over_single_quote){
            if(*whereCondition_data_count == whereCondition->common_count - 1){
                whereCondition = extend_whereCondition(whereCondition);
                copy_string(tokensb->tokens[i] , whereCondition->data[*whereCondition_data_count++]);
            }
            copy_string(tokensb->tokens[i] , whereCondition->data[*whereCondition_data_count++]);
            continue;
        } else if(!is_over_single_quote&& IS_CONTAIN_KEYS(tokensb->tokens[i]->str)){
            if(*whereCondition_logic_count == whereCondition->common_count - 1){
                whereCondition = extend_whereCondition(whereCondition);
                copy_string(tokensb->tokens[i] , whereCondition->logic_condition[*whereCondition_logic_count++]);
            }
            copy_string(tokensb->tokens[i] , whereCondition->logic_condition[*whereCondition_logic_count++]);
            continue;
        } else if(compare(tokensb->tokens[i] , ";")){
            return whereCondition;
        }
    }
}

int* SELECT_exe_SELECT_CONDITION_end_handle(TABLE_LIST_NODE *target_table_node,SELECT_CONDITION *selectCondition , int effective_selectCondition_count){
    if(compare(selectCondition->content[0] , "*")){
        int *reflect_field_index = (int*)malloc(target_table_node->table->length * sizeof(int));
        for (int i = 0; i < target_table_node->table->length; ++i)
            reflect_field_index[i] = i;
        return reflect_field_index;
    }
    int *reflect_field_index = (int*)malloc(effective_selectCondition_count * sizeof(int));
    int reflect_count = 0;
    for (int i = 0; i < effective_selectCondition_count; ++i) {
        for (int j = 0; j < target_table_node->table->length; ++j) {
            compare(selectCondition->content[i] , target_table_node->table->FIELD[j]->str);
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
    free(whereCondition);
}

void SELECT_exe_DATA_QUERY(SELECT_CONDITION *selectCondition , WHERE_CONDITION *whereCondition){

}

static KEYS keys_to_KEYSTYPE(String *key){
    if(compare(key , "CREATE"))return CREATE_K;
    else if(compare(key , "INSERT"))return INSERT_K;
    else if(compare(key , "SELECT"))return SELECT_K;
}

void XSQL_RUN(TOKENSB *tokensb){
    TABLE_LIST_NODE *head = create_TABLE_LIST_NODE(create_string("XSQL") , NULL , 0);
    for (int i = 0; i < tokensb->count; ++i) {
        //printf("[1] i = %d ; str = %s\n" , i , tokensb->tokens[i]->str);
        KEYS keys_enum = keys_to_KEYSTYPE(tokensb->tokens[i]);
        switch (keys_enum) {
            case CREATE_K:
                i = CREATE_exe(head , tokensb , i);
                TABLE_LIST_NODE *table_get = get_TABLE_LIST_NODE(head , "student");
                String** table_msg = table_get->table->FIELD;
                //printf("[BREAK CREATE TABLE I] %d  AND token = %s\n" , i , tokensb->tokens[i]->str);
                break;
            case INSERT_K:
                i = INSERT_exe(head , tokensb , i);
                //printf("[BREAK INSERT TABLE I] %d  AND token = %s\n" , i , tokensb->tokens[i]->str);
                //printf("[INSERT EXE SUCCESS!!!]\n\n\n");
                break;
            case SELECT_K:
                //SELECT_exe(head , tokensb , i);
                break;
            default:
                break;

        }
    }
}
