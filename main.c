#include <stdio.h>
#include "file/file.h"
#include "xstr/String.h"
#include "xsqlg/grm.h"
#include "time/xtime.h"
#include <string.h>
#include "log/xlog.h"
#include "xsqlg/dbt.h"

#define XSQL_WEL_STATUE "Welcome use XSQL terminal. \
\nxsql boot is success. \
\ncurrent version is 1.0.1 single service. \
\n\nXSQL is create by Xokcal, it's a individual developer. \
\nhope you enjoy it!!\n" \

int main() {
    table_db_t *tableDbT = create_tableDbT();
    TABLE_LIST_NODE *head = NULL;
    
    FILE *table_file = get_file_w("db/table.db");
    FILE *data_file = get_file_w("db/data.db");

    head = get_snapPhoto("db/xsql.bin");

    printf("%s" , XSQL_WEL_STATUE);

    char input[512];
    String *xsql_input = create_string("");

    while (1) {

        printf("db @Xokcal ~/m/xsql> ");
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
       }else{
            XSQL_RUN(tokensb_input  ,  head , table_file , data_file , tableDbT);
            save_snapPhoto("db/xsql.bin" , head);
            delete_all(xsql_input);
       }
    }
    return 0;
}
