/* This file is licensed under the terms of MIT license, see LICENSE file. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/object.h"
#include "../include/heap.h"

#define NEW_OBJECT(obj) ((obj) = malloc(sizeof(struct lispobj)))

struct lispobj *object_create(int type, char *value)
{
    struct lispobj *obj;
    char *symbol_name, *error, *string;
    struct cons *cons;
    
    switch(type) {
    case SYMBOL:
        symbol_name = malloc(sizeof(char) * (strlen(value) + 1));

        obj = symbol_table_lookup(value);
        
        if(obj == NULL) {
            NEW_OBJECT(obj);
            SYMBOL_VALUE(obj) = strcpy(symbol_name, value);
            OBJ_TYPE(obj) = SYMBOL;
            
            OBJ_REFS(obj) = 0;
            
            obj = symbol_table_intern(obj);
            heap_add(obj);
        }
        
        break;
    case STRING:
        string = malloc(sizeof(char) * (strlen(value) + 1));
        
        NEW_OBJECT(obj);
        STRING_VALUE(obj) = strcpy(string, value);
        OBJ_TYPE(obj) = STRING;
        heap_add(obj);

        OBJ_REFS(obj) = 0;

        break;
    case NUMBER:
        NEW_OBJECT(obj);
        NUMBER_VALUE(obj) = atoi(value);
        OBJ_TYPE(obj) = NUMBER;
        heap_add(obj);
        
        OBJ_REFS(obj) = 0;
        
        break;
    case CONS:
        cons = malloc(sizeof(struct cons));

        NEW_OBJECT(obj);

        CONS_VALUE(obj) = cons;
        CAR(obj) = NULL;
        CDR(obj) = NULL;
        OBJ_TYPE(obj) = CONS;
        heap_add(obj);

        OBJ_REFS(obj) = 0;
        
        break;
    case ERROR:
        error = malloc(sizeof(char) * (strlen(value) + 1));

        NEW_OBJECT(obj);

        error = strcpy(error, value);
        ERROR_VALUE(obj) = error;
        OBJ_TYPE(obj) = ERROR;
        heap_add(obj);
        
        OBJ_REFS(obj) = 0;

        break;
    default:
        break;
    }
    
    return obj;
}

void object_delete(struct lispobj *obj)
{
    heap_remove(obj);
    
    switch(OBJ_TYPE(obj)) {
    case SYMBOL:
        free(SYMBOL_VALUE(obj));
        free(obj);

        break;
    case NUMBER:
        free(obj);

        break;
    case CONS:
        if(CAR(obj) != NULL)
            heap_release(CAR(obj));
        if(CDR(obj) != NULL)
            heap_release(CDR(obj));
        
        free(CONS_VALUE(obj));
        free(obj);

        break;
    case STRING:
        free(STRING_VALUE(obj));
        free(obj);

        break;
    case ERROR:
        free(ERROR_VALUE(obj));
        free(obj);

        break;
    default:
        break;
    }

    return;
}
