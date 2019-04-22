/*
 * layout_server.h
 *
 *  Created on: 21 Apr 2019
 *      Author: gal
 */

#ifndef PLUGINS_LAYOUT_SERVER_H_
#define PLUGINS_LAYOUT_SERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "plugin_manager.h"


#define OPENSSL_EXAMPLE_TASK_NAME        "openssl_example"
#define OPENSSL_EXAMPLE_TASK_STACK_WORDS 10240
#define OPENSSL_EXAMPLE_TASK_PRIORITY    8

#define OPENSSL_EXAMPLE_RECV_BUF_LEN       1024

#define OPENSSL_EXAMPLE_LOCAL_TCP_PORT     443

//Initialize the layout server
void init_layout_server(void);

#ifdef __cplusplus
}
#endif

#endif /* PLUGINS_LAYOUT_SERVER_H_ */
