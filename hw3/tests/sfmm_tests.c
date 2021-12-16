#include <criterion/criterion.h>
#include <errno.h>
#include <signal.h>
#include "debug.h"
#include "sfmm.h"
#define TEST_TIMEOUT 15

/*
 * Assert the total number of free blocks of a specified size.
 * If size == 0, then assert the total number of all free blocks.
 */
void assert_free_block_count(size_t size, int index, int count) {
    int cnt = 0;
    for(int i = 0; i < NUM_FREE_LISTS; i++) {
	sf_block *bp = sf_free_list_heads[i].body.links.next;
	while(bp != &sf_free_list_heads[i]) {
	    if(size == 0 || size == (bp->header & ~0x3f)) {
		cnt++;
		if(size != 0) {
		    cr_assert_eq(index, i, "Block %p (size %ld) is in wrong list for its size "
				 "(expected %d, was %d)",
				 (long *)(bp) + 1, bp->header & ~0x3f, index, i);
		}
	    }
	    bp = bp->body.links.next;
	}
    }
    if(size == 0) {
	cr_assert_eq(cnt, count, "Wrong number of free blocks (exp=%d, found=%d)",
		     count, cnt);
    } else {
	cr_assert_eq(cnt, count, "Wrong number of free blocks of size %ld (exp=%d, found=%d)",
		     size, count, cnt);
    }
}

Test(sfmm_basecode_suite, malloc_an_int, .timeout = TEST_TIMEOUT) {
	sf_errno = 0;
	size_t sz = sizeof(int);
	int *x = sf_malloc(sz);

	cr_assert_not_null(x, "x is NULL !");

	*x = 4;

	cr_assert(*x == 4, "sf_malloc failed to give proper space for an int!");

	assert_free_block_count(0, 0, 1);
	assert_free_block_count(8000, 8, 1);

	cr_assert(sf_errno == 0, "sf_errno is not zero!");
	cr_assert(sf_mem_start() + PAGE_SZ == sf_mem_end(), "Allocated more than necessary!");
}

Test(sfmm_basecode_suite, malloc_four_pages, .timeout = TEST_TIMEOUT) {
	sf_errno = 0;

	void *x = sf_malloc(32624);
	cr_assert_not_null(x, "x is NULL!");
	assert_free_block_count(0, 0, 0);
	cr_assert(sf_errno == 0, "sf_errno is not 0!");
}

Test(sfmm_basecode_suite, malloc_too_large, .timeout = TEST_TIMEOUT) {
	sf_errno = 0;
	void *x = sf_malloc(524288);

	cr_assert_null(x, "x is not NULL!");
	assert_free_block_count(0, 0, 1);
	assert_free_block_count(130944, 8, 1);
	cr_assert(sf_errno == ENOMEM, "sf_errno is not ENOMEM!");
}

Test(sfmm_basecode_suite, free_no_coalesce, .timeout = TEST_TIMEOUT) {
	sf_errno = 0;
	size_t sz_x = 8, sz_y = 200, sz_z = 1;
	/* void *x = */ sf_malloc(sz_x);
	void *y = sf_malloc(sz_y);
	/* void *z = */ sf_malloc(sz_z);

	sf_free(y);

	assert_free_block_count(0, 0, 2);
	assert_free_block_count(256, 3, 1);
	assert_free_block_count(7680, 8, 1);

	cr_assert(sf_errno == 0, "sf_errno is not zero!");
}

Test(sfmm_basecode_suite, free_coalesce, .timeout = TEST_TIMEOUT) {
	sf_errno = 0;
	size_t sz_w = 8, sz_x = 200, sz_y = 300, sz_z = 4;
	/* void *w = */ sf_malloc(sz_w);
	void *x = sf_malloc(sz_x);
	void *y = sf_malloc(sz_y);
	/* void *z = */ sf_malloc(sz_z);

	sf_free(y);
	sf_free(x);

	assert_free_block_count(0, 0, 2);
	assert_free_block_count(576, 5, 1);
	assert_free_block_count(7360, 8, 1);

	cr_assert(sf_errno == 0, "sf_errno is not zero!");
}

