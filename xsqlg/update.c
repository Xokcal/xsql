#include "../xstr/String.h"
#include <stdio.h>
#include <stdlib.h>
#include "grm.h"
#include <string.h>
#include <time.h>
#include "../time/xtime.h"
#include "update.h"

#define XSQL_DEBUG

#ifdef XSQL_DEBUG 
    #define LOG(statue , str)\
        printf("UPDATE_DEBUG[%s] %s\n" , (statue) , (str))
    #define LOG_INT(statue , d)\
        printf("UPDATE_DEBUG[%s] %d\n" , (statue) , (d))
    #define XSQL_LOG(str)\
        printf("xsql : %s\n" , (str))
    #define XSQL_LOG_PARAM(str , param)\
        printf("xsql : %s \'%s\'\n" , (str) , (param))
#else
    #define LOG(statue , str)
#endif

static int
check_update_field_valid(PUField_p pufield_p)
{
    load_container_t *update_fields = pufield_p.update_fields;
    TABLE_LIST_NODE *table = pufield_p.target_table;
    for(int i = 0; i < *update_fields->auth_capable; i++){
        for(int j = 0; j < table->table->length ; j ++){
            String *origin = table->table->FIELD[j] , *match_field = update_fields->data[i];
            if(compare(origin , match_field->str))break;
            else if(j == table->table->length - 1&&!compare(origin , match_field->str)){
                XSQL_LOG_PARAM("update match field not valid : " , match_field->str);
                return 0;
            }
        }
    }
    return 1;
}

//UPDATE user(username , status) SET ('hajimi' , '0') WHERE id = '2001';
int 
UPDATE_exe(TABLE_LIST_NODE *head,TOKENSB *tokensb , int curr)
{
    String *table_name = create_string("");
    int is_over_update_field_left = 0;
    int is_over_update_field_right = 0;
    int is_over_update_set = 0;
    load_container_t *update_fields = create_loadContainerT();
    load_container_t *set_datas = create_loadContainerT();
    int *update_field_table_indexs;
    TABLE_LIST_NODE *target_table;
    update_where_t *updateWhereT = create_updateWhereT();
    int *datalineArray_count = (int*) malloc(sizeof(int));
    *datalineArray_count = 0;
    PUField_p pufield_p;
            pufield_p.tokensb = tokensb  , pufield_p.update_fields = update_fields,pufield_p
            .update_field_table_indexs = update_field_table_indexs
            ,pufield_p.set_datas = set_datas , pufield_p.updateWhereT = updateWhereT 
            , pufield_p.datalineArray_count = datalineArray_count;
    for(int i = 0 ; i < tokensb->count ; i++){
        if(!is_over_update_field_left && !IS_CONTAIN_KEYS(tokensb->tokens[i]->str)
            && !IS_PRIMARY_KEY(tokensb->tokens[i]->str)){
            combine_tail(table_name , tokensb->tokens[i]->str);
            target_table = get_TABLE_LIST_NODE(head , table_name->str);
            pufield_p.target_table = target_table;
            free(table_name);
            continue;
        }else if (!is_over_update_field_left && compare(tokensb->tokens[i] , "(")){
            is_over_update_field_left = 1;
            continue;
        }else if(is_over_update_field_left&&!is_over_update_field_right){
            pufield_p.curr = i;
            i = parse_update_field(pufield_p);
            is_over_update_field_right = 1;
            // check update fields...
            int check_update_field_status = check_update_field_valid(pufield_p);
            if(!check_update_field_status){return i;}
            continue;
        }else if(is_over_update_field_left && is_over_update_field_right 
            && compare(tokensb->tokens[i] , "set")){
            pufield_p.curr = i;
            i = parse_update_set(pufield_p);
            if(*pufield_p.set_datas->auth_capable != *pufield_p.update_fields->auth_capable){
                XSQL_LOG("update set value and field not valid!");
                return i;
            }
            is_over_update_set = 1;
            continue;
        }else if(is_over_update_set&&compare(tokensb->tokens[i] , "where")){
            pufield_p.curr = i;
            i = parse_update_where(pufield_p); // ; i - 1
            continue;
        }else if(compare(tokensb->tokens[i] , ";")){
            update_data_core(pufield_p);
            return i - 1;
        }
    }
}
// update a(name , id)set('hajimi' , '011')where id = '001' and name = 'laoda';

static void
match_field_index(PUField_p *pufield_p)
{
    TABLE_LIST_NODE *table = pufield_p->target_table;
    pufield_p->update_field_table_indexs =
    (int*)malloc((*pufield_p->update_fields->auth_capable) * sizeof(int));
    int *temp_field_index =pufield_p->update_field_table_indexs;int index_count = 0;
    for(int i = 0; i < *pufield_p->update_fields->auth_capable; i++){
        for(int j = 0; j < table->table->length ; j++){
            if(compare(table->table->FIELD[j] , pufield_p->update_fields->data[i]->str))
                temp_field_index[index_count++] = j;
        }
    }
}

