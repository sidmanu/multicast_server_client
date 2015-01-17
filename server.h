#ifndef _CAPS_SERVER_H
#define _CAPS_SERVER_H

#include "common_hdr.h"

/* Thread functions */
void *client_handler(void *t_args);
void *periodic_print_t(void *t_args);

/* Thread functions */
extern void *periodic_print_thread_fn(void *t_args);
extern void *user_interactor_thread_fn(void *t_args);
/*chaitanya : */
extern void *heartbeat_thread_fn(void *t_args);
void * client_handler_thread(void *t_args);

/* Tasks */
void task_menu();


void quit_app();

#endif
