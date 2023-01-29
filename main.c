#include <stdbool.h>
#include <ncurses.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <stdio.h>

char *fmt_alloc(const char *fmt, ...)
{
	// Tip: this function used inside "internal_error" function, so cannot send verbose error mesages.
	size_t string_length = 32;
	char *string = (char *)calloc(string_length, sizeof(char));
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
			return NULL;
		}
		if(real_string_length_wo_0 + 1 > string_length)
		{
			// Cannot fit fmt, must expand string capacity.
			string_length = real_string_length_wo_0 + 1;
			char *new_string = (char *)realloc(string, string_length * sizeof(char));
			if(new_string == NULL)
			{
				// realloc not work
				free(string);
				return NULL;
			}
			string = new_string;
		}
		else
		{
			// Ok.
			return string;
		}
		// Now string must fit fmt, if string cannot fit second time, then loop become infinite at some point, so must prevent it.
	}
	// Loop became infinite at some point, do not know why.
	free(string);
	return NULL;
}

char *internal_error_str = NULL;
#define internal_error(msg) internal_error_(__FILE__, __func__, __LINE__, msg)
void internal_error_(const char *file, const char *func, const int line, const char *msg)
{
	internal_error_str = fmt_alloc("Internal error in file '%s' function '%s' line '%i' msg '%s'", file, func, line, msg);
	if(internal_error_str == NULL)
	{
		// fmt_alloc not work, vsnprintf or calloc or realloc returns error, or loop become infinite at some point
		exit(2);
	}
	exit(1);
}

#define neglect(STATE) if(STATE) internal_error(#STATE)

void before_exit()
{
	if(stdscr)
	{
		refresh();
		endwin();
		delwin(stdscr);
	}
	if(internal_error_str)
	{
		fprintf(stdout, "%s\n", internal_error_str);
		fflush(stdout);
		free(internal_error_str);
	}
	fprintf(stdout, "ok.\n");
	fflush(stdout);
	return;
}

void signal_handler(int a)
{
	if(SIGINT == a)
	{
		exit(3);
	}
}

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
		for(int y=0; y<cols; ++y)
		{
			for(int x=0; x<rows / 2; ++x)
			{
				float p = perlin((float)(x + shiftx) / delta, (float)(y + shifty) / delta);
				short pi = (short)((p + 1.f) / 2.f * 20) + 1;
				attrset(COLOR_PAIR(pi));
				mvprintw(y, x * 2, "  ");
			}
		}
		mvprintw(0, 0, " %i ", shiftx);
		mvprintw(1, 0, " %i ", shifty);
		refresh();
		napms(100);
		if(dshiftx)
		{
			if(abs(shiftx) % (rows / 2) == 0)
			{
				napms(1000);
				if(rand() & 1)
				{
					dshiftx = rand() & 1 ? -1 : +1;
					dshifty = 0;
				}
				else
				{
					dshiftx = 0;
					dshifty = rand() & 1 ? -1 : +1;
				}
			}
			shiftx += dshiftx;
		}
		else
		{
			if(abs(shifty) % cols == 0)
			{
				napms(1000);
				if(rand() & 1)
				{
					dshiftx = rand() & 1 ? -1 : +1;
					dshifty = 0;
				}
				else
				{
					dshiftx = 0;
					dshifty = rand() & 1 ? -1 : +1;
				}
			}
			shifty += dshifty;
		}
	}

	return 0;
}
