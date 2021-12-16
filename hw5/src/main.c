#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include "csapp.h"

#include "client_registry.h"
#include "maze.h"
#include "player.h"
#include "debug.h"
#include "server.h"

static void terminate(int status);

static char *default_maze[] = {
  "******************************",
  "***** %%%%%%%%% &&&&&&&&&&& **",
  "***** %%%%%%%%%        $$$$  *",
  "*           $$$$$$ $$$$$$$$$ *",
  "*##########                  *",
  "*########## @@@@@@@@@@@@@@@@@*",
  "*           @@@@@@@@@@@@@@@@@*",
  "******************************",
  NULL
};
static char **temp_maze;
CLIENT_REGISTRY *client_registry;
static int terminateFlag = 0;
static int listenfd;

void freeMaze() {
  for(int i = 0; temp_maze[i] != NULL; i++)
  {
    // printf("%s\n", temp_maze[i]);
    free(temp_maze[i]);
  }
  free(temp_maze);
}
static void cleanTerm(int s) {
  close(listenfd);
  terminateFlag = 1;
}


int main(int argc, char* argv[]){
    // Option processing should be performed here.
    // Option '-p <port>' is required in order to specify the port number
    // on which the server should listen.
    // default values
    // option parsing for server
    char port[20]; // port numbers are limited to 16 bits 
    int opt;
    FILE *fp;
    int idx = 0;
    while((opt = getopt(argc, argv, "-:p:t:")) != -1)
    {
      switch(opt)
      {
        case 'p':
          if (atoi(optarg) == 0) 
          {
            exit(EXIT_FAILURE);
          }
          sprintf(port, "%d", atoi(optarg));
          break;
        
        // new template file
        case 't':
          fp = fopen(optarg, "r");  
          if (!fp)
          {
            fprintf(stderr, "Could not open textfile %s\n", optarg);
            exit(EXIT_FAILURE);
          }
          int row = 1;
          char line[MAXLINE];

          // get the number of rows
          while (fgets(line, MAXLINE, fp) != NULL)
          {
            row++;
          }
          fclose(fp);
          fp = fopen(optarg, "r");  

          // allocate space for rows 
          temp_maze = (char**) malloc(row * sizeof(char*));
          
          while (fgets(line, MAXLINE, fp) != NULL)
          {
            int numCharRead = strlen(line);
            
            // set the new lines to null terminate
            if (line[numCharRead -1] == '\n')
            {
              line[numCharRead -1] = 0;
            }
            // allocate space for a row of chars
            temp_maze[idx] = (char*) malloc(numCharRead * sizeof(char));
            
            // copy string read to the temp_maze
            strcpy(temp_maze[idx], line);

            // increment to the next row
            idx++;
          }
          // set the last line to NULL
          // temp_maze[idx] = (char*) malloc(sizeof(char));
          temp_maze[row-1] = NULL;

          // close file pointer
          fclose(fp);
          break;

        // unknown options
        case '?':
          exit(EXIT_FAILURE);
          break;

        // missing options
        case ':':
          fprintf(stderr, "Usage bin/mazewar -p <port> [-t <template_file>]\n");
          exit(EXIT_FAILURE);
          break;
        
        case 1:
          exit(EXIT_FAILURE);
          break;
      }
    }
    if (strcmp(port, "") == 0)
    {
      fprintf(stderr, "Usage bin/mazewar -p <port> [-t <template_file>]\n");
      exit(EXIT_FAILURE);
    }
    // Perform required initializations of the client_registry,
    // maze, and player modules.
    client_registry = creg_init();
    if (idx == 0) 
    {
      maze_init(default_maze);
    }
    else
    {
      maze_init(temp_maze);
    }
    // freeMaze();
    player_init();
    debug_show_maze = 1;  // Show the maze after each packet.

    // TODO: Set up the server socket and enter a loop to accept connections
    // on this socket.  For each connection, a thread should be started to
    // run function mzw_client_service().  In addition, you should install
    // a SIGHUP handler, so that receipt of SIGHUP will perform a clean
    // shutdown of the server.
    
    socklen_t clientlen;
    struct sockaddr_storage clientAddr; // Passed into accept. Filled with socket addr of client 
    pthread_t tid;
    // Signal handling
    struct sigaction sigup;
    sigemptyset(&sigup.sa_mask);
    sigup.sa_flags = 0;
    sigup.sa_handler = &cleanTerm;
    sigaction(SIGHUP, &sigup, NULL);
    // Signal(SIGHUP, cleanTerm);
    signal(SIGPIPE, SIG_IGN);
    // Signal(SIGPIPE, SIG_IGN);
    listenfd = Open_listenfd(port);
    int *connfd;

    while (1) {
        clientlen = sizeof(struct sockaddr_storage);
        connfd = malloc(sizeof(int));
        *connfd = accept(listenfd, (SA *)&clientAddr, &clientlen);
        // if (*connfd < 0) 
        // {
          
        //   free(connfd);
        //   terminateFlag = 1;
        // }
        if (terminateFlag)
        {
          close(*connfd);
          free(connfd);
          terminate(EXIT_SUCCESS);
        }
        if (pthread_create(&tid, NULL, mzw_client_service, (void*)connfd) != 0 )
        {
          close(*connfd);
          free(connfd);
        }
    }
    
    // fprintf(stderr, "You have to finish implementing main() "
	  //   "before the MazeWar server will function.\n");
    exit(EXIT_SUCCESS);
}

/*
 * Function called to cleanly shut down the server.
 */
void terminate(int status) {
    freeMaze();
    close(listenfd);
    // Shutdown all client connections.
    // This will trigger the eventual termination of service threads.
    creg_shutdown_all(client_registry);
    
    debug("Waiting for service threads to terminate...");
    creg_wait_for_empty(client_registry);
    debug("All service threads terminated.");

    // Finalize modules.
    creg_fini(client_registry);
    player_fini();
    maze_fini();

    debug("MazeWar server terminating");
    exit(status);
}
