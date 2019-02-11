#ifndef TOK_H
#define TOK_H

/* For the actual lexemes (characters to type), see the switch/case in the
 * advance function in tok.c. */
enum tok_t
{
  TOK_WORD,
  TOK_REDIR_IN,
  TOK_REDIR_OUT,
  TOK_REDIR_APPEND,
  TOK_BACK,
  TOK_PIPE,
  TOK_SEP,
  TOK_BLOCK_START,
  TOK_BLOCK_END,
  TOK_EOI,
  TOK_EOF,
  NUM_TOKENS,
};

extern char *tok_names[];

enum tok_t get_cur(void);
char *get_cur_data(void);
void advance(void);

#define safe_tok_name(t) ((t) >= 0 && (t) < NUM_TOKENS ? tok_names[t] : "(bad tok)")

#endif
