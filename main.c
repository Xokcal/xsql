#include <stdio.h>
#include "file/file.h"
#include "xstr/String.h"
#include "xsqlg/grm.h"
#include "time/xtime.h"
#include <string.h>
#include "log/xlog.h"
#include "xsqlg/dbt.h"
#include "xsqlg/bplustree.h"

#define XSQL_LOGO_ASCLL \
"\n __  __   _____   ____    __ \
 \n \\ \\/ /  / ___/  / __ \\  / / \
 \n  \\  /   \\__ \\  / / / / / / \
 \n  /  \\  ___/ / / /_/ / / /  \
 \n /_/\\_\\/____/  \\___\\_\\/_/ \n\n"

#define XSQL_WEL_STATUE \
"Welcome use XSQL terminal. \
\nxsql boot is success. \
\n\nXSQL is create by Xokcal, it's a individual developer. \
\nhope you enjoy it!! \
\nneed help can input 'h', show help instruction\n"

#define XSQL_HELP_TXT \
"\n*************** xsql help txt.***************\
\nrun createexe.txt xsql          'c exe file;'. \
\nrun exe.txt xsql                'exe file;'. \
\nquit the xsql terminal          'q'. \
\n**********************************************\n"

int main() {
    table_db_t *tableDbT = create_tableDbT();
    TABLE_LIST_NODE *head = NULL; //create_TABLE_LIST_NODE(NULL , NULL , 0);
    
    FILE *table_file = get_file_w("db/table.db");
    FILE *data_file = get_file_w("db/data.db");

    /*String *origin = open_file("db/xsql.sql");
    TOKENSB *tokens = tokens_parse(origin);*/
    //XSQL_RUN(tokens , head , table_file , data_file , NULL);
    head = get_snapPhoto("db/xsql.bin");
    printf("XSQL v1.0.1 (build %s %s)\n", __DATE__, __TIME__);
    printf("Platform: %s %s\n", "Windows", "x86_64");
    printf("Compiler: GCC %d.%d.%d\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
    printf("%s%s" , XSQL_LOGO_ASCLL , XSQL_WEL_STATUE);

    char input[512];
    String *xsql_input = create_string("");
    xsqlstart:
    while (1) {
        
        char q_input[500];
        printf("\033[38;5;28mdb @%s ~/m/xsql>\033[0m " , __FILE__);
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break; // Ctrl+Z 退出
        }

        // 去掉末尾换行
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0) {
            break;
        }

        if (strlen(input) == 0) {
            continue;
        }
        char *xsql_char = input;
        combine_tail(xsql_input , xsql_char);
        TOKENSB *tokensb_input = tokens_parse(xsql_input);
       if(compare(xsql_input , "exe file;")){
            String *table_file_init = open_file("exe.xsql");
            TOKENSB *table_token_init = tokens_parse(table_file_init);
            XSQL_RUN(table_token_init  ,  head , table_file , data_file , tableDbT);
            save_snapPhoto("db/xsql.bin" , head);
            delete_all(xsql_input);
       }else if(compare(xsql_input , "c exe file;")){
            String *table_file_init = open_file("createexe.xsql");
            TOKENSB *table_token_init = tokens_parse(table_file_init);
            XSQL_RUN(table_token_init  ,  head , table_file , data_file , tableDbT);
            save_snapPhoto("db/xsql.bin" , head);
            delete_all(xsql_input);
       }
       else if(compare(xsql_input , "h")){
            printf("%s" , XSQL_HELP_TXT);
            delete_all(xsql_input);
            continue;
       }
       else if(compare(xsql_input , "q")){
            printf("whether input 'q' quit the programma (yes[y] , no[n])\n");
            printf("\033[38;5;28mdb @%s ~/m/xsql>\033[0m " , __FILE__);
            scanf("%s" , &q_input);
            delete_all(xsql_input);
            if(strcmp(q_input , "y") == 0){
                printf("has quit xsql!!");
                break;
            }
       }
       else{
            XSQL_RUN(tokensb_input  ,  head , table_file , data_file , tableDbT);
            //combine_tail(origin , xsql_char);
            //print_open("db/xsql.sql" , origin->str);
            save_snapPhoto("db/xsql.bin" , head);
            delete_all(xsql_input);
       }
    }
    return 0;
}
