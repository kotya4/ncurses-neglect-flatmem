#include <stdbool.h>
#include <ncurses.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <locale.h>
#include <stdio.h>
#include <float.h>
#include "neglect.h"

static size_t allocatedcap = 0;
static size_t allocatedlen = 0;
static void **allocated = NULL;
static void *allocatebuffer = NULL;
#define allocate(SIZE, TYPE, VALUE)											\
	allocatebuffer = (TYPE *)calloc(SIZE, sizeof(TYPE));					\
	neglect(!allocatebuffer);												\
	if(allocatedcap <= allocatedlen + 1)									\
	{																		\
		allocatedcap += 0x8;												\
		void *ptr = realloc(allocated, allocatedcap * sizeof(void *));		\
		neglect(!ptr);														\
		allocated = (void **)ptr;											\
	}																		\
	allocated[allocatedlen++] = allocatebuffer;								\
	for(size_t i=0; i<SIZE; ++i) ((TYPE *)allocatebuffer)[i] = VALUE		\

// Must be called before exit.
void before_exit()
{
	if(stdscr)
	{
		refresh();
		endwin();
		delwin(stdscr);
	}
	if(allocated)
	{
		for(int i=0; i<allocatedlen; ++i) if(allocated[i]) free(allocated[i]);
		free(allocated);
		allocated = NULL;
	}
	if(neglect_result_str)
	{
		fprintf(stdout, "%s\n", neglect_result_str);
		fflush(stdout);
		free(neglect_result_str);
	}
	fprintf(stdout, "%lu block(s) deallocated\n", allocatedlen);
	fflush(stdout);
	return;
}

// Used to redefine signals behaviour.
void signal_handler(int a)
{
	neglect(SIGINT == a);
}

#include "str.h"

// Reads sequence of bytes from stdin and returns wide-character code, also writes bytes into str.
uint32_t getch_wide(uint8_t *str)
{
	// TIP: str must be unsigned elsewise returning keycode will be negative
	int ch = getch();
	str[0] = str[1] = str[2] = str[3] = 0;
	static const char *const unknown = "�";
	if(ch >= 0x100 || ch == 0x1b)
	{
		str_copy((char *)str, unknown, 4);
		return (uint32_t)ch;
	}
	if((ch & (int)128) == (int)  0) return  (uint32_t)(str[0] = (uint8_t)ch);
	if((ch & (int)224) == (int)192) return ((uint32_t)(str[0] = (uint8_t)ch) <<  8) |  (uint32_t)(str[1] = (uint8_t)getch());
	if((ch & (int)240) == (int)224) return ((uint32_t)(str[0] = (uint8_t)ch) << 16) | ((uint32_t)(str[1] = (uint8_t)getch()) <<  8) |  (uint32_t)(str[2] = (uint8_t)getch());
	if((ch & (int)248) == (int)240) return ((uint32_t)(str[0] = (uint8_t)ch) << 24) | ((uint32_t)(str[1] = (uint8_t)getch()) << 16) | ((uint32_t)(str[2] = (uint8_t)getch()) << 8) | (uint32_t)(str[3] = (uint8_t)getch());
	str_copy((char *)str, unknown, 4);
	return (uint32_t)ch;
}

int main()
{
	setlocale(LC_ALL, "");
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
	short pairlen = 0;

	init_pair((short)(1 + pairlen++), (short)(0xff), (short)(0xff-18+0));
	init_pair((short)(1 + pairlen++), (short)(0xfa), (short)(0xff-18+1));

	short cury = 0;
	short curx = 0;
	size_t screenlen = rows * cols;
	short *screen = allocate(screenlen, short, 1);
	clear();
	move(0, 0);
	for(int i=0; i<screenlen;++i)
	{
		attrset(COLOR_PAIR(screen[i]));
		printw(" ");
	}
	while(1)
	{
		short cpair = pairlen;
		uint8_t chr[4] = { 0, 0, 0, 0 };
		uint32_t chrcode = getch_wide(chr);
		(void)chrcode;
		attrset(COLOR_PAIR(cpair--));cpair=cpair>1?cpair:pairlen;
		mvprintw(cury, curx++, "%s", chr);
	}
	refresh();
	napms(0);
	return with_no_error;
}
