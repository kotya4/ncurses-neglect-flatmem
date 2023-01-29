#include <stdbool.h>
#include <ncurses.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <stdio.h>
#include <float.h>

enum
{
	with_no_error = 0,
	with_fmt_alloc_vsnprintf_error,
	with_fmt_alloc_realloc_error,
	with_fmt_alloc_infinit_loop_error,
	with_fmt_alloc_calloc_error,
	with_neglect_result_str_is_null_error,
	with_neglect_result_error,
	with_signal_handler_sigint,
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
	neglect_result_str = fmt_alloc("Neglected %s->%s->%i\t%s", file, func, line, msg);
	if(neglect_result_str == NULL)
	{
		exit(with_neglect_result_str_is_null_error);
		return;
	}
	exit(with_neglect_result_error);
	return;
}

// Must be called before exit. 
void before_exit()
{
	if(stdscr)
	{
		refresh();
		endwin();
		delwin(stdscr);
	}
	if(neglect_result_str)
	{
		fprintf(stdout, "%s\n", neglect_result_str);
		fflush(stdout);
		free(neglect_result_str);
	}
	fprintf(stdout, "Terminated.\n");
	fflush(stdout);
	return;
}

// Used to redefine signals behaviour.
void signal_handler(int a)
{
	if(SIGINT == a)
	{
		exit(with_signal_handler_sigint);
		return;
	}
}

// Macro for lazy.
#define for_range(V, S) for(int V=0; V<S; ++V)

#include "perlin.h"

int main()
{
	atexit(before_exit);
	signal(SIGINT, signal_handler);
	const WINDOW *const w = initscr();
	neglect(w == NULL);
	neglect(stdscr != w);
	int rows = getmaxx(stdscr);
	int cols = getmaxy(stdscr);
	noecho();
	keypad(stdscr, TRUE); 	// allow non-character keys
	curs_set(FALSE); 		// hide cursor
	start_color();
	neglect(has_colors() == FALSE);

	clear();

	const int colorspace = 20;
	for_range(i, colorspace)
	{
		init_pair((short)1 + i, (short)0xca, (short)0xff - colorspace + i);
	}

	const float delta = 10;
	int shiftx = 0;
	int shifty = 0;
	int dshiftx = 1;
	int dshifty = 0;

	while(true)
	{
		float pmin=+FLT_MAX, pmax=-FLT_MAX;
		for(int y=0; y<cols; ++y)
		{
			for(int x=0; x<rows / 2; ++x)
			{
				float p = perlin((float)(x + shiftx) / delta, (float)(y + shifty) / delta);
				if(pmin>p)pmin=p;
				if(pmax<p)pmax=p;
				
				short pi = (short)((p * .5f + .5f) * colorspace) + 1;
				attrset(COLOR_PAIR(pi));
				mvprintw(y, x * 2, "  ");
			}
		}
		mvprintw(0, 0, " %1.0e * %i ", 1.f / delta, shiftx);
		mvprintw(1, 0, " %1.0e * %i ", 1.f / delta, shifty);
		mvprintw(2, 0, " perlin max %f ", pmax);
		mvprintw(3, 0, " perlin min %f ", pmin);
		refresh();
		napms(100);
		if((dshiftx && abs(shiftx) % (rows / 2) == 0) || (shifty && abs(shifty) % cols == 0))
		{
			dshiftx = 0;
			dshifty = 0;			
			if(rand() & 1)
			{
				dshiftx = rand() & 1 ? -1 : +1;
			}
			else
			{
				dshifty = rand() & 1 ? -1 : +1;
			}
		}
		shiftx += dshiftx;
		shifty += dshifty;
	}

	return with_no_error;
}
