#include <sys/select.h>
#include "client_registry.h"
#include "csapp.h"
#include "debug.h"

struct client_registry {
    int currFdCount;
    sem_t mutex;
    sem_t numMutex;
    fd_set clientFd;
};

CLIENT_REGISTRY * creg_init() {
    debug("Initialize client registry");
    CLIENT_REGISTRY *cr = malloc(sizeof(CLIENT_REGISTRY));
    cr->currFdCount = 0;
    FD_ZERO(&(cr->clientFd));
    sem_init(&(cr->mutex), 0, 1);
    sem_init(&(cr->numMutex), 0, 1);
    return cr;
}

void creg_fini(CLIENT_REGISTRY *cr) {
    free(cr);
}

void creg_register(CLIENT_REGISTRY *cr, int fd) {
    FD_SET(fd, &(cr->clientFd));
    cr->currFdCount += 1;
    if (cr->currFdCount == 1)
    {
        sem_wait(&(cr->numMutex));
        int resp;
        sem_getvalue(&(cr->numMutex), &resp);
    }
}

void creg_unregister(CLIENT_REGISTRY *cr, int fd) {
    FD_CLR(fd, &(cr->clientFd));
    cr->currFdCount -= 1;
    if ((cr->currFdCount) == 0)
    {
        sem_post(&(cr->numMutex));
    }
}

void creg_wait_for_empty(CLIENT_REGISTRY *cr) {
    sem_wait(&(cr->numMutex));
}

void creg_shutdown_all(CLIENT_REGISTRY *cr) {
    sem_wait(&(cr->mutex));
    for (int i = 0; i < FD_SETSIZE; i++) 
    {
        if (FD_ISSET(i, &(cr->clientFd)))
        {
            shutdown(i, SHUT_RD);
        }
    }
    sem_post(&(cr->mutex));
}
