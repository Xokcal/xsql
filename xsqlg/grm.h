//
// Created by 26432 on 2026/8/30.
//

#ifndef XOKSQLC语言版_GRM_H
#define XOKSQLC语言版_GRM_H

#include "../xstr/String.h"
#include <stdio.h>
#include <stdlib.h>
#include "../list/list.h"

typedef enum KEYS{
    CREATE_K,
    TABLE_K,
    SELECT_K,
    FROM_K,
    WHERE_K,
    INSERT_K,
    NULL_K
}KEYS;

typedef struct TOKENSB{
    String **tokens;
    int count;
}TOKENSB;

typedef struct SELECT_CONDITION{
    String **content;
    int count;
}SELECT_CONDITION;

typedef struct WHERE_CONDITION{
    String **logic_condition;
    String **field_name;
    String **data;
    int common_count;
}WHERE_CONDITION;

typedef struct FIELD_INDEXS{
    int *field_indexs;
    int count;
}FIELD_INDEXS;

typedef struct DATALINE_ARRAY{
    DATALINE **datalines;
    int count;
}DATALINE_ARRAY;

static char* sql_keys[20] = {"SELECT" , "CREATE" , "TABLE" , "{" , "}"
                            , "(" , ")" , "COMMENT" , "\'" , "FROM"
                            , "," , "WHERE" , ";" , " " , "*" , "INSERT"
                            ,"STRING" , ":" , "\n" , "AND"};

static char* primary_keys[5] = {"INSERT" , "SELECT" , "UPDATE" , "CREATE" , "DELETE"};

static char* sql_keys_primary[2] = {"CREATE" , "INSERT"};

TOKENSB *tokens_parse(String *origin);
int *replace_reflect_index_to_all(int *reflect_inedxs , int all_length);
int CREATE_exe(TABLE_LIST_NODE *head , TOKENSB *tokensb , int curr);
void XSQL_RUN(TOKENSB *tokensb);
int INSERT_exe(TABLE_LIST_NODE *head,TOKENSB *tokensb , int curr);
int SELECT_exe(TABLE_LIST_NODE *head,TOKENSB *tokensb , int curr);
SELECT_CONDITION *create_selectCondition();
SELECT_CONDITION *extend_selectCondition(SELECT_CONDITION *old);
void free_selectCondition(SELECT_CONDITION *selectCondition);
int* SELECT_exe_SELECT_CONDITION_end_handle(TABLE_LIST_NODE *target_table_node ,SELECT_CONDITION *selectCondition
                                            ,int effective_selectCondition_count , int *where_start_char_effective_count);
WHERE_CONDITION *create_whereCondition();
WHERE_CONDITION *extend_whereCondition(WHERE_CONDITION *old);
void free_whereCondition(WHERE_CONDITION *whereCondition);
WHERE_CONDITION *parse_WHERE_CONDITION(TABLE_LIST_NODE *head,TOKENSB *tokensb
        , int curr ,int *whereCondition_field_count
        ,int *whereCondition_data_count,int *whereCondition_logic_count);


DATALINE_ARRAY *create_DATALINE_ARRAY(TABLE_LIST_NODE *TARGET_TABLE);
void SELECT_exe_DATA_QUERY(TABLE_LIST_NODE *TARGET_TABLE,SELECT_CONDITION *selectCondition
                           , int * reflect_field_index , int select_condition_count, WHERE_CONDITION *whereCondition
                           , int *whereCondition_field_count,int *whereCondition_data_count
                           ,int *whereCondition_logic_count,int *where_start_char_effective_count);
FIELD_INDEXS *get_field_indexs_by_field(TABLE_LIST_NODE *TARGET , String **fields , int field_effective_count);
DATALINE_ARRAY *extend_DATALINE_ARRAY(TABLE_LIST_NODE *TARGET , DATALINE_ARRAY *old);
String *query_upon_down_line_num_calc(TABLE_LIST_NODE*target_table ,DATALINE_ARRAY *dataline_array_t 
    , int *query_field_indexs , int authentic_query_field_num);

#endif //XOKSQLC语言版_GRM_H
