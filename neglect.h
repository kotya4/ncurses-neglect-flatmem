#pragma once
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

enum
{
	with_no_error = 0,
	with_fmt_alloc_vsnprintf_error,
	with_fmt_alloc_realloc_error,
	with_fmt_alloc_infinit_loop_error,
	with_fmt_alloc_calloc_error,
	with_neglect_result_str_is_null_error,
	with_neglect_result_error,
};

// Allocates memory for string, formats string with vsnprintf, exits with error on failure, returns allocated string at success. If formatted string fits 32 bytes then string will be 32 bytes netherless formatted string real size.
char *fmt_alloc(const char *fmt, ...)
{
	// Tip: this function used neglect_result, so cannot send verbose error mesages to stdout, because only neglect_result can safely use stdout.
	size_t string_length = 32;
	char *string = (char *)calloc(string_length, sizeof(char));
	if(string == NULL)
	{
		exit(with_fmt_alloc_calloc_error);
		return NULL;
	}
	for(int loop_counter = 2; loop_counter > 0; --loop_counter)
	{
		// Trying to fit fmt into string.
		va_list a;
		va_start(a, fmt);
		int real_string_length_wo_0 = vsnprintf(string, string_length, fmt, a);
		va_end(a);
		if(real_string_length_wo_0 <= 0)
		{
			// vsnprintf returns incompatible real_string_length_wo_0
			free(string);
			exit(with_fmt_alloc_vsnprintf_error);
			return NULL;
		}
		if(real_string_length_wo_0 < string_length)
		{
			// Ok.
			return string;
		}
		else
		{
			// Cannot fit fmt, must expand string capacity.
			string_length = real_string_length_wo_0 + 1;
			char *new_string = (char *)realloc(string, string_length * sizeof(char));
			if(new_string == NULL)
			{
				// realloc not work
				free(string);
				exit(with_fmt_alloc_realloc_error);
				return NULL;
			}
			string = new_string;
		}
		// Now string must fit fmt, if string cannot fit second time, then loop become infinit at some point, so must prevent it.
	}
	// Loop became infinit at some point, do not know why.
	free(string);
	exit(with_fmt_alloc_infinit_loop_error);
	return NULL;
}

// Checks if STATE is true, if so, exits with error.
#define neglect(STATE) if(STATE) neglect_result(#STATE)
char *neglect_result_str = NULL;
#define neglect_result(C_STR) neglect_result_(__FILE__, __func__, __LINE__, C_STR)
void neglect_result_(const char *file, const char *func, const int line, const char *msg)
{
	neglect_result_str = fmt_alloc("NEGLECTED AT %s:%s:%i WITH %s", file, func, line, msg);
	if(neglect_result_str == NULL)
	{
		exit(with_neglect_result_str_is_null_error);
		return;
	}
	exit(with_neglect_result_error);
	return;
}
