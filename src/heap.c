/* This file is licensed under the terms of MIT license, see LICENSE file. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/object.h"
#include "../include/subr.h"
#include "../include/heap.h"

static void symbol_table_delete(struct lispobj*);
static void heap_grow(void);

//#ifdef __DEBUG_HEAP__
void heap_debug_object(struct lispobj *obj)
{
    if(obj == NULL) {
        printf(" null pointer");
    } else {
        printf(" [0x%x ", obj);
        if(OBJ_TYPE(obj) == SYMBOL) {
            printf("(symbol %s) ", SYMBOL_VALUE(obj));
        } else if(OBJ_TYPE(obj) == NUMBER) {
            printf("(number %d) ", NUMBER_VALUE(obj));
        } else if(OBJ_TYPE(obj) == STRING) {
            printf("(string %s) ", STRING_VALUE(obj));
        } else {
            printf("(cons) ");
        }
        printf("%d] ", OBJ_REFS(obj));
    }
}

void heap_debug(void)
{
    int i = 0;
    while(i < heap->index) {
        heap_debug_object(heap->data[i]);
        i++;
    }

    printf("\nTotal: %d objects (%d bytes).\n",
           i,
           i * sizeof(struct lispobj));
    
    return;
}
//#endif /* __DEBUG_HEAP__ */

struct heap *heap_init(void)
{
    struct heap *h;
    
    h = malloc(sizeof(struct heap));
    h->data = malloc(sizeof(struct lispobj *) * HEAP_SIZE);
    memset(h->data, 0, sizeof(struct lispobj *) * HEAP_SIZE);
    h->index = 0;
    h->size = HEAP_SIZE;

    return h;
}

struct lispobj *heap_add(struct lispobj *obj)
{
    if(heap->index >= heap->size) {
        heap_grow();
    }

    heap->data[heap->index] = obj;
    heap->index++;

    return obj;
}

static void heap_grow(void)
{
    struct heap *new_heap = malloc(sizeof(struct heap));

    new_heap->data = malloc(sizeof(struct lispobj *) * (heap->size * 2));
    memset(new_heap->data, 0, sizeof(struct lispobj *) * (heap->size * 2));
    new_heap->size = heap->size * 2;
    new_heap->index = 0;

    while(new_heap->index < heap->index) {
        new_heap->data[new_heap->index] = heap->data[new_heap->index];
        new_heap->index++;
    }

    free(heap->data);
    free(heap);

    heap = new_heap;

    return;
}

void heap_remove(struct lispobj *obj)
{
    int i = 0;
    while(i < heap->size) {
        /* Try to find object in the heap. */
        if(obj == heap->data[i]) {
            /* If object found,
               assign current heap element to null,
               decrement heap's index.
            */
            heap->data[i] = heap->data[heap->index - 1];
            heap->data[heap->index - 1] = NULL;
            heap->index--;
            
            return;
        }
        i++;
    }

    return;
}

void heap_clean(void)
{
    while(heap->index > 0) {
        /* Call heap_remove() while the heap not will become empty. */
        object_delete(heap->data[0]);
    }

    return;
}

struct lispobj *heap_grab(struct lispobj *obj)
{
    if(obj != NULL) {
        OBJ_REFS(obj)++;
    }

    return obj;
}

void heap_release(struct lispobj *obj)
{
    if(obj != NULL) {
        OBJ_REFS(obj)--;
    
        if(OBJ_REFS(obj) <= 0) {
            /* If object is a symbol delete it from symbol table. */
            if(OBJ_TYPE(obj) == SYMBOL)
                symbol_table_delete(obj);
            
            object_delete(obj);
        }
    }
    return;
}

#ifdef __DEBUG_SYMT__
void symbol_table_debug(void)
{
    struct lispobj *tmp_symt;
    tmp_symt = symbol_table;
    
    while(tmp_symt != NULL) {
        printf(" [%s %d] ",
               SYMBOL_VALUE(CAR(tmp_symt)),
               OBJ_REFS(CAR(tmp_symt)));
        tmp_symt = CDR(tmp_symt);
    }
    printf("\n");
    
    return;
}
#endif /* __DEBUG_SYMT__ */

static void symbol_table_delete(struct lispobj *symbol)
{
    struct lispobj *tmp_symt, *prev_cons;
    
    tmp_symt = symbol_table;
    prev_cons = NULL;

    /* Try to find the necessary symbol in the symbol table. */
    while(tmp_symt != NULL) {
        struct lispobj *curs = CAR(tmp_symt);

        /* If have found it delete it. */
        if(curs == symbol) {
            if(prev_cons != NULL) {
                CDR(prev_cons) = CDR(tmp_symt);
            } else {
                symbol_table = CDR(tmp_symt);
            }
                            
            CAR(tmp_symt) = NULL;
            CDR(tmp_symt) = NULL;
            object_delete(tmp_symt);
            
            return;
        }

        prev_cons = tmp_symt;
        tmp_symt = CDR(tmp_symt);
    }
    
    return;
}

struct lispobj *symbol_table_intern(struct lispobj *symbol)
{
    struct lispobj *pair;

    /* Inserting new symbol on the top of the symbol table. */
    pair = NEW_CONS(symbol, symbol_table);
    symbol_table = heap_grab(pair);

    /* Just return symbol. */
    return symbol;
}

struct lispobj *symbol_table_lookup(char *symbol)
{
    struct lispobj *tmp = symbol_table;
    
    while(tmp != NULL) {
        if(!strcmp(SYMBOL_VALUE(CAR(tmp)), symbol)) {
            return CAR(tmp);
        }
        tmp = CDR(tmp);
    }

    return NULL;
}
