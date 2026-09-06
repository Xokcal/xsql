#include <stdio.h>
#include "file/file.h"
#include "xstr/String.h"
#include "xsqlg/grm.h"
#include "time/xtime.h"
int main() {
    String* origin = open_file("xoksql/exe.xsql");
    //string_println(origin);
    TOKENSB *tokensb = tokens_parse(origin);
    XSQL_RUN(tokensb);

    TABLE_LIST_NODE *head = create_TABLE_LIST_NODE(create_string("name1") , NULL , 10);
    TABLE_LIST_NODE *node1 = create_TABLE_LIST_NODE(create_string("name2") , NULL , 11);
    add_TABLE_LIST_NODE(head , node1);
    TABLE_LIST_NODE *node_r = get_TABLE_LIST_NODE(head , "name2");
    return 0;
}
