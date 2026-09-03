//
// Created by 26432 on 2026/8/30.
//

#ifndef XOKSQLC语言版_STRING_H
#define XOKSQLC语言版_STRING_H

typedef struct String{
    char *str;
    long length;
}String;

typedef struct Splitor{
    String **splits;
    int count;
}Splitor;

String* create_string(char* str);
void combine_tail(String *origin , char* nstr);
void string_println(String *string);
int compare(String *string , char* compare_c);
char indexof(String *string , int index);
String *substring(String *string , int start , int end);
void delete_all(String *string);
void string_free(String *string);
void replace_str(String *string , char* old , char* new);
void combine_tail_char(String *origin , char nstr);
void copy_string(String* copyer , String *be_copyer);
void delete_indexof(String *string , int index);
String *substring_to_end(String *string , int start);
int *index_strof(String *string , char *target);
void string_trim(String *string);
Splitor *split(String *string , char *split_c);
void splitor_free(Splitor *splitor);
String *join(String **strings , int join_count , char* join_str);
void delete_strof(String* string , char* target);

#endif //XOKSQLC语言版_STRING_H
