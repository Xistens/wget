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

/**
 * A function to display an error message and then exit.
 */
void fatal(char *message) {
    char error_message[100] = {0};

    strcpy(error_message, "[!!] Fatal Error: ");
    strncat(error_message, message, 83);
    
    // Print a system error message
    perror(error_message);
    exit(-1);
}

void hexdump(const unsigned char *buffer, const unsigned int length) {
    unsigned char byte;
    unsigned int i, j;

    for (i = 0; i < length; i++) {
        byte = buffer[i];
        // Display byte in hex
        printf("%02x ", buffer[i]);

        if (((i % 16) == 15) || (i == length - 1)) {
            for (j = 0; j < 15 - (i % 16); j++)
                printf("   ");
            printf("| ");

            // Display printable bytes from line
            for (j = (i - (i % 16)); j <= i; j++) {
                byte = buffer[j];

                if ((byte > 31) && (byte < 127))
                    printf("%c", byte);
                else
                    printf(".");
            }
            // End of the dump line (each line is 16 bytes)
            printf("\n");
        }
    }
}