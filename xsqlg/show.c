#include <stdio.h>
#include "../xstr/String.h"
#include "grm.h"
#include "../list/list.h"


#define XSQL_DEBUG

#ifdef XSQL_DEBUG 
    #define LOG(statue , str)\
        printf("SHOW_DEBUG[%s] %s\n" , (statue) , (str))
    #define LOG_INT(statue , d)\
        printf("SHOW_DEBUG[%s] %d\n" , (statue) , (d))
#else
    #define LOG(statue , str)
#endif

#define PERSISTENT_LINE_LENGTH 6

#define SEPARATE_LINE_PRINT(str)\
        printf("+%s+\n" ,(str))

#define TABLE_NAME_PRINT(str)\
        printf("%s\n" ,(str))

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

static String*
separate_line_str(TABLE_LIST_NODE *head)
{
    TABLE_LIST_NODE *temp = head;
    int max = separateLine_len_calc(temp);
    /* the 'line' need free when behind print. */
    String *line = create_string("");
    for(int i = 0; i < max + PERSISTENT_LINE_LENGTH; i++)
        combine_tail(line , "-");
    return line;
}

static void
tableName_Print(TABLE_LIST_NODE *head)
{
    TABLE_LIST_NODE *temp = head;
    while(temp != NULL){
        TABLE_NAME_PRINT(temp->table->NAME->str);
        temp = temp->next;
    }
}

static void
allTable_Print(TABLE_LIST_NODE *head , TOKENSB *tokensb)
{
    TABLE_LIST_NODE *temp = head->next;
    String *line =  separate_line_str(head);
    SEPARATE_LINE_PRINT(line->str);
    tableName_Print(head);
    SEPARATE_LINE_PRINT(line->str);
    string_free(line);
}

static void
tableField_Print()
{
    
}

static void
parse_tableField_core(TABLE_LIST_NODE *head , TABLE_LIST_NODE **target_table 
    , int *is_over_tableName , String *tableName)
{
    *target_table =  get_TABLE_LIST_NODE(head , tableName);

}

static int
parse_show_reach_table(TABLE_LIST_NODE *head , TOKENSB *tokensb , int *is_over_table , int i)
{
    LOG("reach 'table'!!" , "");
    *is_over_table = 1;
    int is_reach_table_behind = 0;
    int is_over_tableName = 0;
    TABLE_LIST_NODE *target_table;
    for(int j = i + 1; j < tokensb->count ; j++){
        String *token = tokensb->tokens[j];
        // over 'table' primary key
        if(!compare(token , " ")&&!is_reach_table_behind){
            if(compare(token , ";")){
                XSQL_LOG("show table is empty!");
                return;
            }
            is_reach_table_behind = 1;
            continue;
        }
        if(is_reach_table_behind&&!compare(token , " ")){
            parse_tableField_core(head, &target_table , &is_over_tableName , token);
        }
    }
}

int 
Xsql_ShowExe(TABLE_LIST_NODE *head,TOKENSB *tokensb , int curr)
{
    int is_over_table = 0;
    for(int i = 0 ; i < tokensb->count ; i ++){
        String *token = tokensb->tokens[i];
        if(compare(token , " ")){
            continue;
        }
        if(!is_over_table&&compare(token , "show")){
            is_over_table = 1;
            continue;
        }
        // is 'tables' condition
        if(is_over_table&&compare(token , "tables")){
            allTable_Print(head , tokensb);
            return i;
        }
        // is 'table' condition
        if(!is_over_table&&compare(token , "table")){
            i = parse_show_reach_table(head , tokensb , &is_over_table , i);
            continue;
        }
        // ...
    }
    return curr + 1;
}