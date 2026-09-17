#include <stdio.h>
#include "../xstr/String.h"
#include <stdlib.h>
#include "../list/list.h"
#include "grm.h"
#include "utils.h"

#define XSQL_DEBUG

#ifdef XSQL_DEBUG
    #define LOG(statue , str)\
        printf("DELETE_DEBUG[%s] %s\n" , (statue) , (str))
    #define LOG_INT(statue , d)\
        printf("DELETE_DEBUG[%s] %d\n" , (statue) , (d))
#else
    #define LOG(statue , str)
#endif

/*                 {  [id , name , address ,| school , tuition]
 XsqlWhrereCondi = {  [2 , zhang , woMSqpl ,| Xinya , 17600]
                   {  [and , and , or , and ]
*/

static void
Where_field_Match_idex_Parse(XsqlWhereCondi *xsqlWhereCondi 
    , TABLE_LIST_NODE *target_table , int *where_filed_match_indexs)
{
    int index = 0;
    for(int i = 0; i < xsqlWhereCondi->field_count ; i++){
        for(int j = 0 ; j < target_table->table->length - 1 ; j++){
            String *table_field = target_table->table->FIELD[j];
            if(compare(xsqlWhereCondi->field[i] , table_field))
                where_filed_match_indexs[index++] = j;
        }
    }
}

static int
Is_Limit_Data_Match(XsqlWhereCondi *xsqlWhereCondi  , int begin_index 
    , int or_index , int *where_field_match_indexs , DATALINE_NODE *temp)
{
    for(int i = begin_index; i <= or_index ; i++){
        String *table_data = temp->dataline->DATA[where_field_match_indexs[i]];
        if(!compare(xsqlWhereCondi->data[i] , table_data->str))return 0;
    }
    return 1;
}

static void 
Select_Limit_Field_And_Match_Func(XsqlWhereCondi *xsqlWhereCondi , DATALINE_ARRAY *datalineArray 
    , TABLE_LIST_NODE *target_table , int begin_index , int or_index 
    , int *where_field_match_indexs , int *array_count)
{
    DATALINE_NODE *temp = target_table->table->dataline_head;
    while(temp != NULL){
        if(Is_Limit_Data_Match(xsqlWhereCondi , begin_index 
            , or_index , where_field_match_indexs  ,temp)){
            datalineArray->datalines[(*array_count)++] = temp->dataline;
        }
        temp = temp->next;
    }
}

static void
Where_Logic_Parse(TABLE_LIST_NODE *target_table , XsqlWhereCondi *xsqlWhereCondi)
{
    int *array_count = (int*) malloc(xsqlWhereCondi->field_count * sizeof(int));
    int *where_field_match_indexs = (int*) malloc(xsqlWhereCondi->field_count * sizeof(int));
    Where_field_Match_idex_Parse(xsqlWhereCondi , target_table , where_field_match_indexs);
    DATALINE_ARRAY *datalineArray = create_DATALINE_ARRAY(target_table);

    // id = 'e001' and name = 'wangwu' and address = 'isqwonqiucn';
    int begin_index = 0 , is_where_have_or_logic = 0;
    for(int i = 0; i < xsqlWhereCondi->logic_count ; i ++){
        // if -> 'or' is last one ...
        if(compare(xsqlWhereCondi->logic[i] , "or")){
            is_where_have_or_logic = 1;
            Select_Limit_Field_And_Match_Func(xsqlWhereCondi, datalineArray
            , target_table , begin_index , i , where_field_match_indexs , array_count);
            begin_index = i + 1;
        }
    }
    if(is_where_have_or_logic); // where don't have logic 'or' ...
}

//  id = 'e001' and name = 'zhangsan' or name = 'xasx' or pps = 'sxas21' and io_id = '1xq';
static int
Where_Parse(TABLE_LIST_NODE *head,TOKENSB *tokensb , int curr 
    , TABLE_LIST_NODE *target_table ,XsqlWhereCondi *xsqlWhereCondi)
{
    int is_over_quote = 0;
    int is_over_field = 0;
    String *temp = create_string("");
    for(int j = curr ; j < tokensb->count ; j++){
        String *token = tokensb->tokens[j];
        if(!is_over_quote&&!is_over_field&&compare(token , " ")&&!compare(token , "=")){ // fieldName
            xsqlWhereCondi->field[xsqlWhereCondi->field_count++] = token;
            is_over_field = 1;
            continue;
        }
        if(is_over_field&&!is_over_quote&&compare(token , "\'")){ // \'
            is_over_quote = 1;
            continue;
        }
        if(is_over_quote&&compare(token , "\'")){ // storage data
            combine_tail(xsqlWhereCondi->data[xsqlWhereCondi->data_count++] , temp->str);
            is_over_quote = 0;
            continue;
        }
        if(is_over_field&&is_over_quote){ // data
            combine_tail(temp , token->str);
            continue;
        }
        if(is_over_field&&!is_over_quote&&compare(token, "and")){ // and
            combine_tail(xsqlWhereCondi->logic[xsqlWhereCondi->logic_count++] , token->str);
            is_over_field = 0;
            continue;
        }
    }
}

static int
is_reach_tableName(String *token ,int is_over_tableName , int is_over_where)
{
    if(is_over_tableName)return 0;if(is_over_where)return 0;
    if(compare(token , " "))return 0;if(IS_CONTAIN_KEYS(token->str))return 0;
    return 1;
}

// delete tableName where id = 'e001' and name = 'zhangsan';
// delete tableName;
int
Xsql_DeleteExe(TABLE_LIST_NODE *head,TOKENSB *tokensb , int curr)
{
    int is_over_tableName = 0;
    int is_over_where = 0;
    XsqlWhereCondi *xsqlWhereCondi = create_XsqlWhereCondi();
    String *tableName = create_string("");
    TABLE_LIST_NODE *target_table;
    for(int i = 0; i < tokensb->count ; i++){
        String *token = tokensb->tokens[i];
        if(is_over_tableName&&!is_over_where&&compare(token , ";")){ // delete all
            //i = Delete_All_Dataline();
            continue;
        }
        if(is_reach_tableName(token , is_over_tableName , is_over_where)){ // tableName
            copy_string(token , tableName);
            target_table = get_TABLE_LIST_NODE(head , tableName->str);
            is_over_tableName = 1;
            continue;
        }
        if(is_over_tableName&&!is_over_where&&compare(token , "where")){ // where
            is_over_where = 1;
            continue;
        }
        if(is_over_where){ // parse
            Where_Parse(head , tokensb , i , target_table , xsqlWhereCondi);
            continue;
        }
        if(is_over_tableName&&is_over_where&&compare(token , ";")){
            // core

        }
    }
}