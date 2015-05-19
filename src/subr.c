/* This file is licensed under the terms of MIT license, see LICENSE file. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "../include/object.h"
#include "../include/heap.h"
#include "../include/repl.h"
#include "../include/eval.h"
#include "../include/read.h"
#include "../include/subr.h"

struct lispobj *cons(struct lispobj *car, struct lispobj *cdr)
{
    struct lispobj *cons;

    cons = object_create(CONS, NULL);
    CAR(cons) = heap_grab(car);
    CDR(cons) = heap_grab(cdr);

    return cons;
}

struct lispobj *list(int n, ...)
{
    va_list ap;
    struct lispobj *list, *tmp;

    list = NEW_CONS(NULL, NULL);

    tmp = list;
    
    va_start(ap, n);
    while(n) {
        CAR(tmp) = heap_grab((struct lispobj *) va_arg(ap, struct lispobj *));
        if(--n) {
            CDR(tmp) = heap_grab(NEW_CONS(NULL, NULL));
            tmp = CDR(tmp);
        }
    }
    va_end(ap);
    
    CDR(tmp) = NULL;

    return list;
}

int length(struct lispobj *list)
{
    int n = 0;
    
    while(list != NULL && list != NEW_SYMBOL("NIL")) {
        list = CDR(list);
        n++;
    }

    return n;
}

struct lispobj *subr_newline(struct lispobj *args)
{
    if(length(args))
        return ERROR_ARGS;

    printf("\n");

    return OBJ_TRUE;
}

struct lispobj *subr_display(struct lispobj *args)
{
    if(length(args) != 1)
        return ERROR_ARGS;

    if(CAR(args) != NULL && OBJ_TYPE(CAR(args)) == STRING) {
        printf("%s", STRING_VALUE(CAR(args)));
    } else {
        print(CAR(args));
    }

    return OBJ_TRUE;
}

struct lispobj *subr_apply(struct lispobj *args)
{
    if(length(args) != 2)
        return ERROR_ARGS;

    struct lispobj *proc, *params;
    proc = CAR(args);
    params = CADR(args);

    if((proc != NULL && OBJ_TYPE(proc) != CONS) ||
       (params != NULL && OBJ_TYPE(params) != CONS)) {
        return NEW_ERROR("Wrong arguments type.\n");
    }

    return apply(proc, params);
}

struct lispobj *subr_error(struct lispobj *args)
{
    if(length(args) != 1)
        return ERROR_ARGS;

    struct lispobj *obj = CAR(args);
    if(OBJ_TYPE(obj) != STRING)
        return NEW_ERROR("Argument is not a string.\n");
    
    return NEW_ERROR(STRING_VALUE(obj));
}

struct lispobj *subr_eval(struct lispobj *args)
{
    if(length(args) != 1)
        return ERROR_ARGS;

    return eval(CAR(args), environment);
}

struct lispobj *subr_read(struct lispobj *args)
{
    if(length(args) != 0)
        return ERROR_ARGS;

    /* Just read a standard input. */
    return read(stdin);
}

struct lispobj *subr_load(struct lispobj* args)
{
    if(length(args) != 1)
        return ERROR_ARGS;

    struct lispobj *obj = CAR(args);

    if(obj == NULL || OBJ_TYPE(obj) != STRING) {
        return NEW_ERROR("Argument is not a string.\n");
    }

    if(!load(STRING_VALUE(obj)))
        return OBJ_FALSE;

    return OBJ_TRUE;
}

struct lispobj *subr_car(struct lispobj *args)
{
    if(length(args) != 1)
        return ERROR_ARGS;

    struct lispobj *obj = CAR(args);
        
    if(obj == NULL || OBJ_TYPE(obj) != CONS) {
        return NEW_ERROR("Argument is not a CONS type.\n");
    }

    return CAR(obj);
}

struct lispobj *subr_cdr(struct lispobj *args)
{
    if(length(args) != 1)
        return ERROR_ARGS;
    
    struct lispobj *obj = CAR(args);
    
