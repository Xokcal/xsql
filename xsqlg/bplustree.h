#include <stdio.h>
#include "../xstr/String.h"
#include "grm.h"
#include "../list/list.h"

#define MAX_KEY_LEN 4

#define STACK_LEN 120

typedef struct BPlusNode{
    int is_leaf;                                        // is leaf node?
    int key_count;                                      // record primary key number.
    int keys[MAX_KEY_LEN];                              // record primary keys's array.
    DATALINE *data[MAX_KEY_LEN];                        // storage datas array.
    struct BPlusNode *children[MAX_KEY_LEN + 1];        // children node.
    struct BPlusNode *next;                             // next node.
}BPlusNode;

typedef struct BPlusTree{
    BPlusNode *bPlusRoot;
}BPlusTree;

BPlusTree*
create_BPlusTree();

BPlusNode*
create_BPlusNode();

BPlusNode*
create_InternalNode();

BPlusNode*
find_BPlusNode(BPlusTree *tree , int primary_key);

void 
insert_BPlusNode(BPlusTree *tree , int key , DATALINE *data);