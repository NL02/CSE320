/*
 * "View mode", in which we view the contents of a file.
 * To keep things simple, we read in the whole file at once.
 */

#include <sys/param.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "browse.h"

void view_file(NODE *node)
{
  FILE *f;
  char *buf;
  size_t bufLen;

  int save_cursor_line = cursor_line;
  NODE *save_cursor_node = cursor_node;
  NODE *first, *last, *new;

  if(node->info == NULL
     || (((node->info->stat.st_mode & S_IFMT) != S_IFREG)
	 && ((node->info->stat.st_mode & S_IFMT) != S_IFLNK))) {
    feep("Not a regular file or link");
    return;
  }
  if ((f = fopen(node->info->path, "r")) == NULL) {
    feep("Can't open file");
    return;
  }
  first = last = NULL;
  while((new = malloc(sizeof(NODE))) != NULL) {
    new->info = NULL;
    new->prev = NULL;
    new->next = NULL;
    buf = NULL;
    bufLen = 0;
    if(getline(&buf, &bufLen, f) == -1 ) {
      free(buf);
      break;
    }
    new->data = strdup(buf);
    free(buf);
    if(first == NULL) first = last = new;
    else last = insert_node(last, new);
  }
  free(new);
  fclose(f);
  if(first != NULL) {
    cursor_node = first;
    cursor_line = 0;
    do { redisplay(); } while(!command(1));
  } else {
    feep("Empty file");
    return;
  }
  int flag = 0;
  while(first->next != NULL) {
    /* actually frees the next */
    delete_node(first);
  }
  if (first->info != NULL)
  {
    free(first->info);
  }
  if (first->data != NULL)
  {
    free(first->data);
  }
  free(first);
  cursor_line = save_cursor_line;
  cursor_node = save_cursor_node;
}
