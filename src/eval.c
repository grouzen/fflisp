/* This file is licensed under the terms of MIT license, see LICENSE file. */

#include <stdio.h>
#include <stdlib.h>

#include "../include/object.h"
#include "../include/heap.h"
#include "../include/subr.h"
#include "../include/environment.h"
#include "../include/eval.h"

static struct lispobj *eval_progn(struct lispobj*, struct lispobj*);
static struct lispobj *eval_cond(struct lispobj *, struct lispobj*);
static struct lispobj *eval_let(struct lispobj*, struct lispobj*);

struct lispobj *eval(struct lispobj *obj, struct lispobj *env)
{
    struct lispobj *ret;
    
    if(obj == NULL || OBJ_TYPE(obj) == NUMBER ||
       OBJ_TYPE(obj) == ERROR || OBJ_TYPE(obj) == STRING) {
        /* Return self-evaluating object. */
        ret = heap_grab(obj);
    } else if(OBJ_TYPE(obj) == SYMBOL) {
        /* Lookup value of the variable in the env. */
        struct lispobj *val;

        val = env_var_lookup(obj, env);
        if(OBJ_TYPE(val) == ERROR) {
            ret = heap_grab(val);
        } else {
            ret = heap_grab(CDR(val));
        }
    } else if(NEW_SYMBOL("QUOTE") == CAR(obj)) {
        /* (quote whatever) */
        if(length(obj) != 2) {
            ret = heap_grab(ERROR_ARGS);
        } else {
            /* Return quoted object. */
            ret = heap_grab(CADR(obj));
        }
#ifdef __DEBUG_GC__
        printf("eval quote debug:");
        heap_debug_object(ret);
        printf("\n");
#endif
    } else if(NEW_SYMBOL("SETQ") == CAR(obj)) {
        /* (setq var val) */
        if(length(obj) != 3) {
            ret = heap_grab(ERROR_ARGS);
        } else {
            /* Try to assign existing variable. */
            struct lispobj *val;
            
            val = eval(CADDR(obj), env);
            if(val != NULL && OBJ_TYPE(val) == ERROR) {
                ret = val;
            } else {
                ret = heap_grab(env_var_assign(CADR(obj), val, env));
                heap_release(val);
            }
        }
    } else if(NEW_SYMBOL("LABEL") == CAR(obj)) {
        /* (label var val) */
        if(length(obj) != 3) {
            ret = heap_grab(ERROR_ARGS);
        } else {
            /* Try to define new variable. */
            struct lispobj *val;

            val = eval(CADDR(obj), env);
            if(val != NULL && OBJ_TYPE(val) == ERROR) {
                ret = val;
            } else {
                ret = heap_grab(env_var_define(CADR(obj), val, env));
                heap_release(val);
            }
        }
    } else if(NEW_SYMBOL("IF") == CAR(obj)) {
        /* (if predicate consequence alternative) */
        if(length(obj) != 4) {
            ret = heap_grab(ERROR_ARGS);
        } else {
            /* Invoke condition function. */
            struct lispobj *pred;

            pred = eval(CADR(obj), env);
            if(pred != NULL && OBJ_TYPE(pred) == ERROR) {
                ret = pred;
            } else {
                if(pred) {
                    /* Eval consequence. */
                    ret = eval(CADDR(obj), env);
                } else {
                    /* Eval alternative. */
                    ret = eval(CADDDR(obj), env);
                }

                heap_release(pred);
            }
        }
    } else if(NEW_SYMBOL("COND") == CAR(obj)) {
        /* (cond (cond1 ret1) (cond2 ret2)) */
        if(length(obj) < 2) {
            ret = heap_grab(ERROR_ARGS);
        } else {
            ret = eval_cond(CDR(obj), env);
        }
    }
    else if(NEW_SYMBOL("LET") == CAR(obj)) {
        if(length(obj) < 3) {
            ret = heap_grab(ERROR_ARGS);
        } else {
            ret = eval_let(CDR(obj), env);
        }
    } else if(NEW_SYMBOL("PROGN") == CAR(obj)) {
        ret = eval_progn(CDR(obj), env);
    } else if(NEW_SYMBOL("LAMBDA") == CAR(obj)) {
        /* (lambda (var) (proc var var)) */
        if(length(obj) < 3) {
            ret = heap_grab(ERROR_ARGS);
        } else {
            /* Make and return new procedure. */
            ret = heap_grab(env_proc_make(CADR(obj), CDDR(obj), env));
        }
    } else {
        /* Apply case. */
        struct lispobj *proc = eval(CAR(obj), env);
        
        if(proc != NULL && OBJ_TYPE(proc) == ERROR) {
            ret = proc;
        } else {
            struct lispobj *args = heap_grab(env_val_list(CDR(obj), env));

            if(args != NULL && OBJ_TYPE(args) == ERROR) {
                ret = args;
            } else {
                ret = apply(proc, args);
                heap_release(args);
            }
            
            heap_release(proc);
        }
    }
    
