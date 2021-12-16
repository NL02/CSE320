
/*
 * File system browser.
 * E. Stark
 * 11/3/93
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <curses.h>
#include <unistd.h>
#include <stdlib.h>

#include <getopt.h>
#include <string.h>
#include "browse.h"

extern int sort; 
extern int humanReadable;

int main(int argc, char *argv[])
{
  int err;
  char *getcwd(), *base;
  int opt;
  int option_index;
  int sortFlag = 0;
  while(1)
  {
    static struct option long_options[] =
    {
      {"sort-key",        required_argument, NULL, 0},
      {"human-readable",  no_argument,       NULL, 0},
      {NULL, 0, NULL, 0}
    };
    opt = getopt_long(argc, argv, "-:s:", long_options, &option_index);
    if(opt == -1)
    {
      break;
    }
    switch(opt)
    {
      case 0:
      if (strcmp(long_options[option_index].name, "human-readable") == 0)
      {
        if (humanReadable == 1)
          {
            fprintf(stderr, "Usage: %s [dir]\n", argv[0]);
            exit(1);
            break;
          }
          humanReadable = 1;
      }
      else if (strcmp(long_options[option_index].name, "sort-key") == 0)
      {
        if (sortFlag == 0)
        {
          if (strcmp(optarg, "none") == 0)
          {
            sort = 0;
          }
          else if (strcmp(optarg, "name") == 0)
          {
            sort = 1;
          }
          else if (strcmp(optarg, "date") == 0)
          {
            sort = 2;
          }
          else if (strcmp(optarg, "size") == 0)
          {
            sort = 3;
          }
          else 
          {
            fprintf(stderr, "Usage: %s [dir]\n", argv[0]);
            exit(1);
            break;
          }
          sortFlag = 1;
        }
        else
        {
          fprintf(stderr, "Usage: %s [dir]\n", argv[0]);
          exit(1);
          break;
        }
      }
      break;

      case 1:
      // Case for directory param
      if(chdir(optarg) < 0)
      {
        fprintf(stderr, "Can't change directory to %s\n", argv[1]);
        exit(1);
      }
      break;

      case 's':
      // We know that it hasn't taken a sort flag yet
      if (sortFlag == 0)
      {
        if (strcmp(optarg, "none") == 0)
        {
          sort = 0;
        }
        else if (strcmp(optarg, "name") == 0)
        {
          sort = 1;
        }
        else if (strcmp(optarg, "date") == 0)
        {
          printf("date is marked\n");
          sort = 2;
        }
        else if (strcmp(optarg, "size") == 0)
        {
          sort = 3;
        }
        else 
        {
          fprintf(stderr, "Usage: %s [dir]\n", argv[0]);
          exit(1);
          break;
        }
        sortFlag = 1;
      }
      else
      {
        fprintf(stderr, "Usage: %s [dir]\n", argv[0]);
        exit(1);
        break;
      }
      break;

      case '?':
      fprintf(stderr, "Usage: %s [dir]\n", argv[0]);
      exit(1);
      break;

      case ':':
      fprintf(stderr, "Usage: %s [dir]\n", argv[0]);
      exit(1);
      break;

      default:
      fprintf(stderr, "Usage: %s [dir]\n", argv[0]);
      exit(1);
      break;
    }
  }
  base = getcwd(NULL, MAXPATHLEN+1);
  if((cursor_node = get_info(base)) == NULL) {
    fprintf(stderr, "Can't stat %s\n", base);
    exit(1);
  }
  initdisplay();
  cursor_line = 0;
  do redisplay(); while(!(err = command(0)));
  enddisplay();
  exit(err < 0 ? EXIT_FAILURE : EXIT_SUCCESS);
}
