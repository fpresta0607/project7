/**
 * @file freemem.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2020.  All rights reserved. */

#include <xinu.h>

/**
 * Frees a block of heap-allocated memory.
 *
 * @param memptr
 *      Pointer to memory block allocated with memget().
 *
 * @param nbytes
 *      Length of memory block, in bytes.  (Same value passed to memget().)
 *
 * @return
 *      ::OK on success; ::SYSERR on failure.  This function can only fail
 *      because of memory corruption or specifying an invalid memory block.
 */
syscall freemem(void *memptr, ulong nbytes)
{
    register struct memblock *block, *next, *prev;
    ulong top;

    /* make sure block is in heap */
    if ((0 == nbytes)
        || ((ulong)memptr < freelist.base)
        || ((ulong)memptr > freelist.base + freelist.bound))
    {
        return SYSERR;
    }

    block = (struct memblock *)memptr;
    nbytes = (ulong)roundmb(nbytes);

    /* TODO:
     *      - Disable interrupts
     *      - Find where the memory block should
     *        go back onto the freelist (based on address)
     *      - Find top of previous memblock
     *      - Make sure block is not overlapping on prev or next blocks
     *      - Coalesce with previous block if adjacent
     *      - Coalesce with next block if adjacent
     *      - Restore interrupts
     */
	int ps = disable();
		
	next = (struct memblock*)freelist.head;
	prev = (struct memblock*)&freelist;

	while(next != NULL)
	{
		if((prev<block) && (block < next))
		{
			prev->next = block;
			block->next = next;
			
			block->length = nbytes;

			freelist.size  = freelist.size + nbytes;
			break;
		}

		prev = next;
		next = next->next;
	}
	
	restore(ps);
	memblk *p = freelist.head;
	kprintf("memory freed: [0x%08X : freed %10d bytes : 0x%08X]\n", p, p->length, p->next);	

    return OK;
}
