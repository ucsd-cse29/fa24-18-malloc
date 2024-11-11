#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>


uint64_t* HEAP_START;
#define HEAP_SIZE 1024
#define VAL_SIZE 8
#define HEAP_END (HEAP_START + (HEAP_SIZE / VAL_SIZE))

uint64_t size(uint64_t* heap_ptr) {
  return *heap_ptr & 0xFFFFFFFFFFFFFFFE;
}
uint64_t val_size(uint64_t* heap_ptr) {
  return (*heap_ptr & 0xFFFFFFFFFFFFFFFE) / VAL_SIZE;
}
uint8_t isfree(uint64_t* heap_ptr) {
  return (*heap_ptr & 1) == 0;
}


void init_heap() {
	HEAP_START = mmap(NULL, HEAP_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
  // Set the first value in the heap to be the size minus one value
  *HEAP_START = HEAP_SIZE - VAL_SIZE;
  *(HEAP_START + (HEAP_SIZE / VAL_SIZE)) = HEAP_SIZE - VAL_SIZE;
}

void* do_alloc(uint64_t* heap_ptr, size_t size_req) {
  printf("comparing: %llu %lu\n", size(heap_ptr), size_req);
  if(size(heap_ptr) >= (size_req + 16)) {
    size_t size_rounded = size_req % 16 == 0 ? size_req : ((size_req + 16) / 16) * 16;
    uint64_t* split_after = heap_ptr + 1 + (size_rounded / VAL_SIZE);    
    printf("Writing to %p that %llu bytes free\n", split_after, size(heap_ptr) - size_rounded - VAL_SIZE);
    *split_after = size(heap_ptr) - size_rounded - VAL_SIZE;
    *heap_ptr = size_rounded;
  }
  *heap_ptr |= 1;
  printf("%llu\n", *heap_ptr);
  return heap_ptr + 1;
}

void* mymalloc(size_t size_req) {
  uint64_t* heap_ptr = HEAP_START;
  while(heap_ptr < HEAP_END) {
    if(isfree(heap_ptr) && ((size(heap_ptr) >= size_req))) {
      return do_alloc(heap_ptr, size_req); 
    }
    heap_ptr += val_size(heap_ptr) + 1;
  }
  return NULL;
}


void myfree(void* v) {
  uint64_t* heap_ptr = ((uint64_t*)v) - 1;
  if(isfree(heap_ptr)) { return; }
  *heap_ptr -= 1; 
}

void show_heap() {
  printf("HEAP_START: %p = %llu, HEAP_END: %p\n", HEAP_START, *HEAP_START, HEAP_END);
  uint64_t* heap_ptr = HEAP_START;
  printf("ADDRESS\t\tFREE?\tSIZE\n\n");
  while(heap_ptr < HEAP_END) {
    printf("%p\t", heap_ptr);
    printf("%s\t", *heap_ptr % 2 == 0 ? "FREE" : "ALLOC");
    printf("%llu\t", (*heap_ptr) / 2 * 2);
    heap_ptr += val_size(heap_ptr) + 1;
    printf("\n");
  }
  if(0) {
    printf("\n(RAW)\n");
    printf("ADDRESS\t\tVALUE");
    heap_ptr = HEAP_START;
    while(heap_ptr < HEAP_END) {
      printf("%p\t\t%llu\t\t%llx\n", heap_ptr, *heap_ptr, *heap_ptr); 
      heap_ptr += 1;
    }
  }
}


int main() {
  init_heap();
  show_heap();
  int* hi = mymalloc(10);
  show_heap();
  int* hi2 = mymalloc(100);
  show_heap();
  myfree(hi);
  show_heap();
  int* hi3 = mymalloc(10);
  show_heap();
}



