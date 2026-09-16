#include <stdio.h>
#include "../xstr/String.h"
#include "grm.h"
#include "../list/list.h"


// #define XSQL_DEBUG

#ifdef XSQL_DEBUG 
    #define LOG(statue , str)\
        printf("SHOW_DEBUG[%s] %s\n" , (statue) , (str))
    #define LOG_INT(statue , d)\
        printf("SHOW_DEBUG[%s] %d\n" , (statue) , (d))
#else
    #define LOG(statue , str)
#endif

#define PERSISTENT_LINE_LENGTH 20

#define TABLE_AND_FIELD_TAIL_EXTRA 1;

#define SHOW_PRINT_SEPARATE_VERTICAL_BAR "|"

#define SEPARATE_LINE_PRINT(str)\
        printf("+%s+\n" ,(str))

#define TABLE_NAME_AND_FIELD_PRINT(str , tail)\
        printf("| %s%s\n" ,(str) , (tail))

#define XSQL_LOG(str)\
        printf("xsql : %s\n" , (str))

static int
separateLine_len_calc(TABLE_LIST_NODE *head)
{
    int max = 0;
    TABLE_LIST_NODE *temp = head;
    while(temp != NULL){
        if(temp->table->NAME->length > max)
            max = temp->table->NAME->length;
        temp = temp->next;
    }
    return max;
}

static int
field_Or_TableName_Max_Length(TABLE_LIST_NODE *target_table)
{
    if(target_table == NULL)
    LOG("field_Or_TableName_Max_Length" , "target_table is NULL!!");
    int max = 0;
    for(int i =0; i < target_table->table->length ; i++){
        int curr_len = target_table->table->FIELD[i]->length;
        if(max < curr_len){
            max = curr_len;
            continue;
        }
    }
    if(max < target_table->table->NAME->length){
        max = target_table->table->NAME->length;
    }
    return max;
}

static String*
separate_line_str(TABLE_LIST_NODE *head)
{
    TABLE_LIST_NODE *temp = head;
    int max = separateLine_len_calc(temp);
    /* the 'line' need free after print. */
    String *line = create_string("");
    for(int i = 0; i < max + PERSISTENT_LINE_LENGTH; i++)
        combine_tail(line , "-");
    return line;
}

/* need addition '-' total num is :
additionNum = (max + PERSISTENT_LINE_LENGTH[12]) - currFieldLength - TABLE_AND_FIELD_TAIL_EXTRA */
static String *
tableName_tail_calc(TABLE_LIST_NODE *head , TABLE_LIST_NODE *target_table)
{
    if(target_table == NULL)
    LOG("field_Or_TableName_Max_Length" , "target_table is NULL!!");
    /* 'tail' need free after print */
    String *tail = create_string("");
     /* field or tableName max length */
    int max = field_Or_TableName_Max_Length(target_table);
    int total_len = max + PERSISTENT_LINE_LENGTH;
    int tail_len = total_len - target_table->table->NAME->length - TABLE_AND_FIELD_TAIL_EXTRA;
    for(int i =0; i < tail_len; i++)combine_tail(tail , " ");
    combine_tail(tail , "|");
    return tail;
}

static String *
allTable_Print_Tail_Calc(String *tableName , int total_len)
{
    String *tail = create_string("");
    int extra = total_len - tableName->length;
    for(int i = 0; i < extra - 1 ; i++)combine_tail(tail , " ");
    combine_tail(tail , "|");
    return tail;
}

static void
tableName_Print(TABLE_LIST_NODE *head , int total_len)
{
    TABLE_LIST_NODE *temp = head->next;
    while(temp != NULL){
        String *tableName = temp->table->NAME;
        String *tableName_tail = allTable_Print_Tail_Calc(tableName , total_len);
        TABLE_NAME_AND_FIELD_PRINT(tableName->str , tableName_tail->str);
        string_free(tableName_tail);
        temp = temp->next;
    }
}

static void
allTable_Print(TABLE_LIST_NODE *head , TOKENSB *tokensb)
{
    String *line =  separate_line_str(head);
    SEPARATE_LINE_PRINT(line->str);
    tableName_Print(head , line->length);
    SEPARATE_LINE_PRINT(line->str);
    string_free(line);
}

