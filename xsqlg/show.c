#include <stdio.h>
#include "../xstr/String.h"
#include "grm.h"
#include "../list/list.h"

#define PERSISTENT_LINE_LENGTH 6

#define XSQL_PUON_LINE_TERMINAL(str)\
        printf("+%s+\n" ,(str))

#define XSQL_TABLE_NAME_TERMINAL(str)\
        printf("%s\n" ,(str))

#define XSQL_LOG(str)\
        printf("xsql : %s\n" , (str))

static int
table_name_max_length_calc(TABLE_LIST_NODE *head)
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

static void
show_all_table_upon_line(TABLE_LIST_NODE *head)
{
    TABLE_LIST_NODE *temp = head;
    int max = table_name_max_length_calc(temp);
    String *line = create_string("");
    for(int i = 0; i < max + PERSISTENT_LINE_LENGTH; i++){
        combine_tail(line , "-");
    }
    XSQL_PUON_LINE_TERMINAL(line->str);
    string_free(line);
}

static void
show_all_name_terminal(TABLE_LIST_NODE *head)
{
    TABLE_LIST_NODE *temp = head;
    while(temp != NULL){
        XSQL_TABLE_NAME_TERMINAL(temp->table->NAME->str);
        temp = temp->next;
    }
}

static void
parse_show_select_all_table(TABLE_LIST_NODE *head , TOKENSB *tokensb)
{
    TABLE_LIST_NODE *temp = head->next;
    show_all_table_upon_line(head);
    show_all_name_terminal(head);
    show_all_table_upon_line(head);
}

static void
parse_show_reach_table(TABLE_LIST_NODE *head , TOKENSB *tokensb , int *is_over_table , int i)
{
    *is_over_table = 1;
    int is_reach_table_behind = 0;
    for(int j = i + 1; j < tokensb->count ; j++){
        String *token = tokensb->tokens[j];
        // over 'table' primary key
        if(!compare(token , " ")&&!is_reach_table_behind){
            if(compare(token , ";")){
                XSQL_LOG("show table is empty!");
                return;
            }
        }
    }
}

// show table;
// show table employees;
int 
SHOW_exe(TABLE_LIST_NODE *head,TOKENSB *tokensb , int curr)
{
    int is_over_table = 0;
    for(int i = 0 ; i < tokensb->count ; i ++){
        String *token = tokensb->tokens[i];
        // is 'tables' condition
        if(!is_over_table&&compare(token , "tables")){
            parse_show_select_all_table(head , tokensb);
            return i;
        }
        // is 'table' condition
        if(!is_over_table&&compare(token , "table")){
            parse_show_reach_table(head , tokensb , &is_over_table , i);
            continue;
        }
        // ...
    }
    return curr + 1;
}