Test(sfmm_basecode_suite, freelist, .timeout = TEST_TIMEOUT) {
        size_t sz_u = 200, sz_v = 300, sz_w = 200, sz_x = 500, sz_y = 200, sz_z = 700;
	void *u = sf_malloc(sz_u);
	/* void *v = */ sf_malloc(sz_v);
	void *w = sf_malloc(sz_w);
	/* void *x = */ sf_malloc(sz_x);
	void *y = sf_malloc(sz_y);
	/* void *z = */ sf_malloc(sz_z);

	sf_free(u);
	sf_free(w);
	sf_free(y);

	assert_free_block_count(0, 0, 4);
	assert_free_block_count(256, 3, 3);
	assert_free_block_count(5696, 8, 1);

	// First block in list should be the most recently freed block.
	int i = 3;
	sf_block *bp = sf_free_list_heads[i].body.links.next;
	cr_assert_eq(bp, (char *)y - 16,
		     "Wrong first block in free list %d: (found=%p, exp=%p)",
                     i, bp, (char *)y - 16);
}

Test(sfmm_basecode_suite, realloc_larger_block, .timeout = TEST_TIMEOUT) {
        size_t sz_x = sizeof(int), sz_y = 10, sz_x1 = sizeof(int) * 20;
	void *x = sf_malloc(sz_x);
	/* void *y = */ sf_malloc(sz_y);
	x = sf_realloc(x, sz_x1);

	cr_assert_not_null(x, "x is NULL!");
	sf_block *bp = (sf_block *)((char *)x - 16);
	cr_assert(*((long *)(bp) + 1) & 0x1, "Allocated bit is not set!");
	cr_assert((*((long *)(bp) + 1) & ~0x3f) == 128,
		  "Realloc'ed block size (%ld) not what was expected (%ld)!",
		  *((long *)(bp) + 1) & ~0x3f, 128);

	assert_free_block_count(0, 0, 2);
	assert_free_block_count(64, 0, 1);
	assert_free_block_count(7808, 8, 1);
}

Test(sfmm_basecode_suite, realloc_smaller_block_splinter, .timeout = TEST_TIMEOUT) {
        size_t sz_x = sizeof(int) * 20, sz_y = sizeof(int) * 16;
	void *x = sf_malloc(sz_x);
	void *y = sf_realloc(x, sz_y);

	cr_assert_not_null(y, "y is NULL!");
	cr_assert(x == y, "Payload addresses are different!");

	sf_block *bp = (sf_block *)((char*)y - 16);
	cr_assert(*((long *)(bp) + 1) & 0x1, "Allocated bit is not set!");
	cr_assert((*((long *)(bp) + 1) & ~0x3f) == 128,
		  "Block size (%ld) not what was expected (%ld)!",
	          *((long *)(bp) + 1) & ~0x3f, 128);

	assert_free_block_count(0, 0, 1);
	assert_free_block_count(7936, 8, 1);
}

Test(sfmm_basecode_suite, realloc_smaller_block_free_block, .timeout = TEST_TIMEOUT) {
        size_t sz_x = sizeof(double) * 8, sz_y = sizeof(int);
	void *x = sf_malloc(sz_x);
	void *y = sf_realloc(x, sz_y);

	cr_assert_not_null(y, "y is NULL!");

	sf_block *bp = (sf_block *)((char *)y - 16);
	cr_assert(*((long *)(bp) + 1) & 0x1, "Allocated bit is not set!");
	cr_assert((*((long *)(bp) + 1) & ~0x3f) == 64,
		  "Realloc'ed block size (%ld) not what was expected (%ld)!",
		  *((long *)(bp) + 1) & ~0x3f, 64);

	assert_free_block_count(0, 0, 1);
	assert_free_block_count(8000, 8, 1);
}

//############################################
//STUDENT UNIT TESTS SHOULD BE WRITTEN BELOW
//DO NOT DELETE THESE COMMENTS
//############################################

