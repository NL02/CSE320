#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>


#include "cookbook.h"
#include "csapp.h"
#include "chefs.h"

volatile sig_atomic_t pid;
int activeCooks = 0;
int exitFlag = 0;

typedef struct {
    int pid;
    RECIPE *recipe;
} PIDRECIPE;

typedef struct {
    PIDRECIPE **pid_recipe_ptr_arr;
    size_t currNumElem;
    size_t size;
} PID_RECIPE_ARRAY;

PID_RECIPE_ARRAY pidRecipeArr; // array for PID recipe pairs
void initPIDArray(PID_RECIPE_ARRAY *a, size_t initialSize) {
    a->pid_recipe_ptr_arr = malloc(initialSize *sizeof(PIDRECIPE *));
    a->currNumElem = 0;
    a->size = initialSize;
}

void insertPIDArray(PID_RECIPE_ARRAY *a, PIDRECIPE *element) {
    if (a->currNumElem == a->size)
    {
        a->size *= 2;
        a->pid_recipe_ptr_arr = realloc(a->pid_recipe_ptr_arr, a->size * sizeof(PIDRECIPE *));
    }
    a->pid_recipe_ptr_arr[a->currNumElem++] = element;
}
void freePIDArray(PID_RECIPE_ARRAY *a)
{
    for (int i = 0; i < a->currNumElem; i++)
    {
        free(a->pid_recipe_ptr_arr[i]->recipe->state);
        free(a->pid_recipe_ptr_arr[i]);
    }
    free(a->pid_recipe_ptr_arr);
    a->pid_recipe_ptr_arr = NULL;
    a->currNumElem = a->size = 0;
}
void removePIDRecipe(PID_RECIPE_ARRAY *a, int pid) {
    for (int i = 0; i < a->currNumElem - 1; i++ )
    {
        if (a->pid_recipe_ptr_arr[i]->pid == pid)
        {
            for(int j = i; j < a->currNumElem -1; j++)
            {
                a->pid_recipe_ptr_arr[j] = a->pid_recipe_ptr_arr[j+1];
            }
        }
    }
    a->currNumElem -= 1;
}

typedef struct {
    RECIPE** recipe_ptr_array;
    size_t currNumElem; //current number of elements
    size_t size; // total size
} RECIPE_PTR_ARRAY; 

RECIPE_PTR_ARRAY nonLeaves; // array for non leaf recipes
RECIPE_PTR_ARRAY workQueue; // array for leaf recipes
RECIPE_PTR_ARRAY completedRecipes; // array for completed recipes

void initArray(RECIPE_PTR_ARRAY *a, size_t initialSize) {
    a->recipe_ptr_array = malloc(initialSize * sizeof(RECIPE *));
    a->currNumElem = 0;
    a->size = initialSize;
}

void insertArray(RECIPE_PTR_ARRAY *a, RECIPE* element) {
    if (a->currNumElem == a->size) 
    {
        a->size *= 2;
        a->recipe_ptr_array = realloc(a->recipe_ptr_array, a->size * sizeof(RECIPE *));
    }
    a->recipe_ptr_array[a->currNumElem++] = element;
}

void freeArray(RECIPE_PTR_ARRAY *a) {
  free(a->recipe_ptr_array);
  a->recipe_ptr_array = NULL;
  a->currNumElem = a->size = 0;
}

void removeRecipe(RECIPE_PTR_ARRAY *a, RECIPE *recipe) {
    for (int i = 0; i < a->currNumElem - 1; i++ )
    {
        if (a->recipe_ptr_array[i]->name == recipe->name)
        {
            for(int j = i; j < a->currNumElem -1; j++)
            {
                a->recipe_ptr_array[j] = a->recipe_ptr_array[j+1];
            }
        }
    }
    a->currNumElem -= 1;
}

RECIPE *pop(RECIPE_PTR_ARRAY *a) {
    RECIPE *top = a->recipe_ptr_array[0];
    for(int i = 0; i < a->currNumElem - 1; i++)
    {
        a->recipe_ptr_array[i] = a->recipe_ptr_array[i+1];
    }
    a->currNumElem -= 1;
    return top;
}

// Function used to parse the arguments for cookbook, mainRecipe and maxCooks
int argumentParse(int argc, char *argv[], int *err, char **cookbook, char **mainRecipe, int *maxCooks) {
    int opt;
    while ((opt = getopt(argc, argv, "-:c:f:")) != -1)
    {
        // format cook [-f cookbook filepath] [-c max cooks] [main_recipe_name]
        switch(opt)
        {
            // max cooks
            case 'c':
                *(maxCooks) = atoi(optarg);
                break;

            // filepath
            case 'f':
                *(cookbook) = optarg;
                break;

            // main recipe name
            case 1:
                // if we're assigning it the first time
                if (*(err) == 0)
                {
                    *(mainRecipe) = optarg;
                    *(err) = 1;
                }
                else 
                {
                    return 1;
                }
                break;

            // unknown options
            case '?':
                // our program isn't supposed to output so we can err I think
                return 1;

            // missing options
            case ':':
                return 1;
        }
    }
    // successfully parsed arugments
    // reset err to 0
    *(err) = 0;
    return 0;
}

