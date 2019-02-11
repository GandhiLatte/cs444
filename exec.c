#include "shtypes.h"
#include "syscalls.h"
#include "io.h"
#include <dirent.h>
#include <unistd.h>

int handle_builtin(struct cmd *c)
{
	if (c == NULL) // c == null;
		return 0;

	switch (c->tp)
	{
	case EXEC:
		if (strcmp(c->exec.argv[0], "echo") == 0)
		{
			for (int i = 1;; i++)
				if (c->exec.argv[i])
					printf("%s ", c->exec.argv[i]);
				else
					break;
			printf("\n");
			return 1;
		}
		else if (strcmp(c->exec.argv[0], "exit") == 0)
		{
			int code = 0;
			if (c->exec.argv[1])
			{
				code = atoi(c->exec.argv[1]);
			}
			exit(code);
		}
		else if (strcmp(c->exec.argv[0], "pwd") == 0)
		{
			/* TODO: implement cd, pwd */
			char buff[FILENAME_MAX];
			getcwd(buff, FILENAME_MAX);
			printf("%s \n", buff);
			return 1;
		}
		else if (strcmp(c->exec.argv[0], "cd") == 0)
		{
			if (c->exec.argv[1])
			{
				chdir(c->exec.argv[1]); // this needs to be completed
			}
			return  1;
		}
	default:
		return 0;
	}
}

/* Most of your implementation needs to be in here, so a description of this
 * function is in order:
 *
 * int exec_cmd(struct cmd *c)
 *
 * Executes a command structure. See shtypes.h for the definition of the
 * `struct cmd` structure.
 *
 * The return value is the exit code of a subprocess, if a subprocess was
 * executed and waited for. (Id est, it should be the low byte of the return
 * value from that program's main() function.) If no return code is collected,
 * the return value is 0.
 *
 * For pipes and lists, the return value is the exit code of the rightmost
 * process.
 *
 * The function does not change the assignment of file descriptors of the
 * current process--but it may fork new processes and change their file
 * descriptors. On return, the shell is expected to remain connected to
 * its usual controlling terminal (via stdin, stdout, and stderr).
 *
 * This will not be called for builtins (values for which the function above
 * returns a nonzero value).
 */
int exec_cmd(struct cmd *c)
{
	// don't try to execute a command if parsing failed.
	if (!c)
		return -1;

	int child;

	switch (c->tp)
	{
	case EXEC:
		/* TODO: What will run a command? How do you make sure the shell
	 * stays around afterward? (Hint: fork, then execvp, then wait; look
	 * at main() for some inspiration.) */
		child = fork1();
		if(child == 0)
		{
			if(execvp(c->exec.argv[0], c->exec.argv) == -1){
			} else
			{
				exit(execvp(c->exec.argv[0], c->exec.argv));
			}
		} else {
			int code;

		/* Avoid spurious deaths--wait for specifically the child with the given PID
		 * to die. This *should* be our only child, but it doesn't hurt to be safe. */
        	while(wait(&code) != child)
            	;
		}
		break;

	case PIPE:
		/* TODO: Run *two* commands, but with the stdout chained to the stdin
	 * of the next command. How? Use the syscall pipe: */
		{
			int pipefds[2];
			pipe(pipefds);



			/* At this point, pipefds[0] is a read-only FD, and pipefds[1] is a
		 * write-only FD. Make *absolutely sure* you close all instances of
		 * pipefds[1] in processes that aren't using it, or the reading child
		 * will never know when it needs to terminate! */
		}
		break;

	case LIST:
		/* Here's a freebie: */
		exec_cmd(c->list.first);
		return exec_cmd(c->list.next);
		break;

	case REDIR:
	 	// c->redir.______  whatever
		/* Redirect a file descriptor, then use it in the command. Note that you
	 * should avoid changing the *shell's* file descriptors, so you probably
	 * want to do this in a process. As a hint, look back at EXEC when you
	 * have that done. */
		break;

	case BACK:
		/* This should be easy--what's something you can remove from EXEC to have
	 * this function return before the child exits? */
		break;

	default:
		fatal("BUG: exec_cmd unknown command type\n");
	}
}
