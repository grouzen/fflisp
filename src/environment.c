/* This file is licensed under the terms of MIT license, see LICENSE file. */

#include <stdio.h>
#include <stdlib.h>

#include "../include/object.h"
#include "../include/heap.h"
#include "../include/subr.h"
#include "../include/eval.h"
#include "../include/environment.h"

/*
 * Representation of environment is like a s-exp:
 * ( ((a . 1) (foo . <PROC>) (bar . <SUBR>))   ((b . 2)) ...)
 * ^ ^                                         ^         ^
 * | Top context's frame.                      Rest context's frames.
 * Whole environment.
 *
 * Representation of PROC:
 * (proc (x) (* x x) <env>)
 * Representation of SUBR:
 * (subr <address>)
 */

struct subrs {
    char *var;
    struct lispobj *(*val)(struct lispobj*);
};

static struct lispobj *env_subr_init(struct subrs*, int, int);

#ifdef __DEBUG_ENV__
void env_debug(void)
{
    struct lispobj *tmp_env = environment;
    
    while(tmp_env != NULL) {
        struct lispobj *frame = ENV_FIRST(tmp_env);

        printf(" (");
        while(frame != NULL) {
            struct lispobj *cell = CAR(frame);
            
            printf(" [%s %d; 0x%x %d] ",
                   SYMBOL_VALUE(CAR(cell)),
                   OBJ_REFS(CAR(cell)),
                   CDR(cell),
                   CDR(cell) != NULL ? OBJ_REFS(CDR(cell)) : -1);
            
            frame = CDR(frame);
        }
        printf(") ");
        tmp_env = CDR(tmp_env);
    }

    printf("\n");
}
#endif /* __DEBUG_ENV__ */

struct lispobj *env_var_lookup(struct lispobj *var, struct lispobj *env)
{
    struct lispobj *frame, *cell;
    char error[64];
    
    while(env != NULL) {
        frame = ENV_FIRST(env);
        while(frame != NULL) {
            cell = CAR(frame);

            if(CAR(cell) == var) {
                /* Return whole cell, e.g. (foo . 1). */
                return cell;
            }

            frame = CDR(frame);
        }
        
        env = ENV_REST(env);
    }
    
    snprintf(error, 64, "Unbound variable: %s.\n", SYMBOL_VALUE(var));
    
    return NEW_ERROR(error);
}

struct lispobj *env_var_assign(struct lispobj *var, struct lispobj *val, struct lispobj *env)
{
    struct lispobj *cell;

    if(var == NULL || OBJ_TYPE(var) != SYMBOL) {
        return NEW_ERROR("Variable name is not a symbol.\n");
    }
    /* Checking on variable existence. */ 
    cell = env_var_lookup(var, env);
    /* If variable not exists return error. */
    if(OBJ_TYPE(cell) == ERROR) {
        return cell;
    }
    /* Remove old value. */
    heap_release(CDR(cell));
    /* Assign new value. */
    CDR(cell) = heap_grab(val);

    return val;
}

struct lispobj *env_var_define(struct lispobj *var, struct lispobj *val, struct lispobj *env)
{
    struct lispobj *frame, *pair, *cell, *lookup;

    /* Checking on variable existence. */
    lookup = env_var_lookup(var, env);
    /* If variable exists return error. */
    if(OBJ_TYPE(lookup) != ERROR) {
        char error[64];
        
        snprintf(error, 64, "Variable already exists: %s.\n", SYMBOL_VALUE(var));
        return NEW_ERROR(error);
    }
    /* Remove not necessary object. */
    heap_release(lookup);

    /* Get top frame from environment. */
    frame = ENV_FIRST(env);

    /* Creating cell for new variable. */
    cell = NEW_CONS(var, val);
    /* Appending new cell into the frame. */
    pair = NEW_CONS(cell, frame);
    frame = heap_grab(pair);

    /* Appending the frame into the environment. */
    CAR(env) = frame;
    
    return val;
}

