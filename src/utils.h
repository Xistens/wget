#ifndef UTILS_H
#define UTILS_H

void *c_malloc(unsigned int size);
void fatal(char *message);
void hexdump(const unsigned char *buffer, const unsigned int length);

#endif