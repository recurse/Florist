#include "bloom.h"

#include <stdio.h>
#include <stdlib.h>

struct _Bloom {
	long (*hashfunc)(char *, size_t);
	size_t size;
	char *array;
};

static long defaultHash(char *data, size_t len);

BloomFilter *bloomCreateFilter(int w) {
	return bloomCreateFilterWithHash(w, defaultHash);
}

BloomFilter *bloomCreateFilterWithHash(int w, long (*hashfunc)(char *, size_t len)) {
	BloomFilter *filter;

	if (w & 0x0007) {
		perror("BloomFilter Invalid width not multiple of 8");
		return NULL;
	} else if ((w | 0x0007) == 0x0007) {
		perror("BloomFilter Invalid width not greater than 8");
		return NULL;
	}

	filter = (BloomFilter *)malloc(sizeof(BloomFilter));
	if (filter == NULL) {
		perror("Allocating BloomFilter");
		return NULL;
	}

	filter->hashfunc = hashfunc;
	filter->size = w >> 8;
	filter->array = (char *)malloc(filter->size);
	if (filter->array == NULL) {
		perror("Allocating BloomFilter array");
		free(filter);
		return NULL;
	}

	return filter;
}

// Note: As a shortcut we use a k of 8.
void bloomAdd(BloomFilter *filter, char * data, size_t len) {
	long hash;

	hash = filter->hashfunc(data, len);

	filter->array[(hash >> 8) % filter->size] |= (hash & 0x00FF);
}

int bloomContains(BloomFilter *filter, char * data, size_t len) {
	long hash;

	hash = filter->hashfunc(data, len);

	return filter->array[(hash >> 8) % filter->size] == (hash & 0x00FF);
}

static long defaultHash(char *data, size_t len) {
	char *p;
	long hash; 
	int shift;

	// Loop unrolling by increments of sizeof(long).
	for (p = data; p - data < len - sizeof(long); p += sizeof(long)) {
		for (shift = 0; shift < sizeof(long); shift++) {
			hash ^= (*p++ << (shift * 8));
		}
	}

	// Cleanup leftovers.
	shift = 0;
	while (p - data < len) {
		hash ^= (*p++ << (shift * 8));
		shift++;
	}
}
