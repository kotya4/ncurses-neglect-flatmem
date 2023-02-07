#pragma once
#include "neglect.h"

// Returns length of a with \0
int str_cat(char *a, const char *b)
{
	neglect(!a || !b);
	size_t i;
	for(i=0;a[i];++i);
	for(;*b;++b)a[i++]=*b;
	a[i]=0;
	return i + 1;
}

// Returns 1 on success
int str_cmp(const char *a, const char *b)
{
	neglect(!a || !b);
	for(int i=0; *a!=0&&*b!=0; ++i)
		if(*a++!=*b++)
			return 0;
	return (*a==0&&*b==0);
}

// Searches b in a, returns -1 if not found
int str_index(const char *a, const char *b)
{
	neglect(!a || !b);
	const char *sb = b;
	int i = 0;
	while(*a)
	{
		for(b=sb;*a&&*a!=*b;++a,++i);
		for(;*a&&*b&&*a==*b;++a,++b);
		if(!*b)
			return i;
		++i;
	}
	return -1;
}

// Returns length of cstr w\o \0
int str_len(const char *src)
{
	neglect(!src);
	int len = 0;
	for(; src[len]; len++);
	return len;
}

// Returns number of copied values (without 0)
// a - destination,
// b - source,
// n - explicit number of values to copy (can be infinite)
int str_copy(char *a, const char *b, int n)
{
	int i;
	for(i=0; b[i] && i < n; ++i)a[i] = b[i];
	//a[i] = 0;
	return i;
}

int str_to_int(const char *s)
{
	neglect(!s);
	int v=0;
	int cbase = 1;
	const int base = 10;
	for(int i=0; s[i]; ++i)
		cbase *= base;
	for(int i=0; *s; ++i,s++)
	{
		cbase /= base;
		v += (*s - '0') * cbase;
	}
	return v;
}

/*
#include <stdlib.h>

char *str_dup(const char *src)
{
	neglect(!src);
	int len = 0;
	for(; src[len]; len++);
	char *out = (char *)calloc((size_t)(len+1), sizeof(char));
	for(int i=0; *src; src++, i++)
		out[i] = *src;
	return out;
}
*/