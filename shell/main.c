#include "tok.h"
#include "io.h"
#include "shtypes.h"
#include "parser.h"
#include "exec.h"
#include "syscalls.h"

#define PROMPT "> "

int
main(int argc, char **argv)
{
  struct cmd *c;
  int child;

  /* Print out a banner telling the user about the shell.
   * (Note: __DATE__ and __TIME__ are somewhat magical preprocessor
   * constants that expand to strings; the compiler statically combines
   * adjacent strings at compile time.) */

  printf("simple sh, " __DATE__ " " __TIME__ "\nPress control+C or control+D to exit.\n");
  printf(PROMPT);
  advance(); // initialize the tokenizer, setting the current token

  while(get_cur() != TOK_EOF)
  {
    c = parse();
	print_cmd(c);
    /* Execute in a subprocess so as to avoid cluttering our file descriptors
     * due to redirections--note that yet more subprocesses may also spawn
     */

    /* builtins need to be handled by the shell process */
    if (handle_builtin(c))
        goto done;

	/* Spawn a new child to execute the command in; this is probably overcautious, but
	 * it (1) guarantees our file descriptors aren't touched, and (2) allows us to catch
	 * "unusual" exits in the child process (other than returns from exec_cmd--for example,
	 * segfaults and other fatal signals). */
    if((child = fork1())) {
		/* This block runs in the parent, with variable child equal to the PID of the
		 * child process below. */
		int code;

		/* Avoid spurious deaths--wait for specifically the child with the given PID
		 * to die. This *should* be our only child, but it doesn't hurt to be safe. */
        while(wait(&code) != child)
            ;

		/* Real shells usually set a shell variable here. */
		printf("command exited with code %d\n", WEXITSTATUS(code));
    } else {
		/* This block runs in the child. */
        exit(exec_cmd(c));
    }

done:
	/* Release any memory associated with the command. */
    free_cmd(c);
	printf(PROMPT);
  }

  return 0;
}
