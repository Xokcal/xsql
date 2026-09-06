#include <stdio.h>
#include "xsqlg/grm.h"
#include "xstr/String.h"

typedef struct upon_down_space_num_t{

}upon_down_space_num_t;

// DATALINES[ DATALINE1[DATA["1" , "ZhouYu"]] , DATALINE2[...]]
String *query_upon_down_line_num_calc(TABLE*target_table ,DATALINE_ARRAY *dataline_array_t 
    , int *query_field_indexs , int authentic_query_field_num){
    String *lines = create_string("");
    int line_num = 0;
    for (int i = 0; i < authentic_query_field_num;i++){ // 3
        int temp_field_line_num = 0;
        for (int j = 0; j < dataline_array_t->count;j++){
            if (dataline_array_t->datalines[j]->DATA[query_field_indexs[i]]->length > temp_field_line_num){
                temp_field_line_num = dataline_array_t->datalines[j]->
                DATA[query_field_indexs[i]]->length;
            }
            temp_field_line_num = dataline_array_t->datalines[j]->
            DATA[query_field_indexs[i]]->length;
        }
        line_num += temp_field_line_num;
    }
    for(int i = 0 ; i < authentic_query_field_num ; i++)
        line_num += target_table->FIELD[i]->length;
    for (int i = 0 ; i < line_num ; i++)
        combine_tail(lines , "-");
    return lines;
}