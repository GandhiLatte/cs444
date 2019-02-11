#include "parser.h"
#include "tok.h"
#include "io.h"
#include "mem.h"

struct cmd *parse_list(void);
struct cmd *parse_pipeline(void);
struct cmd *parse_block(void);
struct cmd *parse_redir(struct cmd *);
struct cmd *parse_exec(void);
void parser_recover(void);

#define is_sync(t) ((t) == TOK_EOI || (t) == TOK_EOF)
#define is_eof(t) ((t) == TOK_EOF)

struct cmd *parse(void)
{
  while(is_sync(get_cur()) && !is_eof(get_cur()))
    advance();

  struct cmd *c = parse_list();

  /* Resynchronize the parser */
  while(!is_sync(get_cur()))
  {
    printf("parse_list: warning: trailing token %d:%s\n",
              get_cur(), safe_tok_name(get_cur())
             );
    advance();
  }

  return c;
}

struct cmd *parse_list(void)
{
  struct cmd *c = parse_pipeline(), *sub;

  /* If sublevel parsing failed, return error. */
  if(!c) return NULL;

  /* If we find a separator... */
  if(get_cur() == TOK_SEP || get_cur() == TOK_BACK)
  {
    /* ...handle BACK speciallly... */
    if(get_cur() == TOK_BACK)
      c = new_back(c);
    /* ...consume it... */
    advance();
    /* ...and make a list (eventually terminated by an error return). */
    sub = parse_list();
    if(sub)
      return new_list(c, sub);
  }

  return c;
}

struct cmd *parse_pipeline(void)
{
  struct cmd *c = parse_block(), *sub;

  if(!c) return NULL;

  if(get_cur() == TOK_PIPE)
  {
    advance();
    sub = parse_pipeline();
    if(sub)
      return new_pipe(c, sub);
    else
      printf("parse_pipeline: warning: trailing pipe\n");
  }

  return c;
}

struct cmd *parse_block(void)
{
  struct cmd *c, *nc;

  if(get_cur() == TOK_BLOCK_START)
  {
    advance();
    c = parse_list();
    if(get_cur() != TOK_BLOCK_END)
    {
      printf("parse_block: synerr: unbalanced block\n");
      parser_recover();
      return NULL;
    }
    advance();
  }
  else
    c = parse_exec();

  nc = parse_redir(c);
  while(c != nc)
  {
    c = nc;
    nc = parse_redir(c);
  }

  return c;
}

struct cmd *parse_redir(struct cmd *c)
{
  int fd, mode;

  if(!c) return NULL;

  switch(get_cur())
  {
  default:
    return c;

  case TOK_REDIR_IN:
    fd = 0;
    mode = O_RDONLY;
    break;

  case TOK_REDIR_OUT:
    fd = 1;
    mode = O_RDWR | O_CREAT | O_TRUNC;
    break;

  case TOK_REDIR_APPEND:
    fd = 1;
    mode = O_WRONLY | O_CREAT | O_APPEND;
    break;
  }

  advance();
  if(get_cur() != TOK_WORD)
  {
    printf("parse_redir: synerr: expected a filename after redirect");
    parser_recover();
    return NULL;
  }
  /* Because of ownership, this word has to be copied--the tokenizer
   * will be freeing its copy on the advance call later. */
  c = new_redir(c, strdup(get_cur_data()), mode, fd);
  advance(); /* cur_data is freed here */

  return c;
}

struct cmd *parse_exec(void)
{
  char **argv, **new_argv;
  unsigned sz = 8, pos = 0;

  /* Not an error: technically, this is just an empty command. */
  if(is_sync(get_cur()))
  {
    //advance();
    return NULL;
  }

  if(get_cur() != TOK_WORD)
  {
    printf("parse_exec: synerr: expected a word, found instead %d:%s",
              get_cur(), safe_tok_name(get_cur())
             );
    parser_recover();
    return NULL;
  }

  argv = calloc(sz, sizeof(char *));
  if(!argv)
    fatal("parse_exec: out of memory (allocation size: %lu)", sz * sizeof(char *));
  argv[pos++] = strdup(get_cur_data());
  advance();
  while(get_cur() == TOK_WORD)
  {
    if(pos + 1 >= sz)
    {
      new_argv = calloc(sz * 2, sizeof(char *));
      if(!new_argv)
        fatal("parse_exec: out of memory (allocation size: %lu)", sz * 2 * sizeof(char *));
      memmove(new_argv, argv, sz);
      free(argv);
      argv = new_argv;
      sz *= 2;
    }
    argv[pos++] = strdup(get_cur_data());
    advance();
  }

  return new_exec(argv);
}

void parser_recover(void)
{
  while(!is_sync(get_cur()))
    advance();
  while(is_sync(get_cur()) && !is_eof(get_cur()))
    advance();
}

