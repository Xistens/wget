#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"


/**
 * Error-checked malloc()
 */
void *c_malloc(unsigned int size){
  void *ptr = malloc(size);
  if (ptr == NULL) {
    fprintf(stderr, "Error: could not allocate heap memory.\n");
    exit(-1);
  }
  return ptr;
}


void fatal(char *message) {
    char error_message[100] = {0};

    strncpy(error_message, "[!!] Fatal Error ", strlen(error_message));
    strncat(error_message, message, 83);
    
    // Print a system error message
    perror(error_message);
    exit(-1);
}