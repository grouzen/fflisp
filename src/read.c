/* This file is licensed under the terms of MIT license, see LICENSE file. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* for memset() */

#include "../include/object.h"
#include "../include/subr.h"
#include "../include/heap.h"

static struct lispobj *read_list(FILE*);
static struct lispobj *read_token(FILE*);
static struct lispobj *read_quote(FILE*);
static struct lispobj *read_string(FILE*);
static void token_to_upper_case(char*);

#define IS_NUMBER(c) (c >= '0' && c <= '9')

#define IS_SYMBOL(c)                                                    \
    ((c >= 'a' && c <= 'z') ||                                          \
     (c >= 'A' && c <= 'Z') ||                                          \
     c == '+' || c == '-' || c == '*' || c == '/' ||                    \
     c == '>' || c == '<' || c == '=' || c == '\'')

#define IS_LEGAL(c)                             \
    ((c >= 'a' && c <= 'z') ||                  \
     (c >= 'A' && c <= 'Z') ||                  \
     (c >= '0' && c <= '9') ||                  \
     (c >= '(' && c <= '/') ||                  \
     (c >= '<' && c <= '>') ||                  \
     c == '\'' || c == '"' || c == ';')

#define IS_WHITESPACE(c)                                            \
    (c == '\t' || c == '\n' || c == ' ' || c == '\r' || c == '\b')

#define IS_MACRO(c) (c == '(' || c == ')' || c == '\'' || c == '"' || c == ';')

#define IS_CONSTITUENT(c)                               \
    ((c >= 'a' && c <= 'z') ||                          \
     (c >= 'A' && c <= 'Z') ||                          \
     (c >= '0' && c <= '9') ||                          \
     c == '+' || c == '-' || c == '*' || c == '=' ||    \
     c == '/' || c == '<' || c == '>' || c == '\'')

#define ERROR_ILLEGAL(c)                                        \
    do {                                                        \
        char error[32];                                         \
        snprintf(error, 32, "Illegal character: %c.\n", (c));   \
        return NEW_ERROR(error);                                \
    } while(0)

#define ERROR_UNMATCHED_BRACKETS NEW_ERROR("Unmatched paranthesis.\n")

int brackets = 0;
int quotes = 0;

struct lispobj *read(FILE *stream)
{
    struct lispobj *obj = NULL;
    char c;
    
    while((c = fgetc(stream)) != EOF) {
        if(IS_WHITESPACE(c))
            continue;
        
        if(!IS_LEGAL(c)) {
            ERROR_ILLEGAL(c);
        }
        
        if(IS_MACRO(c)) {
            if(c == '\'') {
                obj = read_quote(stream);
                break;
            } else if(c == '"') {
                quotes++;
                obj = read_string(stream);
                break;
            } else if(c == '(') {
                brackets++;
                obj = read_list(stream);

                break;
            } else if(c == ')') {
                // NIL
                if(!brackets) {
                    brackets = 0;
                    return ERROR_UNMATCHED_BRACKETS;
                }
                brackets--;
                break;
            } else if(c == ';') {
                while((c = fgetc(stream)) != '\n' && c != EOF);
                continue;
            }
        }

        if(IS_CONSTITUENT(c)) {
            ungetc(c, stream);
            obj = read_token(stream);

            break;
        }
    }

    return obj;
}

static struct lispobj *read_list(FILE *stream)
{
    struct lispobj *list, *ret;
    char c;
    
    ret = read(stream);
    if(ret == NULL) {
        return NEW_SYMBOL("NIL");
    } else if(OBJ_TYPE(ret) == ERROR) {
        return ret;
    }
    
    list = NEW_CONS(NULL, NULL);
    CAR(list) = heap_grab(ret);

    while((c = fgetc(stream)) != ')') {
        if(c == EOF) {
            heap_release(list);
            return ERROR_UNMATCHED_BRACKETS;
        } else if(IS_WHITESPACE(c)) {
            continue;
        } else if(IS_CONSTITUENT(c) ||
                  c == '(' || c == '"' || c == ';') {
            ungetc(c, stream);
            ret = read_list(stream);
            if(OBJ_TYPE(ret) == ERROR) {
                heap_release(list);
                return ret;
            }
            CDR(list) = heap_grab(ret);
            return list;
        } else {
            heap_release(list);
            ERROR_ILLEGAL(c);
        }
    }
    brackets--;
    CDR(list) = NULL;
    
    return list;
}

static struct lispobj *read_quote(FILE *stream)
{
    /* Create (quote read(stream)). */
    return NEW_CONS(NEW_SYMBOL("QUOTE"), NEW_CONS(read(stream), NULL));
}

#define STRING_LENGTH 0x10 // I'm fucking bitch I know ;3

static struct lispobj *read_string(FILE *stream)
{
    struct lispobj *string = NULL;
    char *s, c;
    int i, s_length = STRING_LENGTH;
    
    s = malloc(sizeof(char) * STRING_LENGTH);
    memset(s, 0, STRING_LENGTH);
    
    for(i = 0; (c = fgetc(stream)) != EOF; i++) {
        if(i >= s_length) {
            s = realloc(s, s_length + STRING_LENGTH);
            s_length += STRING_LENGTH;
        }

        if(c == '"') {
            if(s[i - 1] == '\\') {
                s[--i] = c;
                continue;
            }
            
            quotes--;
            s[i] = '\0';
            string = NEW_STRING(s);
            
            break;
        }
        s[i] = c;
    }
    free(s);

    /* If string != NULL, this will never happen. */
    if(quotes) {
        quotes = 0;
        return NEW_ERROR("Unmatched quotes.\n");
    }

    return string;
}

static struct lispobj *read_token(FILE *stream)
{
    struct lispobj *token;
    char token_string[30], c;
    int i = 0, token_type = 0;

    while((c = fgetc(stream)) != EOF) {
        if(IS_MACRO(c) || IS_WHITESPACE(c)) {
            ungetc(c, stream);
            break;
        }
        if(i < 30) {
            if(!token_type) {
                if(!i && (c == '-' || c == '+')) {
                    // just accumulate token
                } else if(IS_NUMBER(c)) {
                    token_type = NUMBER;
                } else if(IS_SYMBOL(c)) {
                    token_type = SYMBOL;
                } else {
                    ERROR_ILLEGAL(c);
                }
            } else if(token_type == SYMBOL) {
                if(!IS_SYMBOL(c) && !IS_NUMBER(c))
                    ERROR_ILLEGAL(c);
            } else { // NUMBER
                if(!IS_NUMBER(c))
                    ERROR_ILLEGAL(c);
            }
            token_string[i] = c;
        } else {
            return NEW_ERROR("Length of token must be less"
                             " then 30 characters.\n");
        }
        
        i++;
    }
    token_string[i] = '\0';

    if(token_type == NUMBER) {
        token = NEW_NUMBER(token_string);
    } else {
        token_to_upper_case(token_string);
        token = NEW_SYMBOL(token_string);
    }

    return token;
}

static void token_to_upper_case(char *token)
{
    int i = 0;
    
    while(token[i] != '\0') {
        if(token[i] >= 'a' && token[i] <= 'z') {
            token[i] -= 0x20;
        }
        i++;
    }

    return;
}
