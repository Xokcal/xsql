#include <stdio.h>
#include "file/file.h"
#include "xstr/String.h"
#include "xsqlg/grm.h"
#include "time/xtime.h"
#include <string.h>
#include "log/xlog.h"
#include "xsqlg/dbt.h"
#include "xsqlg/bplustree.h"

#define XSQL_WEL_STATUE "Welcome use XSQL terminal. \
\nxsql boot is success. \
\ncurrent version is 1.0.1 single service. \
\n\nXSQL is create by Xokcal, it's a individual developer. \
\nhope you enjoy it!!\n" \


void print_tree(BPlusNode* node, int depth) {
    if (node == NULL) return;

    for (int i = 0; i < depth; i++) printf("  ");
    printf("node(is_leaf=%d, key_count=%d, keys=[", node->is_leaf, node->key_count);
    for (int i = 0; i < node->key_count; i++) {
        printf("%d ", node->keys[i]);
    }
    printf("])\n");

    if (!node->is_leaf) {
        for (int i = 0; i <= node->key_count; i++) {
            print_tree(node->children[i], depth + 1);
        }
    }
}

int main() {
    
    /*BPlusTree* tree = create_BPlusTree();

    DATALINE d[20];
    for (int i = 0; i < 20; i++) {
        d[i].DATA = NULL;
    }

    for (int i = 1; i <= 20; i++) {
        printf("=== before insert %d ===\n", i * 10);
        fflush(stdout);
        insert_BPlusNode(tree, i * 10, &d[i - 1]);
        printf("=== after insert %d ===\n", i * 10);
        fflush(stdout);
        print_tree(tree->bPlusRoot, 0);
        printf("\n");
    }

    printf("end!!!\n");*/


    table_db_t *tableDbT = create_tableDbT();
    TABLE_LIST_NODE *head = NULL; //create_TABLE_LIST_NODE(NULL , NULL , 0);
    
    FILE *table_file = get_file_w("db/table.db");
    FILE *data_file = get_file_w("db/data.db");

    /*String *origin = open_file("db/xsql.sql");
    TOKENSB *tokens = tokens_parse(origin);*/
    //XSQL_RUN(tokens , head , table_file , data_file , NULL);
    head = get_snapPhoto("db/xsql.bin");
    printf("%s" , XSQL_WEL_STATUE);

    char input[512];
    String *xsql_input = create_string("");

    while (1) {
        char q_input[500];
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
       }else if(compare(xsql_input , "c exe file;")){
            String *table_file_init = open_file("createexe.xsql");
            TOKENSB *table_token_init = tokens_parse(table_file_init);
            XSQL_RUN(table_token_init  ,  head , table_file , data_file , tableDbT);
            save_snapPhoto("db/xsql.bin" , head);
            delete_all(xsql_input);
       }
       else if(compare(xsql_input , "q")){
            printf("whether input 'q' quit the programma (yes[y] , no[n])\n");
            printf("db @Xokcal ~/m/xsql> ");
            scanf("%s" , &q_input);
            delete_all(xsql_input);
            if(strcmp(q_input , "y") == 0){
                printf("has quit xsql!!");
                break;
            }else continue;
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
