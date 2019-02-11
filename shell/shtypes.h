#ifndef SHTYPES_H
#define SHTYPES_H

struct cmd
{
  enum {EXEC, PIPE, LIST, REDIR, BACK} tp;
  union
  {
    struct
    {
      char **argv;  /* An array of strings */
    } exec;
    struct
    {
      struct cmd *left;
      struct cmd *right;
    } pipe;
    struct
    {
      struct cmd *first;
      struct cmd *next;
    } list;
    struct
    {
      struct cmd *cmd;
      char *path;
      int mode;
      int fd;
    } redir;
    struct
    {
      struct cmd *cmd;
    } back;
  };
};

/* NB: All of these will *own* the dynamic data given to them--you don't have
 * to worry about freeing them after using it as an argument to these
 * functions. However, you *do* have to worry about copying it if you need to,
 * because the data won't survive the lifetime of freeing a command. See
 * parse.c for an example usage. */

struct cmd *new_exec(char **argv);
struct cmd *new_pipe(struct cmd *left, struct cmd *right);
struct cmd *new_list(struct cmd *first, struct cmd *next);
struct cmd *new_redir(struct cmd *cmd, char *path, int mode, int fd);
struct cmd *new_back(struct cmd *cmd);

void free_cmd(struct cmd *cmd);

/* For debugging */

void print_cmd(struct cmd *cmd);

#endif
