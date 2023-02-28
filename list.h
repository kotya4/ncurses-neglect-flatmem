#pragma once
#define list_version 20221202

#include <stdlib.h>

// Macro injectable in structure becoming list
// T is type of members
#define list(T)	\
	int cap;	\
	int size;	\
	T *a;		\
	int len;	\
	int acc;	\

// Allocates memory for type T with accumulator accum
#define list_new(T, accum, ...) {		\
	.cap = accum,						\
	.size = sizeof(T),					\
	.a = (T *)calloc(accum, sizeof(T)),	\
	.len = 0,							\
	.acc = accum,						\
	__VA_ARGS__ }						\

// Makes list L null
#define list_kill(L) {				\
	if((L).a)						\
		free((L).a);				\
	(L).cap = NULL;					\
	(L).cap = (L).len = 0; }		\

// Checks if index is suitable for list L, resizes list if not
#define list_expand(L, index) {											\
	if(index >= (L).cap) {												\
		const int acc = max((L).cap - index + 1, (L).acc);				\
		(L).a = realloc((L).a, ((L).cap += acc) * (L).size); } }		\

// Returns next member of list L ready to be value pushed in
#define list_next(L)			\
	list_expand(L, (L).len);	\
	(L).a[(L).len++]			\

// Puts value into list L at index I, resizes list if
// index out of range, created at resize members are
// not initialized
#define list_put(L, I)			\
	list_expand(L, I);			\
	(L).a[I]					\

// Sets values of list as not initialized
#define list_null(...) {	\
	.cap = 0,				\
	.size = 0,				\
	.a = NULL,				\
	.len = 0,				\
	.acc = 0,				\
	__VA_ARGS__ }			\

// Checks if list L is not initialized with list_new
#define list_is_null(L)		\
	(L).a == NULL			\

// Inserts list B into list A starting at index _offset.
#define list_insert(A, B, _offset) {			\
	(A).len += (B).len;							\
	list_expand(A, (A).len-1);					\
	for(int _i=0; _i<(A).len-(_offset); ++_i) {	\
		const int _k=(A).len-1-_i;				\
		if(_k >= 0 && _k < (A).len)				\
			(A).a[_k]=(A).a[_k-(B).len]; }		\
	for(int _i=0; _i<(B).len; ++_i) {			\
		const int _k=_i+(_offset)<(A).len;		\
		if(_k >= 0 && _k < (A).len)				\
			(A).a[_i+(_offset)]=(B).a[_i]; } }	\

// Returns next member of list L ready to be value pushed in,
// nut unlike list_next macro, allocates list if it is null.
// Lt is type of list, T is type of members, A is accumulator.
#define list_next_safe(L, Lt, T, A, ...)			\
	if(list_is_null(L)) {							\
		L=(Lt)list_new(T, A, __VA_ARGS__); }		\
	list_next(L)									\