    if(obj == NULL || OBJ_TYPE(obj) != CONS) {
        return NEW_ERROR("Argument is not a CONS type.\n");
    }
    
    return CDR(obj);
}

struct lispobj *subr_cons(struct lispobj *args)
{
    if(length(args) != 2)
        return ERROR_ARGS;

    struct lispobj *car, *cdr, *pair;
    car = CAR(args);
    cdr = CADR(args);

    pair = NEW_CONS(car, cdr);

    return pair;
}

struct lispobj *subr_pair(struct lispobj *args)
{
    if(length(args) != 1)
        return ERROR_ARGS;
    
    struct lispobj *obj = CAR(args);
    
    if(obj != NULL && OBJ_TYPE(obj) == CONS)
        return OBJ_TRUE;

    return OBJ_FALSE;
}

struct lispobj *subr_string(struct lispobj *args)
{
    if(length(args) != 1)
        return ERROR_ARGS;

    struct lispobj *obj = CAR(args);

    if(OBJ_TYPE(obj) == STRING)
        return OBJ_TRUE;

    return OBJ_FALSE;
}

struct lispobj *subr_number(struct lispobj *args)
{
    if(length(args) != 1)
        return ERROR_ARGS;

    struct lispobj *obj = CAR(args);
    
    if(OBJ_TYPE(obj) == NUMBER)
        return OBJ_TRUE;

    return OBJ_FALSE;
}

struct lispobj *subr_symbol(struct lispobj *args)
{
    if(length(args) != 1)
        return ERROR_ARGS;

    struct lispobj *obj = CAR(args);
    
    if(OBJ_TYPE(obj) == SYMBOL)
        return OBJ_TRUE;

    return OBJ_FALSE;
}

struct lispobj *subr_atom(struct lispobj *args)
{
    if(length(args) != 1)
        return ERROR_ARGS;

    struct lispobj *obj = CAR(args);
    
    if(OBJ_TYPE(obj) != CONS)
        return OBJ_TRUE;
    
    return OBJ_FALSE;
}

struct lispobj *subr_null(struct lispobj *args)
{
    if(length(args) != 1)
        return ERROR_ARGS;

    struct lispobj *obj = CAR(args);
    
    if(!obj)
        return OBJ_TRUE;
    return OBJ_FALSE;
}

struct lispobj *subr_not(struct lispobj *args)
{
    if(length(args) != 1)
        return ERROR_ARGS;

    struct lispobj *obj = CAR(args);
    
    if(obj)
        return OBJ_FALSE;
    return OBJ_TRUE;
}

struct lispobj *subr_eq(struct lispobj *args)
{
    if(length(args) != 2)
        return ERROR_ARGS;

    return CAR(args) == CADR(args) ? OBJ_TRUE : OBJ_FALSE;
}

struct lispobj *subr_eql(struct lispobj *args)
{
    if(length(args) != 2)
        return ERROR_ARGS;

    struct lispobj *obj1, *obj2;
    
    obj1 = CAR(args);
    obj2 = CADR(args);

    if(obj1 == obj2)
        return OBJ_TRUE;
    
    if(OBJ_TYPE(obj1) == OBJ_TYPE(obj2)) {
        if(OBJ_TYPE(obj1) == SYMBOL && obj1 == obj2) {
            return OBJ_TRUE;
        } else if(OBJ_TYPE(obj1) == NUMBER &&
                  NUMBER_VALUE(obj1) == NUMBER_VALUE(obj2)) {
            return OBJ_TRUE;
        }
    }

    return OBJ_FALSE;
}

struct lispobj *subr_equal(struct lispobj *args)
{
    if(length(args) != 2)
        return ERROR_ARGS;

    struct lispobj *obj1, *obj2, *ret;

    obj1 = CAR(args);
    obj2 = CADR(args);

