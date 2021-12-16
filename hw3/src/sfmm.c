/**
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "debug.h"
#include "sfmm.h"

#define ROWSIZE 8 // bytes
#define HEADER 8 // bytes
#define BLOCKSIZE 64 // bytes
#define GETSIZE(p) ((p->header >> 6) << 6)
#define THIS_BLOCK_NOT_ALLOC 0

unsigned int getFib(int n)
{
    n = n + 2;
    if (n <= 1) {
        return n;
    }
 
    int previousFib = 0, currentFib = 1;
    for (int i = 0; i < n - 1; i++)
    {
        int newFib = previousFib + currentFib;
        previousFib = currentFib;
        currentFib = newFib;
    }
 
    return currentFib;
}

void initFreeList() {
    for(int i = 0; i < NUM_FREE_LISTS; i++)
    {
        sf_free_list_heads[i].body.links.next = &sf_free_list_heads[i];
        sf_free_list_heads[i].body.links.prev = &sf_free_list_heads[i];
    }
}

void *insertInFreeList(sf_block * freeBlock)
{
    // Size of the freeBlock in terms of M, where M = 64
    size_t blockMSize = GETSIZE(freeBlock) / BLOCKSIZE;
    // search for the correct slot in the free lists that can hold the block addr
    for(int i = 0; i < NUM_FREE_LISTS; i++)
    {
        unsigned int fibNum = getFib(i);
        if (blockMSize <= fibNum)
        {   
            // Case where we're inserting into an an "empty" stack
            //  ex: the sentinel's prev points to itself
            if (sf_free_list_heads[i].body.links.prev == &sf_free_list_heads[i])
            {
                sf_free_list_heads[i].body.links.prev = freeBlock;
            }
            // set the prev to the sentinel
            freeBlock->body.links.prev = &sf_free_list_heads[i];
            // set the next to the sentinel's next
            freeBlock->body.links.next = sf_free_list_heads[i].body.links.next;
            // set the sentinel's next to the free block
            sf_free_list_heads[i].body.links.next = freeBlock;
            return NULL;
        }
        // Case where we have a free block larger than the highest fib category 
        else if (blockMSize > fibNum && i == NUM_FREE_LISTS -1 )
        {
            // case for inserting into an "empty" stack
            if (sf_free_list_heads[i].body.links.prev == &sf_free_list_heads[i])
            {
                sf_free_list_heads[i].body.links.prev = freeBlock;
            }
            // set the prev to the sentinel
            freeBlock->body.links.prev = &sf_free_list_heads[i];
            // set the next to the sentinel's next 
            freeBlock->body.links.next = sf_free_list_heads[i].body.links.next;
            // set the sentinel's next to the free block
            sf_free_list_heads[i].body.links.next = freeBlock;
            return NULL;
        }
    }
    return NULL;
}

// Remove a block from the free lists
void *removeFromFree(sf_block *block)
{
    for(int i = 0; i< NUM_FREE_LISTS; i++) 
    {
        sf_block *prev = &sf_free_list_heads[i];
        sf_block *curr = sf_free_list_heads[i].body.links.next;
        while(curr != &sf_free_list_heads[i])
        {
            if(curr == block)
            {
                // set the prev's next to curr's next
                prev->body.links.next = curr->body.links.next;
                // set curr's next's prev node to the prev node
                curr->body.links.next->body.links.prev = prev;
                curr->body.links.next = NULL;
                curr->body.links.prev = NULL;
                return NULL;
            }
            prev = curr;
            curr = curr->body.links.next;
        }
    }
    return NULL;
}
void *setFreeHeaderAndFoot(size_t size, sf_block* block, int prevAlloc, int alloc)
{
    block->header = size | prevAlloc | alloc;
    sf_block * nextBlock = (void *)block + size;
    nextBlock->prev_footer = block->header;
    return NULL;
}

sf_block *setHeader(size_t size, sf_block *blockToSplit, int allocStatus)
{
    // set the new header status, new size, the prev alloc, and the alloc status
    blockToSplit->header = size | (blockToSplit->header & PREV_BLOCK_ALLOCATED) | allocStatus;
    
    // set the next block's prev alloc to the header 
    sf_block *nextBlock = (void *)blockToSplit + size;  
    nextBlock->header = GETSIZE(nextBlock) | PREV_BLOCK_ALLOCATED | (nextBlock->header & THIS_BLOCK_ALLOCATED);
    nextBlock->prev_footer = blockToSplit->header;
    return blockToSplit;
}

// A block to split is greater than or less than the size of adjusted size
sf_block *splitFreeBlock(size_t adjustedSize, sf_block *blockToSplit)
{
    // Allocate the whole block 
    if (GETSIZE(blockToSplit) - adjustedSize < BLOCKSIZE)
    {
        removeFromFree(blockToSplit);
        return setHeader(adjustedSize, blockToSplit, THIS_BLOCK_ALLOCATED);
    }

    // Case where a remainder split will occur
    removeFromFree(blockToSplit);
    int blockSize = GETSIZE(blockToSplit);
    // // The alloc portion
    sf_block *lower = setHeader(adjustedSize, blockToSplit, THIS_BLOCK_ALLOCATED);
    // The free remaining portion
    sf_block *upper = (void *)blockToSplit + adjustedSize;
    setFreeHeaderAndFoot(blockSize - adjustedSize, upper, PREV_BLOCK_ALLOCATED, THIS_BLOCK_NOT_ALLOC);
    insertInFreeList(upper);
    return lower;
}

sf_block *splitMallocBlock(size_t adjustedSize, sf_block *blockToSplit)
{
    int blockSize = GETSIZE(blockToSplit);
    blockToSplit = setHeader(adjustedSize, blockToSplit, THIS_BLOCK_ALLOCATED);
    sf_block *nextBlock = (void *)blockToSplit + adjustedSize;
    nextBlock->header = (blockSize - adjustedSize) | PREV_BLOCK_ALLOCATED | THIS_BLOCK_NOT_ALLOC;
    sf_block * nextNextBlock = (void*)nextBlock + blockSize - adjustedSize;
    nextNextBlock->prev_footer= nextBlock->header;
    return nextBlock;
}

sf_block *coalesce(sf_block *block)
{
    sf_block *prevBlock = (void *)block - ((block->prev_footer>>6) << 6);
    // Get prev alloc status, will be 2 if alloc'd
    int prevAlloc = block->header & PREV_BLOCK_ALLOCATED;
    // Get next alloc status, will be 1 if allo'cd
    sf_block *nextBlock = (void *)block + (GETSIZE(block));
    int nextAlloc = nextBlock->header & THIS_BLOCK_ALLOCATED;
    int size = GETSIZE(block);

    // both prev and next are alloc'd
    if (prevAlloc > 0 && nextAlloc > 0)
    {
        removeFromFree(block);
        return block;
    }
    // Previous is alloc and next is not alloc'd
    else if (prevAlloc > 0 && nextAlloc == 0 )
    {
        size += GETSIZE(nextBlock);
        block->header = size | (block->header & PREV_BLOCK_ALLOCATED) | THIS_BLOCK_NOT_ALLOC;
        sf_block * newNextBlock = (void *)block + size;
        newNextBlock->prev_footer = block->header;
        removeFromFree(block);
        removeFromFree(nextBlock);
        return block;
    }
    // Previous is not alloc'd and next is alloc'd
    else if (prevAlloc == 0 && nextAlloc > 0)
    {
        size += GETSIZE(prevBlock);
        prevBlock->header = size | (prevBlock->header & PREV_BLOCK_ALLOCATED) | THIS_BLOCK_NOT_ALLOC;
        nextBlock->prev_footer = prevBlock->header;
        removeFromFree(block);
        removeFromFree(prevBlock);
        return prevBlock;
    }
    // Both not alloc'd 
    else 
    {
        size += GETSIZE(prevBlock) + GETSIZE(nextBlock);
        prevBlock->header = size | (prevBlock->header * PREV_BLOCK_ALLOCATED) | THIS_BLOCK_NOT_ALLOC;
        sf_block * newNextBlock = (void *)prevBlock + size;
        newNextBlock->prev_footer = prevBlock->header;
        removeFromFree(nextBlock);
        removeFromFree(block);
        removeFromFree(prevBlock);
        return prevBlock;
    }
}

void *allocMoreMemory() 
{
    // sf_show_heap();
    sf_block *prevEpiloguePtr = (sf_block *) (sf_mem_end() -  2 *ROWSIZE);
    // Grow the heap
    sf_block *newPagePtr = (sf_block *) sf_mem_grow();
    // If the heap cannot grow anymore
    if (newPagePtr == NULL)
    {
        sf_errno = ENOMEM;
        return NULL;
    }
    // Set the attributes of the prev epilogue to the new page 
    // prevEpiloguePtr->header = PAGE_SZ | PREV_BLOCK_ALLOCATED | THIS_BLOCK_NOT_ALLOC;

    prevEpiloguePtr->header = PAGE_SZ | (prevEpiloguePtr->header & PREV_BLOCK_ALLOCATED) | THIS_BLOCK_NOT_ALLOC;
    prevEpiloguePtr->prev_footer = prevEpiloguePtr->prev_footer;

    // Set the attributes of the new epilogue
    sf_block *newEpiloguePtr = (sf_block *) (sf_mem_end() -  2 * ROWSIZE);
    newEpiloguePtr->header = 0 | THIS_BLOCK_ALLOCATED;

    newEpiloguePtr->prev_footer = prevEpiloguePtr->header;
    // sf_show_heap();
    sf_block* coalescedBlock = coalesce(prevEpiloguePtr);
    insertInFreeList(coalescedBlock);

    return coalescedBlock;
}
// Look for blocks that can hold a block of this adjuted size and split and coalesce
sf_block *findFit(size_t adjustedSize)
{
    // loop through the free list in order to find a block to malloc
    for(int i = 0; i < NUM_FREE_LISTS; i++)
    {
        // get the head
        sf_block *sentinel = &sf_free_list_heads[i];
        // set the curr node
        sf_block *curr = sentinel->body.links.next;
        while(curr != sentinel)
        {
            // when we reach a block that can store the size
            if (adjustedSize <= GETSIZE(curr)) {
                return splitFreeBlock(adjustedSize, curr);
            }
            // loop through it's next until we reach sentinel again
            curr = curr->body.links.next;
        }
    }
    // If we reach here then we know that a block doesn't exist for the adjusted size, need to grow.  
    if (allocMoreMemory() == NULL)
    {
        return NULL;
    }
    return findFit(adjustedSize);
}

void *sf_malloc(size_t size) {
    // invalid case
    if (size == 0)
    {
        return NULL;
    }
    // Case for first initilization 
    if (sf_mem_start() == sf_mem_end())
    {
        // initliaze the free nodes
        initFreeList();

        // pad by 6 empty rows 
        sf_block* prologueStart = (sf_block*) (sf_mem_grow() + 6 * ROWSIZE);
        // set the block size of the prologue
        prologueStart->header = 64 | THIS_BLOCK_ALLOCATED;

        sf_block* firstFree = (void*)prologueStart + 64;
        // Size =  8192(page) - 64 (prologue) - 64 (epilogue)
        firstFree->header = 8064 | PREV_BLOCK_ALLOCATED;
        
        // insert into the free list arr
        insertInFreeList(firstFree);

        // create the epilogue
        sf_block* epilogueStart = (sf_block*) (sf_mem_end() - 2 * ROWSIZE);
        // set the prevfooter to the new chunk
        epilogueStart->prev_footer = firstFree->header;
        // set the epilogue header information
        epilogueStart->header = 0 | THIS_BLOCK_ALLOCATED;
    }
    size_t adjustedSize;

    // If the size plus the header is less than 64 (min block size)
    if (size + HEADER < BLOCKSIZE) {
        // Set the size to the min block size
        adjustedSize = BLOCKSIZE;
    } 
    // the size needs to be 64 byte aligned
    else 
    {
        adjustedSize = size + HEADER;
        if ((size + HEADER) % BLOCKSIZE != 0) {
            adjustedSize += ((adjustedSize/BLOCKSIZE) + 1) * BLOCKSIZE - adjustedSize;
        }
    }
    sf_block *allocBlockPtr = findFit(adjustedSize);
    
    if (allocBlockPtr != NULL)
    {
        return allocBlockPtr->body.payload;
    }
    sf_errno = ENOMEM;
    return NULL;
}

void sf_free(void *pp) {
    // Invalid Cases
    // Null pointer
    if (pp == NULL)
    {
        abort();
    }
    
    // pointer is not 64 bit aligned
    if ((uintptr_t)pp % BLOCKSIZE != 0)
    {
         abort();
    }
    // The header of the block is before the start of the first block of 
    //  the heap, or the footer of the block is after the end of the last
    //   block in the heap.
    sf_block * ppBlock = pp - 2 * ROWSIZE;
    // need to double check this 
    if ((void*)ppBlock + ROWSIZE < sf_mem_start() + 15 * ROWSIZE || (void *)ppBlock + GETSIZE(ppBlock) - ROWSIZE > sf_mem_end() - ROWSIZE)
    {
        abort();
    }
    // The allocated bit in the header is 0
    // sf_block * ppBlock = pp - 2 * ROWSIZE;
    if ((ppBlock->header & THIS_BLOCK_ALLOCATED) == 0)
    {
        abort();
    }
    // The prev_alloc field in the header is 0, indicating that the previous 
    //  block is free, but the alloc field of the previous block header is not 0.
    sf_block *prevBlock = (void*)ppBlock - ((ppBlock->prev_footer >> 6) << 6);
    // printf("size of prev %ld",(ppBlock->prev_footer >> 6) << 6 );
    // sf_show_block(prevBlock);
    // printf("\n");
    if ((ppBlock->header & PREV_BLOCK_ALLOCATED) == 0 && (prevBlock->header & THIS_BLOCK_ALLOCATED) != 0)
    {
        abort();
    }
    // If we reach here we know the pointer is valid and can free it 
    // Set the alloc to 0 update header and footer 
    ppBlock->header = GETSIZE(ppBlock) | (ppBlock->header & PREV_BLOCK_ALLOCATED) | THIS_BLOCK_NOT_ALLOC;
    sf_block *nextBlock = (void*)ppBlock + GETSIZE(ppBlock);
    nextBlock->prev_footer = ppBlock->header;
    // Set the next block's pal to 0 
    nextBlock->header = GETSIZE(nextBlock) | (PREV_BLOCK_ALLOCATED & 0) | (nextBlock->header & THIS_BLOCK_ALLOCATED);
    // insert into the free list 
    insertInFreeList(coalesce(ppBlock));
    return;
}

void *sf_realloc(void *pp, size_t rsize) {
    // Invalid Cases
    // Null pointer
    if (pp == NULL)
    {
        sf_errno = EINVAL;
        return NULL;
    }
    
    // pointer is not 64 bit aligned
    if ((uintptr_t)pp % BLOCKSIZE != 0)
    {
        sf_errno = EINVAL;
        return NULL;
    }
    // The header of the block is before the start of the first block of 
    //  the heap, or the footer of the block is after the end of the last
    //   block in the heap.
    sf_block * ppBlock = pp - 2 * ROWSIZE;
    // need to double check this 
    if ((void*)ppBlock + ROWSIZE < sf_mem_start() + 15 * ROWSIZE || (void *)ppBlock + GETSIZE(ppBlock) - ROWSIZE > sf_mem_end() - ROWSIZE)
    {
        sf_errno = EINVAL;
        return NULL;
    }
    // The allocated bit in the header is 0
    if ((ppBlock->header & THIS_BLOCK_ALLOCATED) == 0)
    {
        sf_errno = EINVAL;
        return NULL;
    }
    // The prev_alloc field in the header is 0, indicating that the previous 
    //  block is free, but the alloc field of the previous block header is not 0.
    sf_block *prevBlock = (void*)ppBlock - ((ppBlock->prev_footer >> 6) << 6) + ROWSIZE;
    if ((ppBlock->header & PREV_BLOCK_ALLOCATED) == 0 && (prevBlock->header & THIS_BLOCK_ALLOCATED) != 0)
    {
        sf_errno = EINVAL;
        return NULL;
    }
    // Valid but size is 0
    if(rsize == 0)
    {
        sf_free(pp);
        return NULL;
    }

    sf_block *block = pp - 2 * ROWSIZE;
    // reallocating to a larger size 
    int size = GETSIZE(block);
    if (size < rsize)
    {
        void *rePtr = sf_malloc(rsize);
        if (rePtr == NULL)
        {
            sf_errno = ENOMEM;
            return NULL;
        }
        memcpy(rePtr, pp, size - 8);
        sf_free(pp);
        return rePtr;
    }
    // reallocating to a smaller size 
    if (size >= rsize)
    {
        // splitting the returned block results in a splinter
        // don't split the block. Leave the splinter in the block, update the header field if necessary
        // return the same block back to the caller 
        if (size - rsize - ROWSIZE < 64)
        {
            return pp;
        }
        // The block can be split without creating a splinter. 
        // Split the block, update the block size fields in both headers
        // Free the remainder block by inserting it into the appropriate free list 
        // return a pointer to the payload of the now-smaller block to the caller 
        else 
        {
            size_t adjustedSize;

            // If the size plus the header is less than 64 (min block size)
            if (rsize < BLOCKSIZE) {
                // Set the size to the min block size
                adjustedSize = BLOCKSIZE;
            } 
            // the size needs to be 64 byte aligned
            else 
            {
                adjustedSize = rsize;
                if (adjustedSize % BLOCKSIZE != 0) {
                    adjustedSize += ((adjustedSize/BLOCKSIZE) + 1) * BLOCKSIZE - adjustedSize;
                }
            }
            sf_block *blockToSplit = pp - 2*ROWSIZE;
            // insert the other part of the block after coalescing
            insertInFreeList(coalesce(splitMallocBlock(adjustedSize, blockToSplit)));
            return pp;
        }
    }

    return NULL;
}
