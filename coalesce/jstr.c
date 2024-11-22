#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "mem.c"

struct String {
  uint64_t length; // equals strlen(contents)
  char* contents;  // has space for length + null term
};

typedef struct String String;

String new_String(char* init_contents) {
  uint64_t size = strlen(init_contents);
  char* contents = my_malloc(size + 1);
  contents[size] = 0;
  strcpy(contents, init_contents);
  String r = { size, contents };
  return r;
}

String plus(String s1, String s2) {
  uint64_t new_size = s1.length + s2.length + 1;
  char* new_contents = my_malloc(new_size);
  strncpy(new_contents, s1.contents, s1.length);
  strncpy(new_contents + s1.length, s2.contents, s2.length);
  new_contents[new_size - 1] = 0;
  String r = { new_size - 1, new_contents };
  return r;
}

String joinfree(String strs[], int count, String delimiter) {
    String s = new_String("");
    for(int i = 0; i < count; i += 1) {
        char* before_plus = s.contents;
        s = plus(s, strs[i]);
        my_free(before_plus);
        if(i < count - 1) {
            char* before_plus = s.contents;
            s = plus(s, delimiter);
            my_free(before_plus);
        }
    }
    return s;
}

int main() {
  init_heap();
  String s = new_String("hello");
  printf("%s\n", s.contents);

  String s2 = new_String("cse29");
  print_heap();

  String hello_cse = plus(s, s2);
  print_heap();
  String hello_bang = plus(s, new_String("!!!!"));

  print_heap();

  printf("%s\n", hello_cse.contents);
  printf("%s\n", hello_bang.contents);

  String s3 = new_String("hello everyone!");
  print_heap();

  String strs[] = { s, s2, hello_cse, hello_bang };
  String space = new_String(" ");
  print_heap();
  String joined = joinfree(strs, 4, space);
  print_heap();
  printf("Joined: %s\n", joined.contents);

  my_free(joined.contents);
  print_heap();

  int* test = my_malloc(8); // what will the resulting heap look like?
  print_heap();

  return 0;
}