struct lispobj *env_val_list(struct lispobj *vars, struct lispobj *env)
{
    if(vars != NULL) {
        struct lispobj *vals, *car, *cdr;
        
        car = eval(CAR(vars), env);
        if(car != NULL && OBJ_TYPE(car) == ERROR) {
            return car;
        }
        vals = NEW_CONS(NULL, NULL);
        CAR(vals) = car;

        cdr = env_val_list(CDR(vars), env);
        if(cdr != NULL && OBJ_TYPE(cdr) == ERROR) {
            heap_release(vals);
            return cdr;
        }
        CDR(vals) = heap_grab(cdr);

        return vals;
    }

    return NULL;
}

struct lispobj *env_proc_make(struct lispobj *params, struct lispobj *body, struct lispobj *env)
{
    return list(4, NEW_SYMBOL("PROC"), params, body, env);
}

struct lispobj *env_frame_make(struct lispobj *vars, struct lispobj *vals)
{
    struct lispobj *frame, *tmp;

    frame = NEW_CONS(NULL, NULL);
    tmp = frame;

    while(vars != NULL) {
        struct lispobj *cell;

        cell = NEW_CONS(CAR(vars), CAR(vals));
        
        CAR(tmp) = heap_grab(cell);
        vars = CDR(vars);
        vals = CDR(vals);
        
        if(vars != NULL) {
            CDR(tmp) = heap_grab(NEW_CONS(NULL, NULL));
            tmp = CDR(tmp);
        }
    }
    CDR(tmp) = NULL;

    return frame;
}

struct lispobj *env_init(void)
{
    struct lispobj *frame, *cell, *tmp, *env = NULL;
    static struct subrs s[] = {{"CAR", subr_car},
                               {"CDR", subr_cdr},
                               {"CONS", subr_cons},
                               {"PAIR", subr_pair},
                               {"STRING", subr_string},
                               {"NUMBER", subr_number},
                               {"SYMBOL", subr_symbol},
                               {"ATOM", subr_atom},
                               {"NULL", subr_null},
                               {"NOT", subr_not},
                               {"OR", subr_or},
                               {"AND", subr_and},
                               {"EQ", subr_eq},
                               {"EQL", subr_eql},
                               {"LIST", subr_list},
                               {"+", subr_plus},
                               {"-", subr_minus},
                               {"*", subr_multi},
                               {"=", subr_compar},
                               {">", subr_greatthan},
                               {"<", subr_lessthan},
                               {"/", subr_divide},
                               {"MOD", subr_mod},
                               {"HEAP", subr_heap},
                               {"HEAP-OBJECT", subr_heap_object},
                               {"LOAD", subr_load},
                               {"READ", subr_read},
                               {"EVAL", subr_eval},
                               {"ERROR", subr_error},
                               {"APPLY", subr_apply},
                               {"DISPLAY", subr_display},
                               {"NEWLINE", subr_newline},
                               {"RPLACA", subr_rplaca},
                               {"RPLACD", subr_rplacd},
                               {"EQUAL", subr_equal}};
    
    frame = heap_grab(env_subr_init(s, sizeof(s) / sizeof(struct subrs), 0));
    
    env = NEW_CONS(frame, NULL);
    
    env_var_define(NEW_SYMBOL("T"), NEW_SYMBOL("T"), env);
    env_var_define(NEW_SYMBOL("NIL"), NULL, env);
    
    return env;
}

static struct lispobj *env_subr_init(struct subrs *s, int size, int i)
{
    if(i < size) {
        struct lispobj *cell, *frame, *val;
        char num[32];
        
        snprintf(num, 32, "%d", (int) s[i].val);
        val = list(2, NEW_SYMBOL("SUBR"), NEW_NUMBER(num));

        cell = NEW_CONS(NEW_SYMBOL(s[i].var), val);
        
        frame = NEW_CONS(cell, env_subr_init(s, size, i + 1));
        
        return frame;
    }
    
    return NULL;
}
