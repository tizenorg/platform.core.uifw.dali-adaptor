#ifndef WORD_EXP_STUB
#define WORD_EXP_STUB

/* Structure describing a word-expansion run.  */
typedef struct
  {
    size_t we_wordc;		/* Count of words matched.  */
    char **we_wordv;		/* List of expanded words.  */
    size_t we_offs;		/* Slots to reserve in `we_wordv'.  */
  } wordexp_t;

int wordexp(const char *s, wordexp_t *p, int flags);
void wordfree(wordexp_t*);

#endif

