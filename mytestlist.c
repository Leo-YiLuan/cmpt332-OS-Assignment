/*
# NAME: Matthew Munro
# NSID: mam552
# STUDENT NUMBER: 11291769
# NAME: Yi Luan
# NSID: yil160
# STUDENT NUMBER: 11253856
# CMPT 332 2022
# A1 Phase 1
*/
#include "list.h"
#include <stdio.h>

int main(int argc, char **argv){
    
    LIST List1[10];
    LIST List2[10];
    LIST *List1p = List1;
    LIST *List2p = List2;
    int num = 5;
    int *item = &num;

    /* call all the functions in list_adders.c*/
    ListCreate();    

    if (ListAdd(List1p,item)!=0)
    {
        printf("Error: return value of ListAdd() is not correct");
    }
    if (ListAdd(NULL,item)!=-1)
    {
        printf("Error: return value of ListAdd() is not correct");
    }
    if (ListAdd(List1p,NULL)!=-1)
    {
        printf("Error: return value of ListAdd() is not correct");
    }
    
    if (ListInsert(List1p,item)!=0)
    {
        printf("Error: return value of ListInsert() is not correct");
    }
    if (ListInsert(NULL,item)!=-1)
    {
        printf("Error: return value of ListInsert() is not correct");
    }
    if (ListInsert(List1p,NULL)!=-1)
    {
        printf("Error: return value of ListInsert() is not correct");
    }

    
    if (ListAppend(List1p,item)!=0)
    {
        printf("Error: return value of ListAppend() is not correct");
    }
    if (ListAppend(NULL,item)!=-1)
    {
        printf("Error: return value of ListAppend() is not correct");
    }
    if (ListAppend(List1p,NULL)!=-1)
    {
        printf("Error: return value of ListAppend() is not correct");
    }
    
    
    if (ListPrepend(List1p,item)!=0)
    {
        printf("Error: return value of ListPrepend() is not correct");
    }
    if (ListPrepend(NULL,item)!=-1)
    {
        printf("Error: return value of ListPrepend() is not correct");
    }
    if (ListPrepend(List1p,NULL)!=-1)
    {
        printf("Error: return value of ListPrepend() is not correct");
    }
    
    ListConcat(List1p,List2p);
    ListConcat(NULL,List2p);
    ListConcat(List1p,NULL);
    
    
    return 0;
}