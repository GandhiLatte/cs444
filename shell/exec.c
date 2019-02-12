#include "shtypes.h"
#include "syscalls.h"
#include "io.h"
#include <dirent.h>
#include <unistd.h>
#include "exec.h"

job_t *head = NULL;


char history[10][150];
int curr_history = 0;

int handle_builtin(struct cmd *c)
{
	if (c == NULL) // c == null;
		return 0;

	switch (c->tp)
	{
	case EXEC:
		if (strcmp(c->exec.argv[0], "echo") == 0)
		{
			if (curr_history < 10)
				strcpy(history[curr_history], c->exec.argv[0]);

			for (int i = 1;; i++)
				if (c->exec.argv[i])
				{
					strcat(history[curr_history], " ");
					strcat(history[curr_history], c->exec.argv[i]);
					printf("%s ", c->exec.argv[i]);
				}
				else
					break;
			printf("\n");
			curr_history++;
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
			strcpy(history[curr_history], c->exec.argv[0]);
			char buff[FILENAME_MAX];
			getcwd(buff, FILENAME_MAX);
			printf("%s \n", buff);
			curr_history++;
			return 1;
		}
		else if (strcmp(c->exec.argv[0], "cd") == 0)
		{
			strcpy(history[curr_history], c->exec.argv[0]);
			if (c->exec.argv[1])
			{
				strcat(history[curr_history], " ");
				strcat(history[curr_history], c->exec.argv[1]);
				chdir(c->exec.argv[1]); // this needs to be completed
			}
			curr_history++;
			return 1;
		}
		else if (strcmp(c->exec.argv[0], "history") == 0)
		{
			for (int i = 0; i <= curr_history; i++)
			{
				printf("%s \n", history[i]);
			}
		} else if(strcmp(c->exec.argv[0], "help") == 0)
		{
			printf("Example Commands\n-help -echo -pwd -ls -cd -exit\n-history is buggy \n-jobs also buggy\n");
		} else if(strcmp(c->exec.argv[0], "jobs") == 0)
		{
			job_t * temp;
			temp = head;

			while(temp != NULL)
			{
				printf("[%d] %s \n", temp->pid, temp->name);
			}
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
		/*for(int i = 0; i <= sizeof(c->exec.argv); i++)
		{
			strcat(history[curr_history], c->exec.argv[i]);
			strcat(history[curr_history], " ");
		}
		curr_history++;
		*/

		if (child == 0)
		{
			if (execvp(c->exec.argv[0], c->exec.argv) == -1)
			{
				exit(0);
			}
			else
			{
				exit(execvp(c->exec.argv[0], c->exec.argv));
			}
		}
		else
		{
			int code;

			/* Avoid spurious deaths--wait for specifically the child with the given PID
		 * to die. This *should* be our only child, but it doesn't hurt to be safe. */
			while (wait(&code) != child)
				;
			return code;
		}
		break;

	case PIPE:
		/* TODO: Run *two* commands, but with the stdout chained to the stdin
	 * of the next command. How? Use the syscall pipe: */
		{
			int child2;
			int pipefds[2];
			pipe(pipefds);
			child = fork1();

			if (child == 0)
			{
				dup2(pipefds[1], 1);
				close(pipefds[0]);
				close(pipefds[1]);
				exec_cmd(c->pipe.left);
			}
			else
			{
				child2 = fork1();
				if (child2 == 0)
				{
					dup2(pipefds[0], 0);
					close(pipefds[0]);
					close(pipefds[1]);
					exec_cmd(c->pipe.right);
				}
				else
				{
					int code;
					int code2;
					close(pipefds[0]);
					close(pipefds[1]);
					waitpid(child, &code, 0);
					waitpid(child2, &code2, 0);
					return code2;
				}
			}

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
		child = fork1();
		if (child == 0)
		{
			int fd = open(c->redir.path, c->redir.mode, 0644);
			if (fd < 0)
			{
				exit(0);
			}
			else
			{
				dup2(fd, c->redir.fd);
				close(fd);
				exit(exec_cmd(c->redir.cmd));
			}
		}
		else
		{
			int code;
			while (wait(&code) != child)
				;
		}

		break;

	case BACK:
		/* This should be easy--what's something you can remove from EXEC to have
	 * this function return before the child exits? */

		child = fork1();
		if (child == 0)
		{
			exit(exec_cmd(c->back.cmd));
		} else
		{
			if(c->back.cmd->tp == EXEC) {
				head = job_add(child, c->back.cmd->exec.argv[0], head);
			}
			
		}
		
		break;

	default:
		fatal("BUG: exec_cmd unknown command type\n");
	}
}

job_t *job_add(int p , char *n, job_t *h)
{

	job_t *node = malloc(sizeof(job_t));
	node->name = n;
	node->pid = p;
	node->next = h;
	return node;
}

job_t *job_remove_last(job_t *h)
{
	job_t *retjob = NULL;

	if(h->next == NULL)
	{
		retjob->name = h->name;
		retjob->pid = h->pid;
		free(h);
		return retjob;
	}

	job_t * current = h;
	while(current->next->next != NULL)
	{
		current = current->next;
	}

	retjob->name = current->next->name;
	retjob->pid = current->next->pid;
	free(current->next);
	current->next = NULL;
	return retjob;
}