static String *
field_tail_calc(TABLE_LIST_NODE *head , TABLE_LIST_NODE *target_table , String *field)
{
    /* 'tail' need free after print */
    String *tail = create_string("");
    int max = field_Or_TableName_Max_Length(target_table);
    int total_len = max + PERSISTENT_LINE_LENGTH;
    int tail_len = total_len - field->length - TABLE_AND_FIELD_TAIL_EXTRA;
    for(int i =0; i < tail_len; i++)combine_tail(tail , " ");
    combine_tail(tail , SHOW_PRINT_SEPARATE_VERTICAL_BAR);
    return tail;
}

static void
all_tableField_Print(TABLE_LIST_NODE *head , TABLE_LIST_NODE *target_table)
{
    for(int i = 0; i < target_table->table->length - 1; i++){
        String *field_tail = field_tail_calc(head , target_table , target_table->table->FIELD[i]);
        TABLE_NAME_AND_FIELD_PRINT(target_table->table->FIELD[i]->str ,field_tail->str);
        string_free(field_tail);
    }
}

static String*
tableField_Or_TableName_Line_Calc(TABLE_LIST_NODE *target_table)
{
    String *line = create_string("");
    int max = field_Or_TableName_Max_Length(target_table);
    int total = max + PERSISTENT_LINE_LENGTH;
    for(int i = 0; i < total; i++)combine_tail(line , "-");
    return line;
}

static String*
tableContent_Print(TABLE_LIST_NODE *head , TABLE_LIST_NODE *target_table)
{
    /* the line free after function parse_tableField_core() free. */
    if(target_table == NULL)
    LOG("tableContent_Print" , "target_table is NULL!!");
    String *line = tableField_Or_TableName_Line_Calc(target_table);
    SEPARATE_LINE_PRINT(line->str); // yes
    String *tableName_tail = tableName_tail_calc(head , target_table);
    TABLE_NAME_AND_FIELD_PRINT(target_table->table->NAME->str , tableName_tail->str);
    string_free(tableName_tail);
    SEPARATE_LINE_PRINT(line->str);
    return line;
}

static void
parse_tableField_core(TABLE_LIST_NODE *head , TABLE_LIST_NODE *target_table 
    , int *is_over_tableName , String *tableName)
{
    if(target_table == NULL)
    LOG("parse_tableField_core" , "target_table is NULL!!");
    /* print tableName content. */
    String *line = tableContent_Print(head , target_table);
    /* print all tableField. */
    all_tableField_Print(head , target_table);
    /* print end line */
    SEPARATE_LINE_PRINT(line->str);
    string_free(line);
}

static int
parse_show_reach_table(TABLE_LIST_NODE *head , TOKENSB *tokensb , int *is_over_table , int i)
{
    LOG("reach 'table'!!" , "");
    *is_over_table = 1;
    int is_over_tableName = 0;
    String *tableName = create_string("");
    TABLE_LIST_NODE *target_table;
    for(int j = i + 1; j < tokensb->count ; j++){
        String *token = tokensb->tokens[j];
        LOG("show reach tableName for ->" , token->str);
        // over 'table' primary key
        if(*is_over_table&&!compare(token , " ")&&!compare(token , ";")){
            target_table = get_TABLE_LIST_NODE(head , token->str);
            is_over_tableName = 1;
            continue;
        }
        if(!is_over_tableName&&!compare(token , " ")&&compare(token , ";")){
            XSQL_LOG("show table is empty!");
            return j;
        }
        if(!compare(token , " ")&&is_over_tableName&&compare(token , ";")){
            LOG("show reach tableName core func! ->" , token->str);
            parse_tableField_core(head, target_table , &is_over_tableName , token);
        }
    }
    if(i != tokensb->count - 1)return i + 1;return i;
}

int 
Xsql_ShowExe(TABLE_LIST_NODE *head,TOKENSB *tokensb , int curr)
{
    int is_over_table = 0;
    int is_over_show = 0;
    for(int i = 0 ; i < tokensb->count ; i ++){
        String *token = tokensb->tokens[i];
        LOG("show token ->" , token->str);
        // is 'table' condition
        if(is_over_show&&is_over_table){ // tableName
            LOG("show entre tableName" , "");
            i = parse_show_reach_table(head , tokensb , &is_over_table , i);
            continue;
        }
        if(compare(token , " ")){
            continue;
        }
        if(!is_over_table&&compare(token , "show")){
            LOG("show entre show" , "");
            is_over_show = 1;
            continue;
        }
        if(!is_over_table&&compare(token , "table")){
            LOG("show entre table" , "");
            is_over_table = 1;
            continue;
        }
        // is 'tables' condition
        if(is_over_show&&!is_over_table&&compare(token , "tables")){
            allTable_Print(head , tokensb);
            return i;
        }
        // ...
    }
    return curr + 1;
}