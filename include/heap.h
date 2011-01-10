/* This file is licensed under the terms of MIT license, see LICENSE file. */

#ifndef __HEAP_H__
#define __HEAP_H__

struct heap {
    struct lispobj **data;
    int index;
    int size;
};

struct heap *heap_init(void);
struct lispobj *heap_add(struct lispobj*);
void heap_remove(struct lispobj*);
void heap_clean(void);
struct lispobj *heap_grab(struct lispobj*);
void heap_release(struct lispobj*);
struct lispobj *symbol_table_intern(struct lispobj*);
struct lispobj *symbol_table_lookup(char*);
#ifdef __DEBUG_SYMT__
void symbol_table_debug(void);
#endif /* __DEBUG_SYMT__ */
#ifdef __DEBUG_HEAP__
void heap_debug(void);
#endif /* __DEBUG_HEAP__ */

#define HEAP_SIZE (2 << 10)

#endif /* __HEAP_H__ */
