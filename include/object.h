/* This file is licensed under the terms of MIT license, see LICENSE file. */

#ifndef __OBJECT_H__
#define __OBJECT_H__

enum {
    CONS = 0,
    NUMBER,
    SYMBOL,
    STRING,
    ERROR,
};

struct lispobj {
    int refs;
    int type;
    union {
        int number;
        char *symbol;
        char *string;
        char *error;
        struct cons {
            struct lispobj *car;
            struct lispobj *cdr;
        } *cons;
    } value;
};

#include "../include/fflisp.h"

#define OBJ_TRUE t
#define OBJ_FALSE nil

#define SYMBOL_VALUE(x) ((x)->value.symbol)
#define NUMBER_VALUE(x) ((x)->value.number)
#define STRING_VALUE(x) ((x)->value.string)
#define ERROR_VALUE(x) ((x)->value.error)
#define CONS_VALUE(x) ((x)->value.cons)

#define NEW_SYMBOL(o) (object_create(SYMBOL, (o)))
#define NEW_NUMBER(o) (object_create(NUMBER, (o)))
#define NEW_STRING(o) (object_create(STRING, (o)))
#define NEW_ERROR(o) (object_create(ERROR, (o)))
#define NEW_CONS(car, cdr) (cons((car), (cdr)))

#define CAR(x) (CONS_VALUE((x))->car) // 1st element of list/cons
#define CDR(x) (CONS_VALUE((x))->cdr) // 2d element of cons
#define CADR(x) (CAR(CDR((x)))) // 2d element of list
#define CDDR(x) (CDR(CDR((x)))) // 2d element of cons which it's 2d element of list
#define CADDR(x) (CAR(CDR(CDR((x))))) // 3d element of list
#define CDDDR(x) (CDR(CDR(CDR((x)))))
#define CADDDR(x) (CAR(CDR(CDR(CDR((x))))))

#define OBJ_TYPE(x) ((x)->type)
#define OBJ_REFS(x) ((x)->refs)

struct lispobj *object_create(int, char*);
void object_delete(struct lispobj*);

#endif /* __OBJECT_H__ */
