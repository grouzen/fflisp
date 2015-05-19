/* This file is licensed under the terms of MIT license, see LICENSE file. */

#include <stdio.h>

#include "../include/object.h"

static void print_list(struct lispobj*);

int print_bracket = 1;

void print(struct lispobj *obj)
{
#ifdef __DEBUG_PRINT__
    printf("[");
#endif /* __DEBUG_PRINT__ */
    if(obj == NULL) {
        printf("NIL");
    } else if(OBJ_TYPE(obj) == ERROR) {
        printf("Error: %s", ERROR_VALUE(obj));
    } else if(OBJ_TYPE(obj) == SYMBOL) {
        printf("%s", SYMBOL_VALUE(obj));
    } else if(OBJ_TYPE(obj) == NUMBER) {
        printf("%d", NUMBER_VALUE(obj));
    } else if(OBJ_TYPE(obj) == STRING) {
        printf("\"%s\"", STRING_VALUE(obj));
    } else {
        if(CAR(obj) == NEW_SYMBOL("PROC")) {
            printf("<procedure ");
            if(CADR(obj) != NEW_SYMBOL("NIL")) {
                print_list(CADR(obj));
            } else {
                printf("()");
            }
            printf(" %p>", CADDDR(obj));
        } else if(CAR(obj) == NEW_SYMBOL("SUBR")) {
            printf("<primitive-procedure %p>", CADR(obj));
        } else {
            print_list(obj);
        }
    }
#ifdef __DEBUG_PRINT__
    if(obj != NULL) {
        printf(" => %d]", OBJ_REFS(obj));
    } else {
        printf(" => nil]");
    }
#endif /* __DEBUG_PRINT__ */
    return;
}

static void print_list(struct lispobj *obj)
{
    if(print_bracket) {
        printf("(");
    }
    print_bracket = 0;

    if(CAR(obj) != NULL && OBJ_TYPE(CAR(obj)) == CONS) {
        printf("(");
    }
    
    print(CAR(obj));

    if(CDR(obj) == NULL) {
        printf(")");
        print_bracket = 1;
        return;
    }
    
    if(OBJ_TYPE(CDR(obj)) == CONS) {
        printf(" ");
    } else {
        printf(" . ");
    }

    print_bracket = 0;
    
    print(CDR(obj));
    if(!print_bracket) {
        printf(")");
    }
    print_bracket = 1;
    
    return;
}
