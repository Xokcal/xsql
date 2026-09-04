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
    INSERT_K
}KEYS;

typedef struct TOKENSB{
    String **tokens;
    int count;
}TOKENSB;

typedef struct SELECT_CONDITION{
    String **content;
    int count;
}SELECT_CONDITION;

static char* sql_keys[19] = {"SELECT" , "CREATE" , "TABLE" , "{" , "}"
                            , "(" , ")" , "COMMENT" , "\'" , "FROM"
                            , "," , "WHERE" , ";" , " " , "*" , "INSERT"
                            ,"STRING" , ":" , "\n"};

static char* primary_keys[5] = {"INSERT" , "SELECT" , "UPDATE" , "CREATE" , "DELETE"};

static char* sql_keys_primary[2] = {"CREATE" , "INSERT"};

TOKENSB *tokens_parse(String *origin);
int CREATE_exe(TABLE_LIST_NODE *head , TOKENSB *tokensb , int curr);
void XSQL_RUN(TOKENSB *tokensb);
int INSERT_exe(TABLE_LIST_NODE *head,TOKENSB *tokensb , int curr);
int SELECT_exe(TABLE_LIST_NODE *head,TOKENSB *tokensb , int curr);
SELECT_CONDITION *create_selectCondition();
SELECT_CONDITION *extend_selectCondition(SELECT_CONDITION *old);
void free_selectCondition(SELECT_CONDITION *selectCondition);

#endif //XOKSQLC语言版_GRM_H
