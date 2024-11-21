// After we fixed the bugs from Wednesday's lecture on Nov 20, this is where we ended

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>

#define HEAP_SIZE 400
#define VAL_SIZE 8

uint64_t* HEAP_START = NULL;

void init_heap() {
  // Hey operating system, where can I start my heap!
  // mmap means "hey OS, I want heap memory, give me a bunch and tell me where
  // it started
  uint64_t* heap = mmap(NULL,
                        HEAP_SIZE,
                        PROT_READ | PROT_WRITE,
                        MAP_ANON | MAP_SHARED,
                        -1,
                        0);

  HEAP_START = heap;
  // more setup?
  *HEAP_START = HEAP_SIZE - VAL_SIZE;
}

void* my_malloc(size_t size) {
  uint64_t* current = HEAP_START;
  while(current < (HEAP_START + (HEAP_SIZE / VAL_SIZE))) {
    uint64_t cur_header = *current;
    uint64_t cur_size = (cur_header / 2) * 2;
    // want to know (a) size (b) is it free
    if((cur_header % 2 == 0) && (size <= cur_size)) { // check if free
      // GOAL: split up the block into the malloc'd part and the free part 
      // Round up size to next multiple of 8
      size_t rounded = ((size + 7) / 8) * 8;

      if(cur_size >= (rounded + 16)) {
        // what if remaining is close to 0/8/16, etc
        size_t remaining = cur_size - (rounded + VAL_SIZE);
        uint64_t* remaining_ptr = current + (rounded / VAL_SIZE) + 1;
        *remaining_ptr = remaining;
        *current = rounded + 1; // rounds up and sets the "busy" bit
                                // when splitting a block, split by
                                // the amount the user requested
      }
      else {
        *current += 1; // don't change size, just tag as busy!
      }

      return current + 1;
    }
    else {
      uint64_t* next = current + (cur_size / VAL_SIZE) + 1;
      current = next; 
    }
  }
  return NULL;
}

void print_heap() {
  uint64_t* current = HEAP_START;
  while(current < (HEAP_START + (HEAP_SIZE / VAL_SIZE))) {
    uint64_t cur_header = *current;
    uint64_t cur_size = (cur_header / 2) * 2;
    printf("%p\t%d\t%d\n", current, cur_header % 2, cur_size);
    uint64_t* next = current + (cur_size / VAL_SIZE) + 1;
    current = next; 
  }
  printf("\n\n");
}

void my_free(void* p) {
  uint64_t* current = p; 
  uint64_t* header = current - 1;
  if(*header % 2 == 1) { // else case: valgrind reporting double free!
    *header = *header - 1;
  }
}

int main() {
  init_heap();
  int* a = my_malloc(40);
  uint64_t* b = my_malloc(10);
  b[1] = 928347562938457;
  
  int* c = my_malloc(20);
  my_free(b);
  print_heap();
  int* d = my_malloc(30); // this got allocated at the end of the heap
  print_heap();
  int* e = my_malloc(8);  // what should we expect? (changed 12 to 8)
  print_heap();
}
