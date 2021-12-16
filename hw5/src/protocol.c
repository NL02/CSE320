#include "protocol.h"
#include "csapp.h"
#include "debug.h"

int proto_send_packet(int fd, MZW_PACKET *pkt, void *data) {
    // save the payload size before conversion
    uint16_t payloadSize = pkt->size;

    // dont need to worry about converting type and params as there is no endianess
    pkt->size = htons(pkt->size);
    pkt->timestamp_sec = htonl(pkt->timestamp_sec);
    pkt->timestamp_nsec = htonl(pkt->timestamp_nsec);

    // short count
    int currSendCount = 0;
    size_t bytesWritten = 0;
    MZW_PACKET *sendWriter = pkt;
    while (currSendCount < sizeof(MZW_PACKET)) {
        bytesWritten = write(fd, sendWriter , sizeof(MZW_PACKET) - currSendCount);
        if (bytesWritten <= 0 )
        {
            if (bytesWritten == 0 && errno == EINTR)
            {
                errno = 0;
                pkt->type = MZW_NO_PKT;
                return 0;
            }
            else
            {
                return -1;
            }
        }
        currSendCount += bytesWritten;
        sendWriter += bytesWritten;
    }
    // size_t ret = 0;
    // if ((ret = rio_writen(fd, pkt, sizeof(MZW_PACKET))) != sizeof(MZW_PACKET)) 
    // {
    //     if (ret == 0)
    //     {
    //         pkt->type = MZW_NO_PKT;      
    //     }
    //     debug("writing to mzw packet %ld", ret);
    //     return -1;
    // }
    
    // if payload and nonzero size
    if (data && payloadSize > 0) 
    {
        // short count
        int currPayloadSize = 0;
        bytesWritten = 0;
        void *dataWriter = data;
        while (currPayloadSize < payloadSize)
        {
            bytesWritten = write(fd, dataWriter, payloadSize - currPayloadSize);
            if (bytesWritten <= 0)
            {
                if (bytesWritten == 0 && errno == EINTR)
                {
                    errno = 0;
                    pkt->type = MZW_NO_PKT;
                    return 0;
                }
                else
                {
                    return -1;
                }
            }
            data = data + bytesWritten;
            currPayloadSize += bytesWritten;
        }
        
        // if ((ret = rio_writen(fd, data, payloadSize)) != payloadSize) 
        // {
        //     if (ret == 0)
        //     {
        //         pkt->type = MZW_NO_PKT;
        //     }
        //     debug("writing to payload %ld", ret);
        //     return -1;
        // }
    }
    return 0;
}

int proto_recv_packet(int fd, MZW_PACKET *pkt, void **datap) {
    // short count
    int currReadCount = 0;
    int bytesRead = 0;
    MZW_PACKET *recvRead = pkt;
    while (currReadCount < sizeof(MZW_PACKET)) {
        bytesRead = read(fd, recvRead, sizeof(MZW_PACKET) - currReadCount);
        if (bytesRead <= 0)
        {
            if (bytesRead == 0 && errno == EINTR)
            {
                errno = 0;
                pkt->type = MZW_NO_PKT;
                return 0;
            }
            else
            {
                return -1;
            }
        }
        currReadCount += bytesRead;
        recvRead += bytesRead;
    }
    // size_t ret; 
    // if ((ret = rio_readn(fd, pkt,sizeof(MZW_PACKET))) != sizeof(MZW_PACKET))
    // {
    //     if (ret == 0)
    //     {
    //         pkt->type = MZW_NO_PKT;
    //     }
    //     debug("reading mzw_packet %ld", ret);
    //     return -1;
    // }

    // convert back to host byte order
    pkt->size = ntohs(pkt->size);
    pkt->timestamp_sec = ntohl(pkt->timestamp_sec);
    pkt->timestamp_nsec = ntohl(pkt->timestamp_nsec);

    // if data and the nonzero size
    if (datap && pkt->size > 0) 
    {
        char *data = NULL;
        data = malloc((pkt->size) * sizeof(char));

        // short count
        int currPayloadSize = 0;
        bytesRead = 0;
        void *dataReader = data;
        while (currPayloadSize < pkt->size)
        {
            bytesRead = read(fd,data, pkt->size - currPayloadSize);
            if (bytesRead <= 0)
            {
                if (bytesRead == 0 && errno == EINTR)
                {
                    errno = 0;
                    pkt->type = MZW_NO_PKT;
                    return 0;
                }
                else 
                {
                    free(data);
                    return -1;
                }
            }
            currPayloadSize += bytesRead;
            dataReader += bytesRead;
        }
        // if ((ret = rio_readn(fd,data, pkt->size)) != pkt->size)
        // {
        //     if (ret == 0)
        //     {
        //         pkt->type = MZW_NO_PKT;
        //     }
        //     free(data);
        //     debug("reading payload %ld", ret);
        //     return -1;
        // }
        
        *datap = data;
    }
    return 0; 
}