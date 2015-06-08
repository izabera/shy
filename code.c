#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct mynode {
  short type;
  char *value;
  struct mynode *up, *down, *left, *right;
} node;

enum directions { up = 0, down, left, right };
enum types { root = 0,
  pipeline , pipeline_side, pipe, pipe_ampersand,
  ampersand, semicolon,
  simple_command, compound_command, subshell,
  redirection, fd,
  variable,
  string, unquoted, single_quoted, double_quoted, actual_value
};
char *stringtypes[] = { "root",
  "pipeline", "pipeline_side", "pipe", "pipe_ampersand",
  "ampersand", "semicolon",
  "simple_command", "compound_command", "subshell",
  "redirection", "fd",
  "variable",
  "string", "unquoted", "single_quoted", "double_quoted", "actual_value"
};

node *createnode(int direction, node *orig, int type, char *value) {
  if (orig == NULL) return NULL;
  node *newnode = malloc(sizeof(node));
  if (newnode == NULL) return NULL;
  newnode->type = type;
  newnode->value = value;
  switch(direction) {
#if 0
    case up:
      newnode->up   = NULL;     newnode->down  = orig;
      newnode->left = NULL;     newnode->right = NULL;
      orig->up      = newnode;
      break;
#endif
    case down:
      newnode->up   = orig;     newnode->down  = NULL;
      newnode->left = NULL;     newnode->right = NULL;
      orig->down    = newnode;
      break;
#if 0
    case left:
      newnode->up   = orig->up; newnode->down  = NULL;
      newnode->left = NULL;     newnode->right = orig;
      orig->left    = newnode;
      break;
#endif
    case right:
      newnode->up   = orig->up; newnode->down  = NULL;
      newnode->left = orig;     newnode->right = NULL;
      orig->right   = newnode;
      break;
  }
  return newnode;
}

node *createroot() {
  node *root = malloc(sizeof(node));
  if (root == NULL) return NULL;
  root->type = 0;
  root->value = NULL;
  root->left = root->right = root->up = root->down = NULL;
  return root;
}

void freechildren(node *node) {
  if (node->down) freechildren(node->down);
  free(node);
}

void freesiblings(node *node) {
  if (node->down) freechildren(node->down);
  if (node->right) freesiblings(node->right);
  free(node);
}


void printtree(node *node, char *sofar, int lvl) {
  printf("%s", sofar ? sofar : "");
  switch (node->type) {
    case root:             case pipeline:
    case pipeline_side:    case pipe:
    case pipe_ampersand:   case simple_command:
    case compound_command: case subshell:
    case fd:               case variable:
    case string:           case unquoted:
    case single_quoted:    case double_quoted:
    case ampersand:        case semicolon:
      printf("(%s)\n", stringtypes[node->type]);
      break;
    case redirection:      case actual_value:
      puts(node->value);
      break;
  }
  char *tmp;
  if (node->down) {
    tmp = malloc(1 + 4 * (lvl+1));
    if (sofar) {
      if (sofar[strlen(sofar)-4] == '|')
        sprintf(tmp, "%.*s   %c-- ", strlen(sofar) - 3, sofar, (node->down->right) ? '|' : '`');
      else
        sprintf(tmp, "%.*s    %c-- ", strlen(sofar) - 4, sofar, (node->down->right) ? '|' : '`');
    }
    else
      sprintf(tmp, "%c-- ", (node->down->right) ? '|' : '`');
    printtree(node->down, tmp, lvl+1);
  }
  if (node->right) {
    tmp = malloc(1 + 4 * lvl);
    if (sofar) {
      if (sofar[strlen(sofar)-4] == '|')
        sprintf(tmp, "%.*s%c-- ", strlen(sofar) - 4, sofar, (node->right->right) ? '|' : '`');
      else
        sprintf(tmp, "%.*s    %c-- ", strlen(sofar) - 4, sofar, (node->right->right) ? '|' : '`');
    }
    else
      sprintf(tmp, "%c-- ", (node->right->right) ? '|' : '`');
    printtree(node->right, tmp, lvl);
  }
  free(sofar);
}

int main() {
  node *root = createroot(), *last = root;

  /* sample */
  last = createnode(down, last, pipeline, NULL);
  last = createnode(down, last, pipeline_side, NULL);
  last = createnode(down, last, simple_command, NULL);
  last = createnode(down, last, string, NULL);
  last = createnode(down, last, unquoted, NULL);
  last = createnode(down, last, actual_value, "a");
  last = createnode(right, root->down, ampersand, NULL);
  last = createnode(right, last, pipeline, NULL);
  last = createnode(down, last, pipeline_side, NULL);
  last = createnode(down, last, simple_command, NULL);
  last = createnode(down, last, string, NULL);
  last = createnode(down, last, unquoted, NULL);
  last = createnode(down, last, actual_value, "b");

  printtree(root, NULL, 0);
  freechildren(root);
  return 0;
}
