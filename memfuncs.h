#ifndef MEMFUNCS_H
#define MEMFUNCS_H

#include <pthread.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct {
	size_t size;
	bool is_free;
} MemBlock;

#endif
