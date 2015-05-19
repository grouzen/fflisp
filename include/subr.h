/* This file is licensed under the terms of MIT license, see LICENSE file. */

#ifndef __SUBR_H__
#define __SUBR_H__

int length(struct lispobj*);
struct lispobj *cons(struct lispobj*, struct lispobj*);
struct lispobj *list(int, ...);

struct lispobj *subr_newline(struct lispobj*);
struct lispobj *subr_display(struct lispobj*);
struct lispobj *subr_rplaca(struct lispobj*);
struct lispobj *subr_rplacd(struct lispobj*);
struct lispobj *subr_apply(struct lispobj*);
struct lispobj *subr_error(struct lispobj*);
struct lispobj *subr_eval(struct lispobj*);
struct lispobj *subr_read(struct lispobj*);
struct lispobj *subr_load(struct lispobj*);
struct lispobj *subr_car(struct lispobj*);
struct lispobj *subr_cdr(struct lispobj*);
struct lispobj *subr_cons(struct lispobj*);
struct lispobj *subr_pair(struct lispobj*);
struct lispobj *subr_number(struct lispobj*);
struct lispobj *subr_string(struct lispobj*);
struct lispobj *subr_symbol(struct lispobj*);
struct lispobj *subr_atom(struct lispobj*);
struct lispobj *subr_null(struct lispobj*);
struct lispobj *subr_not(struct lispobj*);
struct lispobj *subr_or(struct lispobj*);
struct lispobj *subr_and(struct lispobj*);
struct lispobj *subr_eq(struct lispobj*);
struct lispobj *subr_eql(struct lispobj*);
struct lispobj *subr_list(struct lispobj*);
struct lispobj *subr_plus(struct lispobj*);
struct lispobj *subr_multi(struct lispobj*);
struct lispobj *subr_mod(struct lispobj*);
struct lispobj *subr_compar(struct lispobj*);
struct lispobj *subr_greatthan(struct lispobj*);
struct lispobj *subr_lessthan(struct lispobj*);
struct lispobj *subr_minus(struct lispobj*);
struct lispobj *subr_divide(struct lispobj*);
struct lispobj *subr_equal(struct lispobj*);
struct lispobj *subr_heap(struct lispobj *);
struct lispobj *subr_heap_object(struct lispobj *);

#define ERROR_ARGS object_create(ERROR, "Recieve wrong number of arguments.\n")

/*
#define ERROR_ARGS(n)                                                   \
    do {                                                                \
        char error[64];                                                 \
        snprintf(error, 64, "Recieve wrong number of arguments: %d.\n", (n)); \
        return object_create(ERROR, error);                             \
    } while(0)
*/
#endif /* __SUBR_H__ */