    if(obj1 == NULL && obj2 == NULL) {
        ret = OBJ_TRUE;
    } else if(obj1 != NULL && obj2 != NULL && OBJ_TYPE(obj1) == OBJ_TYPE(obj2)) {
        if(OBJ_TYPE(obj1) == CONS) {
            struct lispobj *args_equal_cars, *args_equal_cdrs;

            args_equal_cars = heap_grab(list(2, CAR(obj1), CAR(obj2)));
            args_equal_cdrs = heap_grab(list(2, CDR(obj1), CDR(obj2)));

            if(subr_equal(args_equal_cars) &&
               subr_equal(args_equal_cdrs)) {
                ret = OBJ_TRUE;
            } else {
                ret = OBJ_FALSE;
            }

            heap_release(args_equal_cars);
            heap_release(args_equal_cdrs);
        } else {
            struct lispobj *args_eq;

            args_eq = heap_grab(list(2, obj1, obj2));
            ret = subr_eq(args_eq);
            heap_release(args_eq);
        }
    } else
        ret = OBJ_FALSE;
    
    return ret;
}

struct lispobj *subr_list(struct lispobj *args)
{
    if(length(args) > 0) {
        struct lispobj *list;

        list = NEW_CONS(NULL, NULL);
        
        CAR(list) = heap_grab(CAR(args));
        CDR(list) = heap_grab(subr_list(CDR(args)));
    
        return list;
    }
    
    return OBJ_FALSE; // NULL
}

struct lispobj *subr_plus(struct lispobj *args)
{
    struct lispobj *num;
    
    num = NEW_NUMBER("0");

    while(args != NULL) {
        if(CAR(args) != NULL && OBJ_TYPE(CAR(args)) == NUMBER) {
            NUMBER_VALUE(num) += NUMBER_VALUE(CAR(args));
            args = CDR(args);
        } else {
            object_delete(num);
            return NEW_ERROR("Argument is not a number.\n");
        }
    }

    return num;
}

struct lispobj *subr_minus(struct lispobj *args)
{
    if(length(args) == 0)
        return ERROR_ARGS;
    
    struct lispobj *num;
    char num_value[30];

    snprintf(num_value, 30, "%d", NUMBER_VALUE(CAR(args)));
    num = NEW_NUMBER(num_value);

    args = CDR(args);
    if(args == NULL) {
        NUMBER_VALUE(num) = 0 - NUMBER_VALUE(num);
    } else {
        while(args != NULL) {
            if(CAR(args) != NULL && OBJ_TYPE(CAR(args)) == NUMBER) {
                NUMBER_VALUE(num) -= NUMBER_VALUE(CAR(args));
                args = CDR(args);
            } else {
                object_delete(num);
                return NEW_ERROR("Argument is not a number.\n");
            }
        }
    }
    return num;
}

struct lispobj *subr_multi(struct lispobj *args)
{
    struct lispobj *num;
    
    num = NEW_NUMBER("1");

    while(args != NULL) {
        if(CAR(args) != NULL && OBJ_TYPE(CAR(args)) == NUMBER) {
            NUMBER_VALUE(num) *= NUMBER_VALUE(CAR(args));
            args = CDR(args);
        } else {
            object_delete(num);
            return NEW_ERROR("Argument is not a number.\n");
        }
    }

    return num;
}

struct lispobj *subr_divide(struct lispobj *args)
{
    if(length(args) < 2)
        return ERROR_ARGS;
    
    if(CAR(args) == NULL || OBJ_TYPE(CAR(args)) != NUMBER) {
        return NEW_ERROR("Argument is not a number.\n");
    }
    
    struct lispobj *num = CAR(args);
    args = CDR(args);
    
    while(args != NULL) {
        if(CAR(args) != NULL && OBJ_TYPE(CAR(args)) == NUMBER) {
            NUMBER_VALUE(num) /= NUMBER_VALUE(CAR(args));
            args = CDR(args);
        } else {
            object_delete(num);
            return NEW_ERROR("Argument is not a number.\n");
        }
    }

    return num;
}

struct lispobj *subr_mod(struct lispobj *args)
{
    if(length(args) != 2)
        return ERROR_ARGS;
    
    struct lispobj *number, *div;

    number = CAR(args);
    div    = CADR(args);

