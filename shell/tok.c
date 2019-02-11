#include "tok.h"
#include "mem.h"
#include "io.h"

char *tok_names[] =
{
  "TOK_WORD",
  "TOK_REDIR_IN",
  "TOK_REDIR_OUT",
  "TOK_REDIR_APPEND",
  "TOK_BACK",
  "TOK_PIPE",
  "TOK_SEP",
  "TOK_BLOCK_START",
  "TOK_BLOCK_END",
  "TOK_EOI",
  "TOK_EOF",
};

enum tok_t cur;
char *cur_data;

enum tok_t get_cur(void)
{
  return cur;
}
char *get_cur_data(void)
{
  return cur_data;
}

#define NOT_PUSHED (EOF - 1)

static int pushback = NOT_PUSHED;

void push_back(char c)
{
  pushback = c;
}

int next_char(void)
{
  char c;

  if(pushback != NOT_PUSHED)
  {
    c = pushback;
    pushback = NOT_PUSHED;
  }
  else
    c = getchar();

  return c;
}

static unsigned data_sz, data_pos;

void prepare_data()
{
  data_sz = 32;
  data_pos = 0;
  cur_data = calloc(data_sz, sizeof(char));
  if(!cur_data)
    fatal("advance: out of memory (allocation size: %u)", data_sz);
}

void push_data(char c)
{
  char *new_data;
  /* Reallocate if necessary--note the space for the
   * terminator */
  if(data_pos + 1 >= data_sz)
  {
    /* Carefully copy the old block to a new allocation
     * without leaking memory or double-freeing; note
     * that the region is grown exponentially, mostly to
     * amortize this expensive copy, but this may be
     * problematic with low memory. */
    new_data = calloc(data_sz * 2, sizeof(char));
    if(!new_data)
      fatal("advance: out of memory (allocation size: %u)", data_sz * 2);
    memmove(new_data, cur_data, data_sz);
    free(cur_data);
    cur_data = new_data;
    data_sz *= 2;
  }
  cur_data[data_pos++] = c;
}

void terminate_data()
{
  cur_data[data_pos] = '\0';
}

void advance(void)
{
  int c, nc;

  if(cur_data)
    free(cur_data);
  cur_data = NULL;

  while(1)
  {
    c = next_char();

    switch(c)
    {
    case EOF:
      cur = TOK_EOF;
      return;

    /* Ignore whitespace (except newlines, see below) */
    case ' ':
    case '\t':
    case '\b':
    case '\v':
    case '\r':
      continue;

    /* Newline ends a user input */
    case '\n':
      cur = TOK_EOI;
      return;

    case ';':
      cur = TOK_SEP;
      return;

    case '&':
      cur = TOK_BACK;
      return;

    case '|':
      cur = TOK_PIPE;
      return;

    case '<':
      cur = TOK_REDIR_IN;
      return;

    case '>':
      /* Handle >> (append) */
      nc = next_char();
      if(nc == '>')
      {
        cur = TOK_REDIR_APPEND;
        return;
      }
      push_back(nc);
      cur = TOK_REDIR_OUT;
      return;

    case '(':
      cur = TOK_BLOCK_START;
      return;

    case ')':
      cur = TOK_BLOCK_END;
      return;

    /* Quoted words */
    case '"':
    case '\'':
      cur = TOK_WORD;
      prepare_data();

      nc = next_char();
      while(nc != c)
      {
        push_data(nc);
        nc = next_char();
      }
      /* XXX: Don't push back the terminating quote */
      terminate_data();
      return;

    default:
      /* XXX Assuming everything else is a word--this is not
       * necessarily safe... */
      cur = TOK_WORD;
      prepare_data();
      push_data(c);

      nc = next_char();
	  /* Separate words on whitespace, end a word at EOF or any special character */
      while(nc != ' ' && nc != '\t' && nc != '\b' && nc != '\v' && nc != '\r' && nc != '\n' &&
			  nc != ';' && nc != '&' && nc != '(' && nc != ')' && nc != '<' && nc != '>' &&
			  nc != '|' && nc != EOF)
      {
        push_data(nc);
        nc = next_char();
      }
      push_back(nc);
      terminate_data();
      return;
    }
  }
}
