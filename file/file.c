//
// Created by 26432 on 2026/8/30.
//
#include <stdio.h>
#include <stdlib.h>
#include "../xstr/String.h"
#include "../list/list.h"
#include "file.h"
#include "../xsqlg/grm.h"

#define ENTER_LIMIT_COUNT 50

String *open_file(char* URL){
    FILE *file = fopen(URL , "r");
    if(file == NULL){
        return NULL;
    }
    String *tokens = create_string("");
    char buf[2];
    while(fgets(buf , sizeof(buf) , file))
        combine_tail_char(tokens , buf[0]);
    fclose(file);
    return tokens;
}

void print_open(char *URL , char *content){
    FILE *file = fopen(URL , "w");
    if(!file){
        log_error_string("fopen error!");
        return;
    }
    fprintf(file , "%s" , content);
    fclose(file);
    return;
}

FILE *get_file_w(char *URL){
    FILE *file = fopen(URL , "w");
    if(!file){
        log_error_string("fopen error!");
        return NULL;
    }
    return file;
}

static void encrypt(String *s){
    for(int i = 0 ; i< s->length ; i++){
        s->str[i] = s->str[i] + 1314;
    }
}

static void decrypt(String *s){
    for(int i = 0 ; i< s->length ; i++){
        s->str[i] = s->str[i] - 1314;
    }
}

void save_snapPhoto(char *URL , TABLE_LIST_NODE *head){
    FILE *fp = fopen(URL , "wb");
    if(!fp){LOG("ERROR" , "snapPhoto open file is null");return;}
    TABLE_LIST_NODE *temp = head->next;
    // ---- table count
    int table_len = 0;
    while (temp != NULL){
        table_len++;
        temp = temp->next;
    }
    printf("table_len] %d\n" , table_len);
    fwrite(&table_len , sizeof(int) , 1 , fp);
    temp = head->next;
    while(temp != NULL){
        // ---- table name
        String *table_name = create_string(temp->table->NAME->str);
        fwrite(&table_name->length , sizeof(int) , 1 , fp);
        printf("[save table name] %s\n" , table_name->str);
        //encrypt(table_name);
        fwrite(table_name->str , sizeof(char) , table_name->length , fp);
        string_free(table_name);
        // ---- table field length
        int table_length = temp->table->length;
        fwrite(&table_length , sizeof(int) , 1 , fp);
        // ---- table FIELD[]
        for(int i = 0 ; i < temp->table->length ; i++){
            String *table_field = create_string(temp->table->FIELD[i]->str);
            fwrite(&table_field->length , sizeof(int) , 1 , fp);
            //encrypt(table_field);
            fwrite(table_field->str , sizeof(char) , table_field->length , fp);
            string_free(table_field);
        }

        // ---- table datalines count
        int datalines_count = 0;
        DATALINE_NODE *dataline_temp = temp->table->dataline_head;
        while (dataline_temp != NULL){
            datalines_count++;
            dataline_temp = dataline_temp->next;
        }
        dataline_temp = temp->table->dataline_head;
        fwrite(&datalines_count , sizeof(int) , 1 , fp);
        printf("[datalines_count] %d\n" , datalines_count);
        // ---- datalines
        printf("EN datalines--------------\n");
        int space_count = 0;
        while (dataline_temp != NULL){
            int auth_FIELD_count = 0;
            for(int j = 0; j < temp->table->length;j++){
                if(!compare(dataline_temp->dataline->DATA[j] , "")){
                    auth_FIELD_count++;
                    continue;
                }
            }
            printf("[auth_FIELD_count] %d\n" , auth_FIELD_count);
            fwrite(&auth_FIELD_count , sizeof(int) , 1 , fp);
            for(int i = 0; i < temp->table->length ; i++){
                /*if(space_count == ENTER_LIMIT_COUNT){
                    char space = '\n';
                    fwrite(&space , sizeof(char) , 1 , fp);
                    space_count = 0;
                }*/
                if(!compare(dataline_temp->dataline->DATA[i] , "")){
                    // ---- DATA[] -> i
                    int DATA_i = i;
                    fwrite(&DATA_i , sizeof(int) , 1 , fp);
                    printf("[DATA_i]%d\n" , DATA_i);
                    String *dataline_str = create_string(dataline_temp->dataline->DATA[i]->str);
                    fwrite(&dataline_str->length , sizeof(int) , 1 , fp);
                    printf("[dataline_str_length] %d\n" , dataline_str->length);
                    //encrypt(dataline_str);
                    fwrite(dataline_str->str , sizeof(char) , dataline_str->length , fp);
                    printf("[save dataline name] %s\n" , dataline_str->str);
                    string_free(dataline_str);
                    //space_count++;
                }
            }
            dataline_temp = dataline_temp->next;
        }
        temp = temp->next;
    }
    fclose(fp);
    return;
}