static void
where_match_field_index(PUField_p pufield_p)
{
    TABLE_LIST_NODE *table = pufield_p.target_table;
    int *temp_field_index =pufield_p.updateWhereT->field_indexs;int index_count = 0;
    for(int i = 0; i < *pufield_p.update_fields->auth_capable; i++){
        for(int j = 0; j < table->table->length ; j++){
            if(compare(table->table->FIELD[j] , pufield_p.updateWhereT->field[i]->str))
                temp_field_index[index_count++] = j;
        }
    }
}

void 
update_data_core(PUField_p pufield_p)
{
    match_field_index(&pufield_p);where_match_field_index(pufield_p);
    DATALINE_ARRAY *datalineArray = select_match_dataline_array(pufield_p);
    for (int i = 0; i < *pufield_p.datalineArray_count; i++){
        for (int j = 0; j < *pufield_p.update_fields->auth_capable; j++){
            copy_string(pufield_p.set_datas->data[j]
                ,datalineArray->datalines[i]->DATA[pufield_p.update_field_table_indexs[j]]
            );
        }
    }
}

static int
is_match_were_field(PUField_p pufield_p,DATALINE_ARRAY *datalineArray , DATALINE_NODE *temp)
{
    int count = *pufield_p.updateWhereT->auth_field_count;
    for(int i = 0; i < count ; i++){
        String *origin = temp->dataline->DATA[pufield_p.updateWhereT->field_indexs[i]];
        String *target = pufield_p.updateWhereT->data[i];
        if(!compare(origin , target->str))return 0;
    }
    return 1;
}

static void
collect_dataline_array(PUField_p pufield_p,DATALINE_ARRAY *datalineArray , DATALINE_NODE *temp)
{
    if((*pufield_p.datalineArray_count) == datalineArray->count)
        extend_DATALINE_ARRAY(pufield_p.target_table , datalineArray);
     datalineArray->datalines[(*pufield_p.datalineArray_count)++] = temp->dataline;
}

DATALINE_ARRAY *
select_match_dataline_array(PUField_p pufield_p)
{
    DATALINE_ARRAY *datalineArray = create_DATALINE_ARRAY(pufield_p.target_table);
    DATALINE_NODE *temp = pufield_p.target_table->table->dataline_head;
    while (temp != NULL){
        if(is_match_were_field(pufield_p , datalineArray , temp))
            collect_dataline_array(pufield_p , datalineArray , temp);
        temp = temp->next;
    }
    return datalineArray;
}

int 
parse_update_field(PUField_p pufield_p)
{
    String *temp_field_name = create_string("");
    for(int i = pufield_p.curr; i < pufield_p.tokensb->count ; i++){
        if(!compare(pufield_p.tokensb->tokens[i] , " ")&&!compare(pufield_p.tokensb->tokens[i] , ",")
        &&!compare(pufield_p.tokensb->tokens[i] , "(") && !compare(pufield_p.tokensb->tokens[i] , ")")){ //field_name
            combine_tail(temp_field_name , pufield_p.tokensb->tokens[i]->str);
            continue;
        }else if(compare(pufield_p.tokensb->tokens[i] , ",")){
            combine_tail(pufield_p.update_fields->data[(*pufield_p.update_fields->auth_capable)++] 
            , temp_field_name->str);
            delete_all(temp_field_name);
            continue;
        }else if(compare(pufield_p.tokensb->tokens[i] , ")")){
            combine_tail(pufield_p.update_fields->data[(*pufield_p.update_fields->auth_capable)++] 
            , temp_field_name->str);
            string_free(temp_field_name);
            parse_update_auth_field_indexs(pufield_p);
            return i;   
        }else continue;
    }
    return pufield_p.curr + 1;
}

static int
is_update_set_token(PUField_p pufield_p, int i)
{
    String *token = pufield_p.tokensb->tokens[i];
    if(compare(token , "(")||compare(token , ")"))return 0;
    if (compare(token , "\'"))return 0;
    if(compare(token , ","))return 0;
    if(compare(token , ";"))return 0;
    return 1;
}

static void
is_over_set_left_func(int *is_over_set_left){
    *is_over_set_left = 1;
}

static void
update_set_quote_over_func(PUField_p pufield_p , String *temp_data
    ,int *is_over_single_quote)
{
    if(*is_over_single_quote){
        if (!compare(temp_data , ""))
            copy_string(temp_data , pufield_p.set_datas->data
                [(*pufield_p.set_datas->auth_capable)++]);
        delete_all(temp_data);
        *is_over_single_quote = 0;
    }else
        *is_over_single_quote = 1;
}

