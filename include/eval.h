/* This file is licensed under the terms of MIT license, see LICENSE file. */

#ifndef __EVAL_H__
#define __EVAL_H__

struct lispobj *eval(struct lispobj*, struct lispobj*);
struct lispobj *apply(struct lispobj*, struct lispobj*);

#endif /* __EVAL_H__ */
