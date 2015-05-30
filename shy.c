#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

enum quotetypes {
  no_quote = 0, double_quote, single_quote, dollar_single, dollar_double
};

enum contexts {
  no_context = 0, com_sub, proc_sub, backticks, subshell, extglob, var_expn, math
};

struct ASTleaf {
  int nodetype;
  char *content;
  struct ASTleaf *sibling, *child, *parent;
};

enum ASTnodetypes {
  root = 0,
  pipeline, pipeline_side, pipe, pipe_ampersand,
  single_quoted, double_quoted,// dollar_single, dollar_double,
  simple_command, compound_command,
  subshell, comsub, procsub,
  redirection
};

int parser (char *line, struct ASTleaf *tree) {
  while (*line) {
    switch (*tree->parent) {
      case root:
      case subshell:
      case compound_command:

    }
  }
}

void printAST (struct ASTleaf *tree, char *prestring) {
}

/* tokenizer
 *
 * splits a string into several tokens
 * keeps com_sub and proc_sub together
 *
 * returns 0 if the command is not complete, aka the line
 * - has a trailing unescaped \
 * - ends withing a com_sub, a proc_sub, a subshell, a subshell
 *   or a math or quoting context */
int tokenizer (char *line, char **words) {
  static int quote_type = no_quote, context_type = no_context;
  char prev_char = 0;
  bool escaped = false, prev_escaped = false;
  int word = 0, character = 0;

  for (int i = 0; line[i]; i++) {
    if (escaped)
      escaped = false;
    else {
      switch (line[i]) {
        case '\\':
          if (quote_type != single_quote)
            escaped = true;
          break;

        case ' ':
        case '\t':
          if (character) {
            word++;
            continue;
          }
          break;

        case '\'':
          if (quote_type == single_quote || quote_type == dollar_single)
            quote_type = no_quote;
          else if (prev_char == '$' && !prev_escaped)
            quote_type = dollar_single;
          else
            quote_type = single_quote;
          break;

        case '"':
          if (quote_type == double_quote || quote_type == dollar_double)
            quote_type = no_quote;
          else if (prev_char == '$' && !prev_escaped)
            quote_type = dollar_double;
          else
            quote_type = double_quote;
          break;

        case '&':
          if (quote_type == no_quote && context_type == no_context) {
            /* 2>&1 */
            if ((prev_char != '>' && prev_char != '<') || prev_escaped)
              character = 0;
            word++;
          }
          break;

        case '(':
          if (context_type == no_context) {
            if (!character)
              context_type = subshell;
            else if (quote_type == no_quote && !prev_escaped)
              switch (prev_char) {
                case '<': case '>':
                  context_type = proc_sub;
                  break;
                case '$':
                  context_type = com_sub;
                  break;
                  /* extglobs */
                case '+': case '*': case '?': case '!': case '@':
                  /* TODO: ksh can use - in front of ( */
                /* case '}':  TODO */
                  context_type = extglob;
                  break;
              }
          }
          break;

        case ')':
          switch (context_type) {
            case com_sub: case proc_sub: case subshell: case extglob:
              context_type = no_context;
          }
          break;

        case '{':
          if (prev_char == '$' && !prev_escaped && !context_type)
            context_type = var_expn;
          break;

        case '}':
          if (context_type == var_expn)
            context_type = no_context;
      }

    } /* !escaped */

    /* TODO: allocate as needed */
    words[word][character++] = line[i];
  }
  return (!(quote_type || escaped || context_type));
}

int main (int argc, char *argv[]) {
  for (;;) {
  }
}