void postorder(RECIPE_PTR_ARRAY *rPtrArray, RECIPE_PTR_ARRAY *leafRecipes, RECIPE *recipe) {
    if (recipe == NULL)
        return;
    RECIPE_LINK *dependsOn = recipe->this_depends_on;
    int flag = 0;
    // case for leaves
    if(recipe->this_depends_on == NULL)
    {
        for (int i = 0; i < leafRecipes->currNumElem ; i++)
        {
            if(leafRecipes->recipe_ptr_array[i] == recipe)
            {
                flag = 1;
            }
        }
        if( flag != 1)
        {
            insertArray(leafRecipes, recipe);
        }
    }
    // case for non leaves
    else 
    {
        while (dependsOn)
        {
        postorder(rPtrArray, leafRecipes, dependsOn->recipe);
        dependsOn = dependsOn->next;
        }

        for (int i = 0; i < rPtrArray->currNumElem ; i++)
        {
            if(rPtrArray->recipe_ptr_array[i] == recipe)
            {
                flag = 1;
            }
        }
        if( flag != 1)
        {
            insertArray(rPtrArray, recipe);
        }
    }
}

 void buildWorkQueue(RECIPE_PTR_ARRAY *nonLeaves, RECIPE_PTR_ARRAY *leafRecipes, COOKBOOK *cbp, char *mainRecipe) {
    RECIPE *recipes = cbp->recipes;
    // Get the receipe that we're trying to make from the cookbook
    if( strcmp(mainRecipe, "first") != 0)
    {
        while (recipes) 
        {
            if(strcmp(recipes->name, mainRecipe) == 0)
            {
                break;
            }
            recipes = recipes->next;
        }
    }
    if (recipes == NULL)
    {
        fprintf(stderr, "Main Recipe %s not found\n", mainRecipe);
        exit(EXIT_FAILURE);
    }
    postorder(nonLeaves, leafRecipes, recipes);
}

void execTask(TASK *task) {
    STEP * step = task->steps;
    int childStatus;
    int fd[2]; // fd[0] is stdin, fd[1] is stdout
    int original = dup(STDOUT_FILENO); // original file descriptor
    int temp;
    int firstFlag = 0; // flag to determine whether we're the first element
    // int test = 0;
    while(step)
    {
        if (pipe(fd) < 0)
        {
            fprintf(stderr, "Piping failed for task");
            exit(EXIT_FAILURE);;
        }
        // executing util
        if((pid = Fork() == 0))
        {
            // if input
            if (firstFlag == 0)
            {
                if (task->input_file)
                {
                    int inputFile;
                    if ((inputFile = open(task->input_file, O_RDONLY)) < 0)
                    {
                        fprintf(stderr, "failure occured when opening a file to read");
                        exit(EXIT_FAILURE);;
                    }
                    dup2(inputFile, STDIN_FILENO);
                    close(inputFile);
                    firstFlag = 1;
                }
            }
            // normal sequential step
            else
            {
                dup2(temp, STDIN_FILENO);
                dup2(fd[1], STDOUT_FILENO);
            }
            // if output 
            int outputFile;
            if (task->next == NULL)
            {
                if (task->output_file)
                {
                    
                    if ((outputFile = open(task->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0777)) < 0)
                    {
                        fprintf(stderr, "failure occured when opening a file to output");
                        exit(EXIT_FAILURE);
                    }
                    dup2(outputFile, STDOUT_FILENO);
                    close(outputFile);
                }
            }
            close(fd[0]);
            char **word = step->words;
            // concat together
            char util[strlen(*(word)) + 6];
            strcpy(util, "util/");
            strncat(util, *(word), strlen(*(word)));   
            execvp(util,word);
            execvp(*(word), word);
            exit(EXIT_FAILURE);
        }
        temp = fd[0];
        close(fd[1]);
        step = step->next;
    }
    dup2(original, STDOUT_FILENO);
    while (waitpid(pid, &childStatus, 0) > 0)
    {
        if (WIFEXITED(childStatus))
        {
            if(WEXITSTATUS(childStatus) == 1)
            {
                exit(EXIT_FAILURE);
            }
        }
    }
}