static void
quote_true_storage_value(PUField_p pufield_p , int i , String *temp_data)
{
    combine_tail(temp_data , pufield_p.tokensb->tokens[i]->str);
}

int 
parse_update_set(PUField_p pufield_p)
{
    int is_over_single_quote = 0;
    int is_over_set_left = 0;
    String *temp_data = create_string("");
    for (int i = pufield_p.curr ; i < pufield_p.tokensb->count ; i++){
        String *token = pufield_p.tokensb->tokens[i];

        // encounter ';' is over!
        if(compare(token , ")")&&is_over_set_left){
            return i;
        }

        // encounter '(' is start!
        if(compare(token , "(")){
            is_over_set_left_func(&is_over_set_left);
            continue;
        }

        // encounter '\'' transfer status!
        if(compare(token , "\'")&&is_over_set_left){
            update_set_quote_over_func(pufield_p , temp_data , &is_over_single_quote);
            continue;
        }
        
        // quote is true storage value
        if(is_over_single_quote&&is_over_set_left){
            quote_true_storage_value(pufield_p , i , temp_data);
            continue;
        }
        
        // not is set primary key and quote is false
        if(is_update_set_token(pufield_p , i )){
            continue;
        }
    }
}

static int
is_where_field_token(TOKENSB *tokensb, int i, int is_over_single_quote)
{
    String *token = tokensb->tokens[i];
    if (is_over_single_quote)return 0;
    if (compare(token, " "))return 0;
    if (IS_PRIMARY_KEY(token->str))return 0;
    if (compare(token, "\'"))return 0;
    if (compare(token, "="))return 0;
    if (compare(token, "and"))return 0;
    if (compare(token, "or")) return 0;
    if (compare(token, ";")) return 0;
    return 1;
}

static void
parse_where_field(PUField_p pufield_p, int i)
{
    int *count = pufield_p.updateWhereT->auth_field_count;
    copy_string(pufield_p.tokensb->tokens[i],
                pufield_p.updateWhereT->field[(*count)++]);
}

static void
parse_where_value(PUField_p pufield_p, String *temp_token)
{
    int *count = pufield_p.updateWhereT->auth_data_count;
    copy_string(temp_token,
                pufield_p.updateWhereT->data[(*count)++]);
    delete_all(temp_token);
}

static void
parse_where_logic(PUField_p pufield_p, int i)
{
    int *count = pufield_p.updateWhereT->auth_logic_count;
    copy_string(pufield_p.tokensb->tokens[i],
                pufield_p.updateWhereT->logic[(*count)++]);
}

int
parse_update_where(PUField_p pufield_p)
{
    int is_over_single_quote = 0;
    String *temp_token = create_string("");

    for (int i = pufield_p.curr + 1; i < pufield_p.tokensb->count; i++) {
        String *token = pufield_p.tokensb->tokens[i];

        // encounter ';' is over!
        if (compare(token, ";")) {
            string_free(temp_token);
            return i - 1;
        }

        // encounter '\'' quote is transfer!
        if (compare(token, "\'")) {
            if (is_over_single_quote) {
                parse_where_value(pufield_p, temp_token);
                is_over_single_quote = 0;
            } else {
                is_over_single_quote = 1;
            }
            continue;
        }

        // combine temp string value!
        if (is_over_single_quote) {
            combine_tail(temp_token, token->str);
            continue;
        }

        // 4. logic sign!
        if (compare(token, "and") || compare(token, "or")) {
            parse_where_logic(pufield_p, i);
            continue;
        }

        // 5. field name!
        if (is_where_field_token(pufield_p.tokensb, i, is_over_single_quote)) {
            parse_where_field(pufield_p, i);
            continue;
        }

    }

    string_free(temp_token);
    return -1;
}

void 
parse_update_auth_field_indexs(PUField_p pufield_p)
{
    TABLE_LIST_NODE *target_table = pufield_p.target_table;
    int *auth_field_indexs = (int*)malloc((*pufield_p.update_fields->auth_capable) * sizeof(int));
    for(int i = 0; i < *pufield_p.update_fields->auth_capable ; i++){
        for(int j = 0 ; j < target_table->table->length ; j++){
            if(compare(target_table->table->FIELD[j] , pufield_p.update_fields->data[i]->str))
                auth_field_indexs[i] = j;
        }
    }
}

