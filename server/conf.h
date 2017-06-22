#ifndef __CONF_H__
#define __CONF_H__

#include <string.h>
#include <stdio.h>
#include "unp.h"
#include "../common/common.h"

// configure file path
#define CONF_FILE "/root/unp/server/configure"

// check login username and password
int check_login(char *loginstr);

// check cwd is workdir
int check_workdir(char *dir);

// get capability
int get_capab();

// locate label in file to support above functions
FILE *lab_locate(char *label);

#endif // conf.h