/* This file is licensed under the terms of MIT license, see LICENSE file. */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>

#include "../include/object.h"
#include "../include/environment.h"
#include "../include/heap.h"
#include "../include/repl.h"

#define VERSION "0.0.0rc7"
/* global symbol table */
struct lispobj *symbol_table = NULL;
/* global environment */
struct lispobj *environment = NULL;
/* global pointer to NIL */
struct lispobj *nil = NULL;
/* global pointer to T */
struct lispobj *t = NULL;
/* global pointer to heap */
struct heap *heap = NULL;

static void usage(void)
{
    printf("Usage: fflisp [--load filename] [--help].\n");
    printf("       --load eval code from file.\n");
    printf("       --help print help message.\n");

    return;
}

static void welcome(void)
{
    printf("Welcome to FFLISP " VERSION
           " <http://bitbucket.org/grouzen/fflisp/>.\n");
    printf("Nedokushev Michael <grouzen.hexy@gmail.com> (c) 2010.\n");
    printf("\n");

    printf("    (IN LISP WE TRUST)           \n");
    printf("    __________  __________ (*)   \n");
    printf("   / __/ __/ / /_/ __/ _ / /_\\  \n");
    printf("  / __/ __/ /_/ /__ / __/ /___\\ \n");
    printf(" /_/ /_/ /___/_/___/_/   /_____\\\n");
    printf("\n");

    return;
}

void sigint_handler(int signum)
{
    heap_clean();
    printf("\nBye Bye!\n");
    exit(EXIT_SUCCESS);

    return;
}

int main(int argc, char *argv[])
{
    int opt;
    static struct option long_options[] = {
        {"load", 1, NULL, 'l'},
        {"help", 0, NULL, 'h'},
        {0, 0, 0, 0}
    };

    signal(SIGINT, sigint_handler);

    /* Initialize heap. */
    heap = heap_init();
    /* Define global alias to TRUE object. */
    t = heap_grab(NEW_SYMBOL("T"));
    /* Define global alias to NIL object. */
    nil = NULL;
    /* Configure environment. */
    environment = heap_grab(env_init());
    
    welcome();
    
    while((opt = getopt_long_only(argc, argv, "", long_options, NULL)) != -1) {
        switch(opt) {
        case 'l':
#if 0
            printf("load file: %s.\n", optarg);
            if(load(optarg)) {
                printf("file loaded.\n");
            } else {
                printf("file loading failed.\n");
            }
#endif
            load(optarg);
            
            break;
        case 'h':
        default:
            usage();
            heap_clean();
            exit(EXIT_FAILURE);
        }
    }
    
    repl(stdin);
    return 0;
}
