#ifndef UTILS_H
#define UTILS_H

#include "http.h"

void *c_malloc(unsigned int size);
void hexdump(const unsigned char *buffer, const unsigned int length);

#endif