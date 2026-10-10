#include <stdio.h>
#include "../xstr/String.h"
#include "../list/list.h"
#include "bplustree.h"

BPlusTree*
create_BPlusTree()
{
    BPlusTree *bPlusTree = (BPlusTree*)malloc(sizeof(BPlusTree));
    bPlusTree->bPlusRoot = NULL;
    return bPlusTree;
}

BPlusNode*
create_BPlusNode()
{
    BPlusNode *bPlusNode = (BPlusNode*)malloc(sizeof(BPlusNode));
    bPlusNode->is_leaf = 1;
    bPlusNode->key_count = 0;
    bPlusNode->next = NULL;
    return bPlusNode;
}

BPlusNode*
create_InternalNode()
{
    BPlusNode *bPlusNode = (BPlusNode*)malloc(sizeof(BPlusNode));
    bPlusNode->is_leaf = 0;
    bPlusNode->key_count = 0;
    bPlusNode->next = NULL;
    for (int i = 0; i < MAX_KEY_LEN + 1; i++) {
        bPlusNode->children[i] = NULL;
    }
    for (int i = 0; i < MAX_KEY_LEN; i++) {
        bPlusNode->keys[i] = 0;
    }
    return bPlusNode;
}

BPlusNode*
find_BPlusNode(BPlusTree *tree , int primary_key)
{
    if(tree == NULL)return NULL;
    BPlusNode *node = tree->bPlusRoot;
    while(!node->is_leaf){ // is leaf node break!
        int count = 0;
        while(count < node->key_count&&primary_key >= node->keys[count]){
            count++;
        }
        node = node->children[count];
    }

    for(int i = 0; i < node->key_count; i++){
        if(primary_key == node->keys[i]){
            return node;
        }
    }
    return NULL;
}

void 
insert_BPlusNode(BPlusTree *tree , int primary_key , DATALINE *data)
{
    // 还没有根节点
    if(tree->bPlusRoot == NULL){
        BPlusNode *newNode = create_BPlusNode();
        newNode->key_count = 1;
        newNode->data[0] = data;
        newNode->keys[0] = primary_key;
        tree->bPlusRoot = newNode;
        return;
    }

    //有根节点
    BPlusNode *temp = tree->bPlusRoot;
    BPlusNode *stack[100];
    int stack_count = 0;
    while(!temp->is_leaf){
        stack[stack_count++] = temp; // 收集栈
        int i = 0;
        while(i < temp->key_count && primary_key >= temp->keys[i]){
            i++;
        }
        temp = temp->children[i];
    }
    BPlusNode *target_leaf = temp;
    //找到叶子节点，然后插入
    int i = 0;
    while(i < target_leaf->key_count && primary_key >= target_leaf->keys[i]){
        i++;
    }

    for(int j = target_leaf->key_count; j > i ; j--){
        target_leaf->keys[j] = target_leaf->keys[j - 1];
    }
    target_leaf->keys[i] = primary_key;

    for(int j = target_leaf->key_count; j > i ; j--){
        target_leaf->data[j] = target_leaf->data[j - 1];
    }
    target_leaf->data[i] = data; // 插入成功！
    target_leaf->key_count++;

    int revise = 0;
    BPlusNode *new_leaf = NULL;
    BPlusNode *parent = NULL;
    // 超过最大值的情况（分裂）
    if(target_leaf->key_count > MAX_KEY_LEN){
        int mid = target_leaf->key_count / 2;
        new_leaf = create_BPlusNode();
        
        int j = 0;
        for(int i = mid; i < target_leaf->key_count; i++){
            new_leaf->keys[j++] = target_leaf->keys[i];
        }

        int j2 = 0;
        for(int i = mid; i < target_leaf->key_count; i++){
            new_leaf->data[j2++] = target_leaf->data[i];
        }
        new_leaf->key_count = j2;

        target_leaf->key_count = mid;
        new_leaf->next = target_leaf->next;
        target_leaf->next = new_leaf;
        revise = new_leaf->keys[0]; // 把上提值给到revise

        //如果当前分裂的target是根节点，就创建一个新的根节点
        if(target_leaf == tree->bPlusRoot){
            BPlusNode *root = create_InternalNode();
            root->keys[0] = revise;
            root->children[0] = target_leaf;
            root->children[1] = new_leaf;
            root->key_count++;
            tree->bPlusRoot = root;
            return;
        }
        //分裂完成，上传父节点
        parent = stack[--stack_count];
        int i = 0;
        while(i < parent->key_count&& revise >= parent->keys[i]){
            i++;
        }
        // i = 1
        // keys     [10 , 20 , 30]   <--- 20
        // children [C1 , C2 , C3 , C4]  <---- C3
        for(int j = parent->key_count; j > i; j--){
            parent->keys[j] = parent->keys[j - 1];
        }
        for(int j = parent->key_count + 1; j > i; j--){
            parent->children[j] = parent->children[j - 1];
        }
        parent->keys[i] = revise;
        parent->children[i + 1] = new_leaf;
        parent->key_count++;
    }

    while(1){
        if(parent == NULL){
            return;
        }
        //如果父节点超过最大值
        if(parent->key_count > MAX_KEY_LEN){
            int mid = parent->key_count / 2;
            revise = parent->keys[mid]; // 设置revise
            new_leaf = create_InternalNode();
            int j = 0;
            for(int i = mid + 1; i < parent->key_count ; i ++){
                new_leaf->keys[j++] = parent->keys[i];
            }
            int j2 = 0;
            for(int i = mid + 1; i < parent->key_count + 1; i++){
                new_leaf->children[j2++] = parent->children[i];
            }
            target_leaf = parent;
            target_leaf->key_count = mid;
            //如果当前分裂的target是根节点，就创建一个新的根节点
            if(target_leaf == tree->bPlusRoot || stack_count == 0){
                BPlusNode *root = create_InternalNode();
                root->keys[0] = revise;
                root->children[0] = target_leaf;
                root->children[1] = new_leaf;
                root->key_count++;
                tree->bPlusRoot = root;
                return;
            }
            parent = stack[--stack_count];
            //分裂完成，上传父节点
            int i = 0;
            while(i < parent->key_count&& revise >= parent->keys[i]){
                i++;
            }
            for(int j = parent->key_count; j > i; j--){
                parent->keys[j] = parent->keys[j - 1];
            }
            for(int j = parent->key_count + 1; j > i + 1; j--){
                parent->children[j] = parent->children[j - 1];
            }
            parent->keys[i] = revise;
            parent->children[i + 1] = new_leaf;
            parent->key_count++;
        }else break;
    }
}

























































