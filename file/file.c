//
// Created by 26432 on 2026/8/30.
//
#include <stdio.h>
#include <stdlib.h>
#include "../xstr/String.h"
#include "../list/list.h"
#include "file.h"
#include "../xsqlg/grm.h"

#define ENTER_LIMIT_COUNT 8

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
    TABLE_LIST_NODE *temp = head;
    // ---- table count
    int table_len = 0;
    while (temp != NULL){
        table_len++;
        temp = temp->next;
    }
    printf("table_len] %d\n" , table_len);
    fwrite(&table_len , sizeof(int) , 1 , fp);
    temp = head;
    while(temp != NULL){
        // ---- table name
        String *table_name = create_string(temp->table->NAME->str);
        fwrite(&table_name->length , sizeof(int) , 1 , fp);
        encrypt(table_name);
        fwrite(table_name->str , sizeof(char) , table_name->length , fp);
        string_free(table_name);
        // ---- table FIELD[]
        for(int i = 0 ; i < temp->table->length ; i++){
            String *table_field = create_string(temp->table->FIELD[i]->str);
            fwrite(&table_field->length , sizeof(int) , 1 , fp);
            encrypt(table_field);
            fwrite(table_field->str , sizeof(char) , table_field->length , fp);
            string_free(table_field);
        }
        // ---- table length
        int table_length = temp->table->length;
        fwrite(&table_length , sizeof(int) , 1 , fp);
        // ---- table datalines count
        int datalines_count = 0;
        DATALINE_NODE *dataline_temp = temp->table->dataline_head;
        while (dataline_temp != NULL){
            datalines_count++;
            dataline_temp = dataline_temp->next;
        }
        dataline_temp = temp->table->dataline_head;
        fwrite(&datalines_count , sizeof(int) , 1 , fp);
        // ---- datalines
        int space_count = 0;
        while (dataline_temp != NULL){
            for(int i = 0; i < temp->table->length ; i++){
                if(space_count == ENTER_LIMIT_COUNT){
                    char space = '\n';
                    fwrite(&space , sizeof(int) , 1 , fp);
                    space_count = 0;
                }
                String *dataline_str = create_string(dataline_temp->dataline->DATA[i]->str);
                fwrite(&dataline_str->length , sizeof(int) , 1 , fp);
                encrypt(dataline_str);
                fwrite(dataline_str->str , sizeof(char) , dataline_str->length , fp);
                string_free(dataline_str);
                space_count++;
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
    int l = 0;
    fread(&l , sizeof(int) , 1 , fp);
    printf("[L] %d\n" , l);
    return NULL;
}