#include "shtypes.h"
#include "mem.h"
#include "io.h"

#define NEW_CMD calloc(1, sizeof(struct cmd))

struct cmd *new_exec(char **argv)
{
  struct cmd *c = NEW_CMD;
  c->tp = EXEC;
  c->exec.argv = argv;  /* XXX: Taken */
  return c;
}

struct cmd *new_pipe(struct cmd *left, struct cmd *right)
{
  struct cmd *c = NEW_CMD;
  c->tp = PIPE;
  c->pipe.left = left;
  c->pipe.right = right;
  return c;
}

struct cmd *new_list(struct cmd *first, struct cmd *next)
{
  struct cmd *c = NEW_CMD;
  c->tp = LIST;
  c->list.first = first;
  c->list.next = next;
  return c;
}

struct cmd *new_redir(struct cmd *cmd, char *path, int mode, int fd)
{
  struct cmd *c = NEW_CMD;
  c->tp = REDIR;
  c->redir.cmd = cmd;
  c->redir.path = path;  /* XXX: Taken */
  c->redir.mode = mode;
  c->redir.fd = fd;
  return c;
}

struct cmd *new_back(struct cmd *cmd)
{
  struct cmd *c = NEW_CMD;
  c->tp = BACK;
  c->back.cmd = cmd;
  return c;
}

void free_cmd(struct cmd *c)
{
  char **s;

  if(!c)
    return;

  switch(c->tp)
  {
  default:
    fatal("free_cmd: bad cmd type %d", c->tp);
    break;

  case EXEC:
    s = c->exec.argv;
    while(*s)
      free(*s++);
    free(c->exec.argv);
    break;

  case PIPE:
    free_cmd(c->pipe.left);
    free_cmd(c->pipe.right);
    break;

  case LIST:
    free_cmd(c->list.first);
    free_cmd(c->list.next);
    break;

  case REDIR:
    free_cmd(c->redir.cmd);
    free(c->redir.path);
    break;

  case BACK:
    free_cmd(c->back.cmd);
    break;
  }

  free(c);
}

void print_cmd_inner(struct cmd *c, unsigned lev)
{
  char **s;
  unsigned i;

  for(i = 0; i < lev; i++)
    printf("  ");

  if(!c)
  {
    printf("<NULL cmd>\n");
    return;
  }

  switch(c->tp)
  {
  default:
    printf("<BAD cmd %d>\n", c->tp);
    break;

  case EXEC:
    printf("EXEC: ");
    s = c->exec.argv;
    while(*s)
      printf("\"%s\" ", *s++);
    printf("\n");
    break;

  case PIPE:
    printf("PIPE:\n");
    print_cmd_inner(c->pipe.left, lev + 1);
    print_cmd_inner(c->pipe.right, lev + 1);
    break;

  case LIST:
    printf("LIST:\n");
    print_cmd_inner(c->list.first, lev + 1);
    print_cmd_inner(c->list.next, lev + 1);
    break;

  case REDIR:
    printf("REDIR: FD %d MODE %d PATH %s\n",
              c->redir.fd,
              c->redir.mode,
              c->redir.path
             );
    print_cmd_inner(c->redir.cmd, lev + 1);
    break;

  case BACK:
    printf("BACK:\n");
    print_cmd_inner(c->back.cmd, lev + 1);
    break;
  }
}

void print_cmd(struct cmd *c)
{
  print_cmd_inner(c, 0);
}