/*void 
insert_BPlusNode(BPlusTree *tree , int key , DATALINE *data)
{
    if(tree->bPlusRoot == NULL){
         BPlusNode *root = create_BPlusNode();
         root->is_leaf = 1;
         root->keys[0] = key;
         root->data[0] = data;
         root->key_count = 1;
         tree->bPlusRoot = root;
         return;
    }
    BPlusNode *stack[STACK_LEN];
    int stack_index = 0;
    BPlusNode *node = tree->bPlusRoot;
    BPlusNode *targetLeaf = NULL;
    while(!node->is_leaf){
        stack[stack_index++] = node;
        int i = 0;
        while(i < node->key_count && key >= node->keys[i]){
            i++;
        }
        node = node->children[i];
    }
    targetLeaf = node;
    int i = 0;
    while(i < targetLeaf->key_count&&key >= targetLeaf->keys[i]){
        i++;
    }
    for(int j = targetLeaf->key_count; j > i ; j--){
        targetLeaf->keys[j] = targetLeaf->keys[j - 1];
        targetLeaf->data[j] = targetLeaf->data[j - 1];
    }
    targetLeaf->data[i] = data;
    targetLeaf->keys[i] = key;
    targetLeaf->key_count++;

    int revise_key;

    BPlusNode *newLeaf = NULL;

    if(targetLeaf->key_count > MAX_KEY_LEN){
        int mid = targetLeaf->key_count / 2;
        newLeaf = create_BPlusNode();
        int new_index = 0;
        for(int i = mid; i < targetLeaf->key_count ; i++){
            newLeaf->keys[new_index] = targetLeaf->keys[i];
            newLeaf->data[new_index] = targetLeaf->data[i];
            new_index++;
        }
        newLeaf->key_count = new_index;
        targetLeaf->key_count = mid;

        newLeaf->next = targetLeaf->next;
        targetLeaf->next = newLeaf;

        revise_key = newLeaf->keys[0];
    }

    if(newLeaf != NULL){ // key_count is over MAX_KEY_LEN! separate.
        while(1){
            if(stack_index == 0){ // not have parent node.
                BPlusNode *parentNode = create_InternalNode();
                parentNode->key_count = 1;
                parentNode->keys[0] = revise_key;
                parentNode->children[0] = targetLeaf;
                parentNode->children[1] = newLeaf;
                tree->bPlusRoot = parentNode;
                return;
            }

            BPlusNode *currNode = stack[--stack_index];

            int p = 0;
            while(p < currNode->key_count && revise_key >= currNode->keys[p]){
                p++;
            }

            for(int i = currNode->key_count; i > p; i--){
                currNode->keys[i] = currNode->keys[i - 1];
            }

            for(int i = currNode->key_count + 1; i > p + 1; i--){
                currNode->children[i] = currNode->children[i - 1];
            }
            currNode->keys[p] = revise_key;
            currNode->children[p + 1] = newLeaf;
            currNode->key_count++;
            // new internal..
            if(currNode->key_count <= MAX_KEY_LEN)return;
            // currNode keys = [10 , 20 , 30] MAX 2;
            int internal_mid = currNode->key_count / 2; // mid = 1;
            revise_key = currNode->keys[internal_mid]; // key[mid] = 20
            BPlusNode *newInternalNode = create_InternalNode();
            
            int newInternalKeyIndex = 0;
            for(int i = internal_mid + 1; i < currNode->key_count ; i++){ // [30]
                newInternalNode->keys[newInternalKeyIndex++] = currNode->keys[i];
            }

            int newInternalChildrenIndex = 0;
            for(int i = internal_mid + 1; i < currNode->key_count + 1; i++){
                newInternalNode->children[newInternalChildrenIndex++] = currNode->children[i];
            }

            newInternalNode->key_count = newInternalKeyIndex;
            currNode->key_count = internal_mid; // mid = 1 , [10 , 20]
            newLeaf = newInternalNode;
            targetLeaf = currNode;
        }
    }
}*/