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
#include "utils.h"

/*
 create table user(
    user_id string comment 'user_id',
    user_name string comment 'user_name',
    salary string comment 'salary'
)comment 'user'; 
*/

static int
table_field_count(TABLE_LIST_NODE *head ,TOKENSB *t , int curr)
{
    int count = 0;
    for(int i = curr ; i < t->count ; i++){
        String * token = t->tokens[i];
        if(compare(token , ",")){
            count++;
            continue;
        }
        if(compare(token  , ")")){
            count++;
            continue;
        }
    }
}



static void
collect_table_content(TABLE_LIST_NODE *head ,TOKENSB *t , int curr)
{
    int field_count = table_field_count(head , t , curr);
    String **field = (String **)malloc(field_count * sizeof(String *));
    for(int i = curr ; i < t->count ; i++){
        String * token = t->tokens[i];
        
    }
}

int 
XsqlCreate_exe(TABLE_LIST_NODE *head,TOKENSB *t , int curr,table_db_t *tableDbT)
{
    int is_over_table_key = 0;
    int is_over_table = 0;
    int is_entre_main_content = 0;
    String *table_name = create_string("");
    for(int i = curr ; i < t->count ; i++){
        String * token = t->tokens[i];
        if(is_entre_main_content&&compare(token , ")")){
            return i;
        }
        if(!is_over_table_key && compare(token , "table")){
            is_over_table_key = 1;continue;
        }
        //table name
        if(is_over_table_key&&!compare(token , " ")
        &&!IS_CONTAIN_KEYS(token->str)&&!is_over_table){
            combine_tail(table_name , token->str);
            is_over_table = 1;continue;
        }
        if(!is_entre_main_content&&compare(token , "(")){
            is_entre_main_content = 1;continue;
        }
        if(is_entre_main_content){

        }
    }
}
