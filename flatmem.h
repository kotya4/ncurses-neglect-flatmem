#pragma once

#include <stdlib.h>
#include "neglect.h"

static size_t flatmemcap = 0;
static size_t flatmemlen = 0;
static void **flatmem = NULL;
static void *flatmem_buf = NULL;
// Flat memory, will be freed at exit
#define flatmem_new(TYPE, SIZE, VALUE)									\
	flatmem_buf = (TYPE *)calloc(SIZE, sizeof(TYPE));					\
	neglect(!flatmem_buf);												\
	if(flatmemcap <= flatmemlen + 1)									\
	{																	\
		flatmemcap += 0x8;												\
		void *ptr = realloc(flatmem, flatmemcap * sizeof(void *));		\
		neglect(!ptr);													\
		flatmem = (void **)ptr;											\
	}																	\
	flatmem[flatmemlen++] = flatmem_buf;								\
	for(size_t i=0; i<SIZE; ++i) ((TYPE *)flatmem_buf)[i] = VALUE		\

