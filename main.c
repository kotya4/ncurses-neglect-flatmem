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
#include "flatmem.h"

// Must be called before exit.
void before_exit()
{
	if(stdscr)
	{
		refresh();
		endwin();
		delwin(stdscr);
	}
	if(flatmem)
	{
		for(int i=0; i<flatmemlen; ++i) if(flatmem[i]) free(flatmem[i]);
		free(flatmem);
		flatmem = NULL;
	}
	if(neglect_result_str)
	{
		fprintf(stdout, "%s\n", neglect_result_str);
		fflush(stdout);
		free(neglect_result_str);
	}
	fprintf(stdout, "%lu block(s) of flatmem freed\n", flatmemlen);
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
	int ch = getch();
	str[0] = str[1] = str[2] = str[3] = 0;
	static const char *const unknown = "�";
	if(ch >= 0x100 || ch == 0x1b)
	{
		str_copy((char *)str, unknown, 4);
		return (uint32_t)ch;
	}
	// TIP: str must be unsigned elsewise returning keycode will be negative
	if((ch & 128) ==   0) return  (uint32_t)(str[0] = (uint8_t)ch);
	if((ch & 224) == 192) return ((uint32_t)(str[0] = (uint8_t)ch) <<  8) |  (uint32_t)(str[1] = (uint8_t)getch());
	if((ch & 240) == 224) return ((uint32_t)(str[0] = (uint8_t)ch) << 16) | ((uint32_t)(str[1] = (uint8_t)getch()) <<  8) |  (uint32_t)(str[2] = (uint8_t)getch());
	if((ch & 248) == 240) return ((uint32_t)(str[0] = (uint8_t)ch) << 24) | ((uint32_t)(str[1] = (uint8_t)getch()) << 16) | ((uint32_t)(str[2] = (uint8_t)getch()) << 8) | (uint32_t)(str[3] = (uint8_t)getch());
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
	curs_set(TRUE);
	start_color();
	neglect(has_colors() == FALSE);
	short pairlen = 1;
	const short colorpair_bg	= pairlen;init_pair((short)(pairlen++), (short)(0xff), (short)(0xff-18+0));
	const short colorpair_text	= pairlen;init_pair((short)(pairlen++), (short)(0xfa), (short)(0xff-18+1));
	(void)colorpair_text;

	short cury = 0;
	short curx = 0;
	size_t screenlen = rows * cols;
	short *screen = flatmem_new(short, screenlen, colorpair_bg);

	size_t inputlen = 8;
	size_t inputcap = inputlen;
	char *input = flatmem_new(char, inputcap, '\0');

	const short outputy = cols / 2;

	//struct functions functions = list_new();
	clear();
	move(0, 0);
	for(int i=0; i<screenlen;++i)
	{
		attrset(COLOR_PAIR(screen[i]));
		printw(" ");
	}
	while(1)
	{
		uint32_t chrcode = 0;
		#define not_whitespace(a) (a != ' ' && a != '\n' && a != '\r' && a != '\t')
		while(1)
		{
			short cpair = pairlen;
			char chr[5] = { 0, 0, 0, 0, 0 }; // use 4 if do not want to use as str
			chrcode = getch_wide((uint8_t *)chr);
			if(!not_whitespace(chrcode)) break;

			attrset(COLOR_PAIR(--cpair));cpair=cpair>colorpair_bg?cpair:pairlen; // every color descending but colorpair_bg
			mvprintw(cury, curx++, "%s", chr);

			if(inputlen + 4 > inputcap)
			{
				inputcap += 0x8;
				void *ptr = realloc(input, inputcap * sizeof(char));
				neglect(ptr == NULL);
				input = (char *)ptr;
			}
			inputlen = str_cat(input, chr);

			move(outputy, 0);
			for(int i=0;i<5;++i)printw("%i ", chr[i]);
			printw("      ");
			printw("%s (%lu, %lu)    ", input, inputlen, inputcap);
		}

		neglect(str_cmp(input, "ришат"));
		neglect(str_cmp(input, "exit"));

		move(outputy+1, 0);
		printw("checking %s (%lu, %lu)    ", input, inputlen, inputcap);

		input[inputlen=0]='\0';
		cury++;
		curx=0;
	}
	refresh();
	napms(0);
	return with_no_error;
}