// 1 Test for freeing a null pointer
Test(sfmm_student_suite, student_test_1_freeAndReallocNULL, .timeout = TEST_TIMEOUT, .signal = SIGABRT)
{
	// Null pointer
	sf_free(NULL);
}

// 2 Test for freeing a non-aligned pointer
Test(sfmm_student_suite, student_test_2_freeAndReallocNotByteAligned, .timeout = TEST_TIMEOUT, .signal = SIGABRT)
{
	size_t sz_x = sizeof(double) * 8;
	//Pointer is not 64 byte aligned due to +1
	void *notAligned = sf_malloc(sz_x) + 1;
	sf_free(notAligned);
}

// 3 Test for freeing a free block
Test(sfmm_student_suite, student_test_3_freeAndReallocNonMallocPointer, .timeout = TEST_TIMEOUT, .signal = SIGABRT)
{
	size_t sz_x = sizeof(double) * 8;
	void *x = sf_malloc(sz_x);
	sf_free(x);
	sf_free(x);
}

// 4 Test for Header of the block is before the start of the first block of the heap
Test(sfmm_student_suite, student_test_4_freeAndReallocHeader, .timeout = TEST_TIMEOUT, .signal = SIGABRT)
{
	void *beforeFirstBlock = sf_mem_start();
	size_t sz_x = sizeof(double) * 8;
	/* void *x */ sf_malloc(sz_x);
	sf_free(beforeFirstBlock);
}

// 5 Test for Footer of the block is after the end of the last block in the heap
Test(sfmm_student_suite, student_test_5_freeAndReallocFooter, .timeout = TEST_TIMEOUT, .signal = SIGABRT)
{
	// Allocate a whole page
	sf_block *afterLastBlock = (void*)sf_malloc(8064) - 16;
	// Increase the size so that the footer addr would be beyond the epilogue
	afterLastBlock->header = (((afterLastBlock->header >> 6 ) << 6) + 64) | PREV_BLOCK_ALLOCATED | THIS_BLOCK_ALLOCATED ;
	sf_free(afterLastBlock);
}

// 6 Test for Prev alloc in the header is 0, indicating the previous block is free
//	,but the alloc field of the previous block header is not 0
Test(sfmm_student_suite, student_test_6_freeAndReallocIncorrect_Heading, .timeout = TEST_TIMEOUT, .signal = SIGABRT)
{
	size_t sz_x = sizeof(double) * 8;
	sf_block *incorrectHeader = (void*)sf_malloc(sz_x) - 16;
	sf_block *nextBlock = (void*)sf_malloc(sz_x) - 16;
	void *payload = (void *) incorrectHeader + 16;
	// set the block to free or not alloc'd 
	incorrectHeader->header = ((incorrectHeader->header >> 6) << 6) | PREV_BLOCK_ALLOCATED | (THIS_BLOCK_ALLOCATED & 0);
	// set the footer as well
	nextBlock->prev_footer = incorrectHeader->header;
	// try freeing the next block
	sf_free(payload);
}


// 7 Test to ensure that once exactly a page is mallocd another malloc can be used without
// 	compromising the heap
Test(sfmm_student_suite, student_test_7_ensuringAllocStatus, . timeout = TEST_TIMEOUT)
{
	// size of a block before the need for another page when initiating the prologue and the epiogue
	size_t sz_v = 8056;
	/*void *v*/ sf_malloc(sz_v);
	sf_malloc(128);
	// Assert no free blocks exist as we allocated everything
	assert_free_block_count(8000, 8, 1);
}

// 8 Test for reallocing a block with the same size, ensure that the heap doesn't change 
Test(sfmm_student_suite, student_test_8_ensuringReallocSameSize, . timeout = TEST_TIMEOUT)
{
	size_t sz_x = 8056;
	void *x = sf_malloc(sz_x);
	sf_realloc(x, 8056);
	assert_free_block_count(0, 0, 0);
}