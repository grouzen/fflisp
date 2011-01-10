/* This file is licensed under the terms of MIT license, see LICENSE file. */

#ifndef __ENVIRONMENT_H__
#define __ENVIRONMENT_H__

#define ENV_FIRST(env) (CAR((env)))
#define ENV_REST(env) (CDR((env)))

struct lispobj *env_var_lookup(struct lispobj*, struct lispobj*);
struct lispobj *env_var_assign(struct lispobj*, struct lispobj*, struct lispobj*);
struct lispobj *env_var_define(struct lispobj*, struct lispobj*, struct lispobj*);
struct lispobj *env_val_list(struct lispobj*, struct lispobj*);
struct lispobj *env_proc_make(struct lispobj*, struct lispobj*, struct lispobj*);
struct lispobj *env_frame_make(struct lispobj*, struct lispobj*);
struct lispobj *env_init(void);
#ifdef __DEBUG_ENV__
void env_debug(void);
#endif /* __DEBUG_ENV__ */

#endif /* __ENVIRONMENT_H__ */