load_container_t *
create_loadContainerT()
{
    load_container_t *load_container = (load_container_t*)malloc(sizeof(load_container_t));
    load_container->data = (String**)malloc(10 * sizeof(String*));
    for(int i = 0; i < 10 ; i++)
        load_container->data[i] = create_string("");
    load_container->count = 10;
    load_container->auth_capable = (int*) malloc(sizeof(int));
    *load_container->auth_capable = 0;
    return load_container;
}

load_container_t *
extend_updateFieldT(load_container_t *old)
{
    int new_count = old->count * 2;
    String** new_load_data = (String**)malloc(new_count * sizeof(String*));
    for(int i = 0 ; i < old->count ; i++)
        copy_string(old->data[i] , new_load_data[i]);
    for(int i = old->count ; i < new_count ; i++)
        new_load_data[i] = create_string("");
    for(int i = 0 ; i < old->count ; i++)
        string_free(old->data[i]);
    free(old->data);
    old->data = new_load_data;
    return old;
}

void 
free_updateFieldT(load_container_t * load_container)
{
    for(int i = 0 ; i < load_container->count ; i++)
        string_free(load_container->data[i]);
    free(load_container->data);
    free(load_container->auth_capable);
    free(load_container);
    return;
}

update_where_t *
create_updateWhereT()
{
    update_where_t *updateWhereT = (update_where_t*)malloc(sizeof(update_where_t));
    updateWhereT->field = (String**)malloc(10 * sizeof(String*));
    updateWhereT->data = (String**)malloc(10 * sizeof(String*));
    updateWhereT->logic = (String**)malloc(10 * sizeof(String*));
    updateWhereT->field_indexs = (int *)malloc(10 * sizeof(int));
    updateWhereT->auth_logic_count = (int*)malloc(sizeof(int));
    updateWhereT->auth_field_count = (int*)malloc(sizeof(int));
    updateWhereT->auth_data_count = (int*)malloc(sizeof(int));
    updateWhereT->auth_indexs_count = (int*)malloc(sizeof(int));
    
    for (int i = 0; i < 10; i++) {
        updateWhereT->field[i] = create_string("");
        updateWhereT->data[i] = create_string("");
        updateWhereT->logic[i] = create_string(""); 
    }
    for (int i = 0; i < 10; i++) {
        updateWhereT->field_indexs[i] = -1;
    }
    
    *updateWhereT->auth_logic_count = 0; 
    *updateWhereT->auth_field_count = 0;
    *updateWhereT->auth_data_count = 0;
    *updateWhereT->auth_indexs_count = 0;
    updateWhereT->count = 0;
    return updateWhereT;
}

update_where_t *
extend_updateWhereT(update_where_t *old)
{
    if (old == NULL) return NULL;
    int new_count = old->count * 2;
    if (new_count == 0) new_count = 10;
    
    String **new_field = (String**)malloc(new_count * sizeof(String*));
    String **new_data = (String**)malloc(new_count * sizeof(String*));
    String **new_logic = (String**)malloc(new_count * sizeof(String*)); 
    int *new_indexs = (int*)malloc(new_count * sizeof(int));
    
    if (!new_field || !new_data || !new_logic || !new_indexs) {
        free(new_field); free(new_data); free(new_logic); free(new_indexs);
        return NULL;
    }
    
    for (int i = 0; i < old->count; i++) {
        new_field[i] = create_string(old->field[i]->str);
        new_data[i] = create_string(old->data[i]->str);
        new_logic[i] = create_string(old->logic[i]->str); 
        new_indexs[i] = old->field_indexs[i];
    }
    for (int i = old->count; i < new_count; i++) {
        new_field[i] = create_string("");
        new_data[i] = create_string("");
        new_logic[i] = create_string("");
        new_indexs[i] = -1;
    }
    
    // 释放旧数据
    for (int i = 0; i < old->count; i++) {
        string_free(old->field[i]);
        string_free(old->data[i]);
        string_free(old->logic[i]);
    }
    free(old->field);
    free(old->data);
    free(old->logic);
    free(old->field_indexs);
    
    old->field = new_field;
    old->data = new_data;
    old->logic = new_logic;
    old->field_indexs = new_indexs;
    old->count = new_count;
    return old;
}

void 
free_updateWhereT(update_where_t *ptr)
{
    if (ptr == NULL) return;
    for (int i = 0; i < ptr->count; i++) {
        if (ptr->field[i]) string_free(ptr->field[i]);
        if (ptr->data[i]) string_free(ptr->data[i]);
        if (ptr->logic[i]) string_free(ptr->logic[i]);
    }
    free(ptr->field);
    free(ptr->data);
    free(ptr->logic); 
    free(ptr->field_indexs);
    free(ptr->auth_logic_count); 
    free(ptr->auth_field_count);
    free(ptr->auth_data_count);
    free(ptr->auth_indexs_count);
    free(ptr);
}