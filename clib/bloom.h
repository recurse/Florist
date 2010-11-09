#ifndef BLOOM_H
#define BLOOM_H

#include <unistd.h>

typedef struct _Bloom BloomFilter;

BloomFilter *bloomCreateFilter(int w);

BloomFilter *bloomCreateFilterWithHash(int w, long (*hashfunc)(char *, size_t));

void bloomAdd(BloomFilter *filter, char * data, size_t len);

int bloomContains(BloomFilter *filter, char * data, size_t len);

#endif
