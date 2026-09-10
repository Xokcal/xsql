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
\ncongratulate you use nice!\n" \

int main() {
    //开机，初始化储存表
    String *table_file_init = open_file("db/table.db");
    string_println(table_file_init);
    TOKENSB *table_token_init = tokens_parse(table_file_init);

    String *insert_file_init = open_file("db/insert.db");
    string_println(insert_file_init);
    TOKENSB *insert_token_init = tokens_parse(insert_file_init);

    table_db_t *tableDbT = create_tableDbT();
    table_db_t *insertDbT = create_tableDbT();
    TABLE_LIST_NODE *head = create_TABLE_LIST_NODE(create_string("XSQL") , NULL , 0);
    FILE *table_file = get_file_w("db/table.db");
    FILE *data_file = get_file_w("db/data.db");
    XSQL_RUN(table_token_init , head , table_file , data_file ,tableDbT , insertDbT);
    XSQL_RUN(insert_token_init , head , table_file , data_file ,tableDbT , insertDbT);

    //全部获取，并且重新存入文件
    String *all_table_data = create_string("");
    String *all_insert_data = create_string("");
    for(int i = 0 ; i < *tableDbT->auth_count ; i++)
        combine_tail(all_table_data , tableDbT->table_db_str[i]->str);
    print_open("db/table.db" , all_table_data->str);
    delete_all(all_table_data);

    copy_string(insert_file_init , all_insert_data);
    print_open("db/insert.db" , all_insert_data->str);

    printf("%s" , XSQL_WEL_STATUE);

    char input[512];
    String *xsql_input = create_string("");

    while (1) {
        printf("xsql> ");
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

        // 这里把 input 交给你的 XSQL 解析执行
        //printf("you input> %s\n", input);
        char *xsql_char = input;
        combine_tail(xsql_input , xsql_char);
        TOKENSB *tokensb_input = tokens_parse(xsql_input);
        XSQL_RUN(tokensb_input  ,  head , table_file , data_file , tableDbT ,insertDbT);
        for(int i = 0 ; i < *tableDbT->auth_count ; i++)
            combine_tail(all_table_data , tableDbT->table_db_str[i]->str);
        print_open("db/table.db" , all_table_data->str);
        delete_all(all_table_data);
        combine_tail(all_insert_data , xsql_input->str);
        print_open("db/insert.db" , all_insert_data->str);
        delete_all(xsql_input);
    }
    return 0;
}