    return ret;
}

struct lispobj *apply(struct lispobj *proc, struct lispobj *args)
{   
    if(proc != NULL && OBJ_TYPE(proc) == CONS) {
        struct lispobj *ret;
        
        if(NEW_SYMBOL("SUBR") == CAR(proc)) {
            /* Apply primitive function. */
            struct lispobj *body, *(*subr)(struct lispobj *);

            body = CADR(proc);
            subr = (void *) NUMBER_VALUE(body);

            ret = heap_grab(subr(args));
        } else if(NEW_SYMBOL("PROC") == CAR(proc)) {
            /* Apply user defined procedure. */
            struct lispobj *body, *params, *penv;

            body = CADDR(proc);
            params = CADR(proc);
            penv = CADDDR(proc);

            if(length(params) == length(args)) {
                struct lispobj *env;

                if(params == NULL) {
                    env = penv;

                    ret = eval(body, env);
                } else {
                    env = heap_grab(NEW_CONS(env_frame_make(params, args), penv));

                    ret = eval_progn(body, env);
                    heap_release(env);
                }
            } else {
                char error[64]; 
                snprintf(error,
                         64,
                         "Has recieved wrong number of parameters: %d.\n",
                         length(args));
                ret = heap_grab(NEW_ERROR(error));
            }
        } else {
            goto error;
        }

        return ret;
    }
    
    error:
    return heap_grab(NEW_ERROR("Unknown procedure.\n"));
}

static struct lispobj *eval_progn(struct lispobj *exps, struct lispobj *env)
{
    if(exps == NULL) {
        return exps;
    } else if(CDR(exps) == NULL) {
        return eval(CAR(exps), env);
    } else {
        eval(CAR(exps), env);
        return eval_progn(CDR(exps), env);
    }
}

static struct lispobj* eval_cond(struct lispobj *exps, struct lispobj *env)
{
    struct lispobj *ret = OBJ_FALSE;
    
    if(exps != NULL) {
        struct lispobj *cond;
    
        cond = CAR(exps);
        if(cond != NULL && OBJ_TYPE(cond) == CONS) {
            struct lispobj *pred;

            pred = eval(CAR(cond), env);
            if(pred != NULL && OBJ_TYPE(pred) == ERROR) {
                ret = pred;
            } else {
                if(pred) {
                    if(length(cond) == 1) {
                        ret = OBJ_TRUE;
                    } else {
                        ret = eval(CADR(cond), env);
                    }
                } else {
                    ret = eval_cond(CDR(exps), env);
                }
                heap_release(pred);
            }
        } else {
            ret = NEW_ERROR("Bad cond clause.\n");
        }
    }

    return ret;
}

struct lispobj *eval_let(struct lispobj *exps, struct lispobj *env)
{
    struct lispobj *binds, *body, *vars, *vals, *lambda, *ret, *evals;

    binds = CAR(exps);
    body = CDR(exps);

    if(length(binds) > 0) {
        struct lispobj *tvars, *tvals;
        
        vars = heap_grab(NEW_CONS(NULL, NULL));
        vals = heap_grab(NEW_CONS(NULL, NULL));
        tvars = vars; tvals = vals;
        
        while(binds != NULL) {
            struct lispobj *bind = CAR(binds);

            if(length(bind) != 2) {
                ret = NEW_ERROR("Bad binding in the let exp.\n");
                goto exit;
            }
            
            CAR(tvars) = heap_grab(CAR(bind));
            CAR(tvals) = heap_grab(CADR(bind));
            CDR(tvars) = heap_grab(NEW_CONS(NULL, NULL));
            CDR(tvals) = heap_grab(NEW_CONS(NULL, NULL));
            
            tvars = CDR(tvars);
            tvals = CDR(tvals);
            binds = CDR(binds);
        }

        tvars = NULL;
        tvals = NULL;
    } else {
        return NEW_ERROR("Empty bindgings in the let exp.\n");
    }

    lambda = heap_grab(env_proc_make(vars, body, env));
    
    evals = heap_grab(env_val_list(vals, env));
    if(evals != NULL && OBJ_TYPE(evals) == ERROR) {
        ret = evals;
    } else {
        ret = apply(lambda, evals);
        heap_release(evals);
    }

    heap_release(lambda);
    
    exit:
    heap_release(vals);
    heap_release(vars);

    return ret;
}