TABLE_LIST_NODE *get_snapPhoto(char *URL){
    FILE *fp = fopen(URL , "rb");
    if(!fp){LOG("ERROR" , "snapPhoto open file is null");return NULL;}
    TABLE_LIST_NODE *head = create_TABLE_LIST_NODE(create_string("XSQL") , NULL , 0);
    TABLE_LIST_NODE *temp_table = head;
    // ---- table count
    int table_count = 0;
    fread(&table_count , sizeof(int) , 1 , fp);
    printf("[table count-----] %d\n" , table_count);
    for(int i = 0 ; i < table_count ; i++){
        // ---- table name
        int name_len = 0;
        fread(&name_len , sizeof(int) , 1 , fp);
        printf("name len] %d \n" , name_len);
        String *name = create_string("");
        char *temp_name = malloc(name_len * sizeof(char) + 1);
        fread(temp_name , sizeof(char) , name_len , fp);
        temp_name[name_len] = '\0';
        combine_tail(name , temp_name);
        free(temp_name);
        printf("[name] %s\n" , name->str);
        // ---- table length
        int table_field_length = 0;
        fread(&table_field_length , sizeof(int) , 1 , fp);
        printf("[tablel_field_length]%d\n" , table_field_length);
        // ---- table FIELD[]
        int field_count = 0;
        String **FIELDS = malloc(table_field_length * sizeof(String*)); // FIELD
        for(int i = 0; i < table_field_length ; i++)
            FIELDS[i] = create_string("");
        for(int j = 0 ; j < table_field_length ; j++){
            int field_str_len = 0;
            fread(&field_str_len , sizeof(int) , 1 , fp);
            String *field_str = create_string("");
            char *temp_field_str = malloc(field_str_len * sizeof(char) + 1);
            fread(temp_field_str , sizeof(char) , field_str_len , fp);
            temp_field_str[field_str_len] = '\0';
            combine_tail(field_str , temp_field_str);
            printf("[field name] %s\n" , field_str->str);
            free(temp_field_str);
            FIELDS[field_count++] = field_str;
        }
        // ---- table datalines count
        printf("save EN datalines--------------\n");
        int dataline_count = 0;
        fread(&dataline_count , sizeof(int) , 1 , fp);
        printf("[dataline count] %d\n" , dataline_count); // 65
        // ---- create table node
        TABLE_LIST_NODE *new_table_node = create_TABLE_LIST_NODE(name , FIELDS, table_field_length);
        add_TABLE_LIST_NODE(head , new_table_node);
        temp_table = new_table_node;
        // ---- datalines 开始获取datalines
        for(int j = 0; j < dataline_count ; j++){
            printf("every dataline ----------------------------\n");
            // every dataline DATA auth count
            int auth_FIELD_count = 0;
            fread(&auth_FIELD_count , sizeof(int) , 1 , fp);
            String **DATA = (String**)malloc(table_field_length * sizeof(String*)); // 15
            for(int data_i = 0; data_i < table_field_length ; data_i++)
                DATA[data_i] = create_string("");
            int *indexs = (int *)malloc(auth_FIELD_count * sizeof(int));
            int auth_indexs_count = 0;
            printf("[auth_FIELD_count---] %d\n" , auth_FIELD_count);
            for(int k = 0; k < auth_FIELD_count ; k++){
                // auth DATA[i]  -> i
                int auth_DATA_i = 0;
                fread(&auth_DATA_i , sizeof(int) , 1 , fp);
                printf("[save auth_DATA_i] %d\n" , auth_DATA_i);
                indexs[auth_indexs_count++] = auth_DATA_i;
                // data length
                int data_length = 0;
                fread(&data_length , sizeof(int) , 1 , fp);
                printf("[data_length] %d\n" , data_length);
                //data str
                String *data_str = create_string("");
                char *temp_data_str = malloc(data_length * sizeof(char) + 1);
                fread(temp_data_str , sizeof(char) , data_length , fp);
                temp_data_str[data_length] = '\0';
                combine_tail(data_str , temp_data_str);
                printf("[data_str] %s\n" , data_str->str);
                copy_string(data_str , DATA[k]);
                free(temp_data_str);
            }
            DATALINE_NODE *new_dataline_node = create_DATALINE_NODE(DATA , indexs , auth_FIELD_count , table_field_length);
            add_DATALINE_NODE(&temp_table->table->dataline_head , new_dataline_node);
            printf("--------------------------------}  %s\n" , new_dataline_node->dataline->DATA[1]->str);
        }
    }
    fclose(fp);
    printf("[-1][test] %s\n" , head->next->table->NAME->str);
    printf("[1][test] %s\n" , head->next->table->FIELD[1]->str);   // -------这里报错
    printf("[1][test] %s\n" , head->next->table->dataline_head->next->dataline->DATA[1]->str);
    printf("[2][test] %s\n" , head->next->table->dataline_head->dataline->DATA[0]->str);
    printf("[4][test] %s\n" , head->next->table->dataline_head->dataline->DATA[1]->str);
    return head;
}