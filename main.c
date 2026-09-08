#include <stdio.h>
#include "file/file.h"
#include "xstr/String.h"
#include "xsqlg/grm.h"
#include "time/xtime.h"
#include <string.h>

typedef struct ARPMessage{
    int arp_req_arr;
}ARPMessage;

int main() {
    TABLE_LIST_NODE *head = create_TABLE_LIST_NODE(create_string("XSQL") , NULL , 0);

    String* origin = open_file("xoksql/test1.xsql");
    //string_println(origin);
    TOKENSB *tokensb = tokens_parse(origin);
    XSQL_RUN(tokensb , head);

    /*TABLE_LIST_NODE *head = create_TABLE_LIST_NODE(create_string("name1") , NULL , 10);
    TABLE_LIST_NODE *node1 = create_TABLE_LIST_NODE(create_string("name2") , NULL , 11);
    add_TABLE_LIST_NODE(head , node1);
    TABLE_LIST_NODE *node_r = get_TABLE_LIST_NODE(head , "name2");
    ARPMessage arp_message;
    arp_message.arp_req_arr = 10;
    printf("%d\n" , arp_message.arp_req_arr);*/


    char input[512];
    String *xsql_input = create_string("");

    while (1) {
        printf("XSQL> ");
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
        printf("you input> %s\n", input);
        char *xsql_char = input;
        combine_tail(xsql_input , xsql_char);
        TOKENSB *tokensb_input = tokens_parse(xsql_input);
        XSQL_RUN(tokensb_input , head);
        delete_all(xsql_input);
    }
    return 0;
}