void execRecipe(RECIPE *recipe) {
    TASK *task = recipe->tasks;

    while(task)
    {
        execTask(task);
        task = task->next;
    }
}

int arrayContains(RECIPE_PTR_ARRAY *a, RECIPE *recipe) {
    for(int i = 0; i < a->currNumElem; i++)
    {
        if (a->recipe_ptr_array[i] == recipe)
        {
            return 1;
        }
    }
    return 0;
}

void transferRecipes(RECIPE_PTR_ARRAY *nonLeaves, RECIPE_PTR_ARRAY *workQueue, RECIPE_PTR_ARRAY *completedRecipes)
{
    for(int i = 0; i < nonLeaves->currNumElem; i++)
    {
        RECIPE_LINK *dependsOn = nonLeaves->recipe_ptr_array[i]->this_depends_on;
        int totalDepend = 0;
        int counter = 0;
        // loop through non leaves and check if their depends on are completed in the completed array
        //  if all their dependson are inside of the completed array move it to workqueue and remove it from nonleaves
        while(dependsOn)
        {
            if (arrayContains(completedRecipes, dependsOn->recipe))
            {
                counter += 1;
            }
            totalDepend += 1;
            dependsOn = dependsOn->next;
        }
        if(totalDepend == counter)
        {
            insertArray(workQueue, nonLeaves->recipe_ptr_array[i]);
            removeRecipe(nonLeaves, nonLeaves->recipe_ptr_array[i]);
        }
    }
}

void sigchld_handler(int s) {
    int childStatus; // exitstatus of child
    int olderno = errno;
    while ((pid = waitpid(-1, &childStatus, WNOHANG )) > 0)
    {
        RECIPE *completed = NULL;
        
        if (WIFEXITED(childStatus))
        {
            if (WEXITSTATUS(childStatus) == 1)
            {
                exitFlag = 1;
            }
        }
        
        // check the pid mapping to see if the pid matches with a recipe, it should
        for (int i = 0; i < pidRecipeArr.currNumElem; i++)
        {
            if(pid == pidRecipeArr.pid_recipe_ptr_arr[i]->pid)
            {
                completed = pidRecipeArr.pid_recipe_ptr_arr[i]->recipe;
                removePIDRecipe(&pidRecipeArr, pid);
                break;
            }
        }

        if (completed != NULL)
        {
            // insert it into the completed recipes
            insertArray(&completedRecipes, completed);
            // transfer recipes who's dependencies have finished into the work queue
            transferRecipes(&nonLeaves, &workQueue, &completedRecipes);
        }
    }
    activeCooks -= 1;
    errno = olderno;
}

int cook(COOKBOOK *cbp, char *mainRecipe, int maxCooks) {
    // create two lists of tasks, leaves and non leaves branching from the main receipe
    initArray(&nonLeaves, 100);
    initArray(&workQueue, 100);
    buildWorkQueue(&nonLeaves, &workQueue, cbp, mainRecipe);

    // have non leaves arr
    // have leaves/workqueue arr
    // have completed arr
    sigset_t mask_child, prev;
    Sigemptyset(&mask_child);
    Sigaddset(&mask_child, SIGCHLD);
    Signal(SIGCHLD, sigchld_handler);
    // init job lists 
    initArray(&completedRecipes, nonLeaves.currNumElem + workQueue.currNumElem);
    // init pid table
    initPIDArray(&pidRecipeArr, 100);

    int totalComplete = nonLeaves.currNumElem + workQueue.currNumElem;
    Sigprocmask(SIG_BLOCK, &mask_child, &prev);
    PIDRECIPE *pidRecipe;
    while(1)
    {
        if (exitFlag != 0)
        {   
            return 1;
        }
        if(completedRecipes.currNumElem == totalComplete)
        {
            freeArray(&nonLeaves);
            freeArray(&workQueue);
            freeArray(&completedRecipes);
            freePIDArray(&pidRecipeArr);
            return 0;
        }
        if (activeCooks < maxCooks && workQueue.currNumElem > 0)
        {
            RECIPE *recipeToExecute = pop(&workQueue);
            pid = Fork();
            if (pid == 0)
            {
                // unblock child
                // Sigprocmask(SIG_SETMASK, &prev, NULL);
                execRecipe(recipeToExecute);
                exit(0);
            }
            // parent
            // unblock child
            // Sigprocmask(SIG_SETMASK, &prev, NULL);
            activeCooks += 1;
            // create pid mapping
            pidRecipe = malloc(sizeof(PIDRECIPE));
            pidRecipe->recipe = recipeToExecute;
            pidRecipe->pid = pid;
            insertPIDArray(&pidRecipeArr, pidRecipe);
        }
        else
        {
            sigsuspend(&prev);
        }        
    }
}