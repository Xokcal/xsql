#include <stdio.h>
#include "../xstr/String.h"
#include <stdlib.h>
#include "../list/list.h"

typedef struct XsqlWhereCondi{
    int *field_match_indexs;
    int field_count;
    int data_count;
    int logic_count;
    String **field;
    String **data;
    String **logic;
    int index_count;
    int size;
}XsqlWhereCondi;

XsqlWhereCondi*
create_XsqlWhereCondi();

XsqlWhereCondi*
extend_XsqlWhereCondi(XsqlWhereCondi *old);

void
free_XsqlWhereCondi(XsqlWhereCondi *xsqlWhereCondi);