    if(OBJ_TYPE(number) == NUMBER && OBJ_TYPE(div) == NUMBER) {
        char mod[30];
        
        snprintf(mod, 30, "%d", NUMBER_VALUE(number) % NUMBER_VALUE(div));
        return NEW_NUMBER(mod);
    } else {
        return NEW_ERROR("Arguments must be numbers.\n");
    }
    
}

struct lispobj *subr_greatthan(struct lispobj *args)
{
    if(length(args) != 2)
        return ERROR_ARGS;

    struct lispobj *obj1, *obj2;

    obj1 = CAR(args);
    obj2 = CADR(args);

    if(obj1 != NULL && obj2 != NULL) {
        if(OBJ_TYPE(obj1) == NUMBER && OBJ_TYPE(obj2) == NUMBER) {
            if(NUMBER_VALUE(obj1) > NUMBER_VALUE(obj2)) {
                return OBJ_TRUE;
            }
        } else {
            return NEW_ERROR("Argument must be a number.\n");
        }
    }

    return OBJ_FALSE;
}

struct lispobj *subr_lessthan(struct lispobj *args)
{
    if(length(args) != 2)
        return ERROR_ARGS;

    struct lispobj *obj1, *obj2;

    obj1 = CAR(args);
    obj2 = CADR(args);

    if(obj1 != NULL && obj2 != NULL) {
        if(OBJ_TYPE(obj1) == NUMBER && OBJ_TYPE(obj2) == NUMBER) {
            if(NUMBER_VALUE(obj1) < NUMBER_VALUE(obj2)) {
                return OBJ_TRUE;
            }
        } else {
            return NEW_ERROR("Argument must be a number.\n");
        }
    }

    return OBJ_FALSE;
}

struct lispobj *subr_compar(struct lispobj *args)
{
    if(length(args) != 2)
        return ERROR_ARGS;

    struct lispobj *obj1, *obj2;

    obj1 = CAR(args);
    obj2 = CADR(args);
    
    if(obj1 != NULL && obj2 != NULL) {
        if(OBJ_TYPE(obj1) == NUMBER && OBJ_TYPE(obj2) == NUMBER) {
            if(NUMBER_VALUE(obj1) == NUMBER_VALUE(obj2))
                return OBJ_TRUE;
        } else {
            return NEW_ERROR("Arguments must be a number.\n");
        }
    }

    return OBJ_FALSE;
}

struct lispobj *subr_heap_object(struct lispobj *args)
{
    if(length(args) != 1)
        return ERROR_ARGS;

    struct lispobj *obj = CAR(args);

    printf("Debug object:");
    heap_debug_object(obj);
    printf("\n");
    
    return OBJ_FALSE;
}

struct lispobj *subr_heap(struct lispobj *args)
{
    if(length(args) != 0)
        return ERROR_ARGS;

    heap_debug();

    return OBJ_FALSE;
}

struct lispobj *subr_or(struct lispobj *args)
{
    if(length(args) > 0) {
        while(args != NULL) {
            if(CAR(args)) {
                return CAR(args);
            }

            args = CDR(args);
        }
    }

    return OBJ_FALSE;
}

struct lispobj *subr_and(struct  lispobj *args)
{
    if(length(args) > 0) {
        struct lispobj *ret = NULL;
        
        while(args != NULL) {
            if(!CAR(args)) {
                return OBJ_FALSE;
            }
            
            ret = CAR(args);
            args = CDR(args);
        }

        return ret;
    }

    return OBJ_TRUE;
}

struct lispobj *subr_rplaca(struct lispobj *args)
{
    if(length(args) != 2)
        return ERROR_ARGS;

    struct lispobj *old, *val, *place;
    place = CAR(args);
    val = CADR(args);

    old = CAR(place);
    CAR(place) = heap_grab(val);
    heap_release(old);

    return place;
}

struct lispobj *subr_rplacd(struct lispobj *args)
{
    if(length(args) != 2)
        return ERROR_ARGS;

    struct lispobj *old, *val, *place;
    place = CAR(args);
    val = CADR(args);

    old = CDR(place);
    CDR(place) = heap_grab(val);
    heap_release(old);
    
    return place;
}
