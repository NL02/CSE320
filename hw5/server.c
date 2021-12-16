#include <time.h>
#include "server.h"
#include "client_registry.h"
#include "protocol.h"
#include "player.h"
#include "debug.h"
#include "csapp.h"


int debug_show_maze;
CLIENT_REGISTRY *client_registry;

void printPkt(MZW_PACKET *pkt)
{
    debug("Type %u\n", pkt->type);
    debug("param1 %d\n", pkt->param1);
    debug("param2 %d\n", pkt->param2);
    debug("param3 %d\n", pkt->param3);
    debug("Size %u\n", pkt->size);
    debug("Seconds %u\n", pkt->timestamp_sec);
    debug("Nano Seconds %u\n", pkt->timestamp_nsec);
}
void *mzw_client_service(void *arg) {
    // retrieve file descriptor
    int fd = *((int*) arg);
    // free the storage
    free(arg);
    // detach thread
    if (pthread_detach(pthread_self()) != 0)
    {
        debug("Pthread detach err");
    }
    // register the client file descriptor with the client regitry
    creg_register(client_registry, fd);
    
    // service loop
    int loginFlag = 0; // 1 for sucessful login
    MZW_PACKET *pkt = malloc(sizeof(MZW_PACKET));
    void *data = NULL;// space gets allocated in protocol
    PLAYER *playerPtr = NULL;
    struct timespec tms;
    timespec_get(&tms, TIME_UTC);
    while (1)
    {
        // receieve packet 
        if (proto_recv_packet(fd, pkt, &data) != 0)
        {
            debug("Error reading packet");
            break;
        }
        printPkt(pkt);
        // printf("data %s %ld\n", (char*)data, strlen((char*)data));
        int pktType = pkt->type;
        // login
        if (pktType == MZW_LOGIN_PKT)
        {
            playerPtr = player_login(fd, pkt->param1, (char*)data);
            loginFlag = 1;
            MZW_PACKET *resp = malloc(sizeof(MZW_PACKET));
            resp->param1 = 0;
            resp->param2 = 0;
            resp->param3 = 0;
            resp->size = 0;
            resp->timestamp_sec = tms.tv_sec;
            resp->timestamp_nsec = tms.tv_nsec;
            if (playerPtr)
            {
                resp->type = MZW_READY_PKT;
            }
            else
            {
                resp->type = MZW_INUSE_PKT;
            }
            proto_send_packet(fd, resp, NULL);
            // free(resp);
            if (playerPtr)
            {
                player_reset(playerPtr);
            }
        }
        if (loginFlag)
        {
            // move
            if (pktType == MZW_MOVE_PKT)
            {
                debug("IN MOVE");
            }
            // turn
            if (pktType == MZW_TURN_PKT)
            {
                debug("IN TURN");
            }
            // refresh
            if (pktType == MZW_REFRESH_PKT)
            {
                debug("IN REFRESH");
            }
            // send
            if (pktType == MZW_SEND_PKT)
            {
                debug("IN SEND");
            }
        }
    }
    
    return NULL;
}