/* This file is licensed under the terms of MIT license, see LICENSE file. */

#include <stdio.h>
#include <stdlib.h>

#include "../include/object.h"
#include "../include/heap.h"
#include "../include/environment.h"
#include "../include/eval.h"
#include "../include/read.h"
#include "../include/print.h"

int load(const char *filename)
{
    FILE *stream;
    char c;

    if((stream = fopen(filename, "r")) == NULL) {
        perror("load");
        return 0;
    }
    
    while((c = fgetc(stream)) != EOF) {
        struct lispobj *read_obj = NULL, *eval_obj = NULL;
        
        ungetc(c, stream);
        
        read_obj = heap_grab(read(stream));
        eval_obj = eval(read_obj, environment);

        if((eval_obj != NULL && OBJ_TYPE(eval_obj) == ERROR) ||
           fgetc(stream) != EOF) {
            print(eval_obj);
            printf("\n");
        }

        heap_release(read_obj);
        heap_release(eval_obj);
    }

    return 1;
}

void repl(FILE *stream)
{
    while("all humans alive") {
        struct lispobj *read_obj = NULL, *eval_obj = NULL;

        // Print prompt
        printf("fflisp> ");
        fflush(stdin);
        
        read_obj = heap_grab(read(stream));

        eval_obj = eval(read_obj, environment);
        
        // Print result
        printf("=> ");
        print(eval_obj);
        printf("\n");
        
        heap_release(read_obj);
        heap_release(eval_obj);

#ifdef __DEBUG_ENV__
        env_debug();
#endif /* __DEBUG_ENV__ */
#ifdef __DEBUG_SYMT__        
        symbol_table_debug();
#endif /* __DEBUG_SYMT__ */
    }

    return;
}
