
/*
 * Process keyboard input and dispatch commands
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <curses.h>
#include <dirent.h>
#include <string.h>
#include <strings.h>

#include "browse.h"
#include "time.h"

int sort; 
int humanReadable;
static void next_line();
static void previous_line();
static void next_screen();
static void previous_screen();
static void open_directory(NODE *dir);

/*
 * Process commands
 * vmode is 1 if in view mode, 0 if in normal mode
 */

int command(int vmode)
{
  int c;

  switch(c = getch()) {
  case 'n':
    next_line();
    break;
  case 'N':
    next_screen();
    break;
  case 'p':
    previous_line();
    break;
  case 'P':
    previous_screen();
    break;
  case 'q':
    if(!vmode) return(1);
    else feep("");
    break;
  case 'o':
    if(!vmode) open_directory(cursor_node);
    else feep("");
    break;
  case 'c':
    if(!vmode) close_directory(cursor_node);
    else feep("");
    break;
  case 'v':
    if(!vmode) view_file(cursor_node);
    else feep("");
    break;
  case 033:  /* ESC */
    if(vmode) return(1);
    else feep("");
    break;
  case '\f':
    refreshdisplay();
    break;
  case ERR:
    return(-1);
    break;
  default:
    feep("");
    break;
  }
  return(0);
}

/*
 * Move cursor to the next line of information
 */

static void next_line()
{
  if(cursor_node->next != NULL) {
    cursor_node = cursor_node->next;
    if(cursor_line < screen_height-1) cursor_line++;
  } else {
    feep("");
  }
}

/*
 * Move cursor to the previous line of information
 */

static void previous_line()
{
  if(cursor_node->prev != NULL) {
    cursor_node = cursor_node->prev;
    if(cursor_line > 0) cursor_line--;
  } else {
    feep("");
  }
}

/*
 * Move cursor to the next screen of information
 */

static void next_screen()
{
  int i;

  for(i = 0; i < screen_height-1; i++) {
    if(cursor_node->next != NULL) {
      cursor_node = cursor_node->next;
      if(cursor_line < screen_height-1) cursor_line++;
    }
  }
}

/*
 * Move cursor to the previous screen of information
 */

static void previous_screen()
{
  int i;

  for(i = 0; i < screen_height-1; i++) {
    if(cursor_node->prev != NULL) {
      cursor_node = cursor_node->prev;
      if(cursor_line > 0) cursor_line--;
    }
  }
}

void sorting(NODE *next)
{
  NODE *i;
  NODE *j;
  FILE_INFO *tempInfo;
  char * tempData;
  int cmp = 0; 
  for(i = next; i->next != NULL; i = i->next)
  {
    for(j = i->next; j != NULL; j = j->next)
    {
      // sorting on names
      if(sort == 1)
      {
        cmp = strcasecmp(i->info->path, j->info->path);
      }
      // sorting on dates
      else if (sort == 2)
      {
        cmp = i->info->stat.st_mtime > j->info->stat.st_mtime;
      }
      // sorting on file size 
      else if (sort == 3)
      {
        cmp = i->info->stat.st_size - j->info->stat.st_size;
      }
      // Swap the elements
      if (cmp > 0 && (i->info->level == j->info->level))
        {
          // Copy the data into temp
          tempInfo = i->info;
          tempData = i->data;
          // Set i's data and info to j's
          i->info = j->info;
          i->data = j->data;
          // Set j's data and info to temp's
          j->info = tempInfo;
          j->data = tempData;
        }
    }
  }
}

/*
 * Insert nodes for the contents of a directory
 */

static void open_directory(NODE *dir)
{
  NODE *node, *new;
  DIR *d;
  struct dirent *dp;
  char path[MAXPATHLEN+1];
  
  /* Make sure it's a directory */
  if(dir->info == NULL
     || (dir->info->stat.st_mode & S_IFMT) != S_IFDIR) {
    feep("Not a directory");
    return;
  }
  if((d = opendir(dir->info->path)) == NULL) {
    feep("Can't read directory");
    return;
  }
  node = dir;
  while((dp = readdir(d)) != NULL) {
    /* Don't display ".." */
    if(!strcmp(dp->d_name, "..")) continue;
    strcpy(path, dir->info->path);
    if(strcmp(path, "/")) strcat(path, "/");
    strcat(path, dp->d_name);
    if((new = get_info(path)) == NULL) continue;
    if(new->info->stat.st_dev == dir->info->stat.st_dev
       && new->info->stat.st_ino == dir->info->stat.st_ino) {
      free(new->info);
      free(new->data);
      free(new);
      continue;       /* Don't display '.' */
    }
    node = insert_node(node, new);
    node->info->parent = dir;
    node->info->level = dir->info->level+1;
  }
  closedir(d);

  // Sort dir-> next 
  if (sort > 0)
  {
    sorting(dir->next);
  }
}


/*
 * Delete nodes under a particular directory
 */

void close_directory(NODE *dir)
{
  while(dir->next != NULL
	&& dir->next->info != NULL
	&& dir->next->info->parent == dir) {
    if((dir->next->info->stat.st_mode & S_IFMT) == S_IFDIR) {
      close_directory(dir->next);
      delete_node(dir);
    } else {
      delete_node(dir);
    }
  }
}
