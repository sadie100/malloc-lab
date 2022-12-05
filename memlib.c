/*
 * memlib.c - a module that simulates the memory system.  Needed because it
 *            allows us to interleave calls from the student's malloc package
 *            with the system's malloc package in libc.
 */
#include "memlib.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "config.h"

/* private variables */
static char *mem_start_brk; /* points to first byte of heap */
static char *mem_brk;       /* points to last byte of heap */
static char *mem_max_addr;  /* largest legal heap address */
void *heap_listp;

/*
 * mem_init - initialize the memory system model
 힙에 가용한 가상메모리를 큰 더블 워드로 정렬된 바이트의 배열로 모델한 것
 */
void mem_init(void) {
  /* allocate the storage we will use to model the available VM */
  if ((mem_start_brk = (char *)malloc(MAX_HEAP)) == NULL) {
    fprintf(stderr, "mem_init_vm: malloc error\n");
    exit(1);
  }

  mem_max_addr = mem_start_brk + MAX_HEAP; /* max legal heap address */
  mem_brk = mem_start_brk;                 /* heap is empty initially */
}

/*
 * mem_deinit - free the storage used by the memory system model
 */
void mem_deinit(void) { free(mem_start_brk); }

/*
 * mem_reset_brk - reset the simulated brk pointer to make an empty heap
 */
void mem_reset_brk() { mem_brk = mem_start_brk; }

/*
 * mem_sbrk - simple model of the sbrk function. Extends the heap
 *    by incr bytes and returns the start address of the new area. In
 *    this model, the heap cannot be shrunk.
 */
void *mem_sbrk(int incr) {
  char *old_brk = mem_brk;

  if ((incr < 0) || ((mem_brk + incr) > mem_max_addr)) {
    // 음수값을 인자로 받거나, incr을 적용한 주소값이 max를 초과했을 경우
    errno = ENOMEM;
    fprintf(stderr, "ERROR: mem_sbrk failed. Ran out of memory...\n");
    return (void *)-1;
  }
  mem_brk += incr;
  // old_brk를 리턴하는 이유 : 새로 만든 영역의 시작 주소를 리턴하기 위함.
  return (void *)old_brk;
}

/*
 * mem_heap_lo - return address of the first heap byte
 */
void *mem_heap_lo() { return (void *)mem_start_brk; }

/*
 * mem_heap_hi - return address of last heap byte
 */
void *mem_heap_hi() { return (void *)(mem_brk - 1); }

/*
 * mem_heapsize() - returns the heap size in bytes
 */
size_t mem_heapsize() { return (size_t)(mem_brk - mem_start_brk); }

/*
 * mem_pagesize() - returns the page size of the system
 */
size_t mem_pagesize() { return (size_t)getpagesize(); }
