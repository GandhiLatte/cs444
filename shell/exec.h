#ifndef EXEC_H
#define EXEC_H

#include "shtypes.h"


typedef struct jobs {
	int pid;
	char *name;
	struct jobs *next;
} job_t;

job_t *job_add(int , char *, job_t *);

int handle_builtin(struct cmd *);
int exec_cmd(struct cmd *c);

#endif
