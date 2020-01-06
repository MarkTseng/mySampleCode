/*******************************************
* File Name : scrolling_form.c
* Purpose :
* Creation Date : 30-08-2016
* Last Modified : Tue 30 Aug 2016 10:21:06 AM CST
* Created By : Mark Tseng  
**********************************************/

#include <stdio.h>  
#include <unistd.h>  
#include <sys/mman.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

/*
 * This is a simple example of "scrolling" form with ncurses.
 * It use "page" to allow forms with more fields than your window can print.
 *
 * It prints a "label" (inactive field) and a regular field and let you
 * "scroll" pages of the form.
 *
 * How to compile:
 *	gcc -o test scrolling_form.c -lform -lncurses
 */

#include <form.h> // this includes <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

WINDOW *wbody, *inner;
FIELD **field;
FORM *form;

int nb_field; // how many fields you want (modify it in main())
int cur_y = 1, cur_x = 1; // current y and x for the inner window
char str_label[7], str_field[6];
char cur_ch_label = 'A', cur_ch_field = 'a';
int lines = 21, cur_page = 0;

/*
+----------------------------------+ <-- wbody
|+--------------------------------+ <-- inner
||  ^                             ||
||  |                             ||
||  |                             ||
||  |                             ||
||  | lines                       ||
||  |                             ||
||  |                             ||
||  |                             ||
||  v                             ||
|+--------------------------------+|
+----------------------------------+
*/

// We free everything here
void quit() {
	int i;

	unpost_form(form);

	for (i = 0; i < nb_field; i++) {
		free(field_buffer(field[i], 0));
		free_field(field[i]);
	}

	free_form(form);
	delwin(wbody);
	delwin(inner);
}

char* get_next_str_field() {
	int i;

	if (str_field[0] != '\0')
		cur_ch_field++;
	
	for (i = 0; i < 4; i++)
		str_field[i] = cur_ch_field;

	str_field[5] = '\0';

	return str_field;
}

char* get_next_str_label() {
	int i;

	if (str_label[0] != '\0')
		cur_ch_label++;
	
	for (i = 0; i < 5; i++)
		str_label[i] = cur_ch_label;

	str_label[5] = ':';
	str_label[6] = '\0';

	return str_label;
}

/*
 * Create nb_field fields, one as a label, one as an active field e.g.:
 *	[label]:	[field]
 */
void create_nb_fields() {
	int i;

	field = calloc(nb_field, sizeof(FIELD *));

	if (!field) // No memory for you !
		assert(1);

	for (i = 0; i < nb_field-1; i++, cur_y += 2) {
		// label
		field[i] = new_field(1, 20, cur_y, cur_x, 0, 0);
		field_opts_on(field[i], O_VISIBLE);
		field_opts_off(field[i], O_EDIT);
		field_opts_off(field[i], O_ACTIVE);
		set_field_buffer(field[i], 0, get_next_str_label());

		i++;

		/*
		 * This is a nasty tric:
		 * I check if we wrote at the tail of the inner window,
		 * if so, I move the field where it belongs (head of
		 * the inner window).
		 */
		if (cur_y % lines == 0) {
			cur_y = 1;
			set_new_page(field[i-1], TRUE);
			move_field(field[i-1], cur_y, cur_x);
		}

		// field
		field[i] = new_field(1, 20, cur_y, cur_x + 10, 0, 0);
		set_field_back(field[i], A_UNDERLINE);
		field_opts_on(field[i], O_VISIBLE);
		field_opts_on(field[i], O_ACTIVE);
		field_opts_off(field[i], O_EDIT);
		set_field_buffer(field[i], 0, get_next_str_field());
	}

	field[nb_field-1] = NULL;
}

int main() {
	int ch;

	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	// This is required to init get_next_str_XXXX
	str_label[0] = '\0';
	str_field[0] = '\0';

	wbody = newwin(lines + 2, 44, 1, 1);
	box(wbody, 0, 0);
	keypad(wbody, TRUE);

	nb_field = 43; // this is too much fields for the window
	create_nb_fields();
	form = new_form(field);

	// Check the doc here:
	// http://www.tldp.org/HOWTO/NCURSES-Programming-HOWTO/forms.html#FORMWINDOWS
	set_form_win(form, wbody);
	inner = derwin(wbody, lines, 42, 1 ,1);
	set_form_sub(form, inner);

	post_form(form);

	mvprintw(LINES - 4, 0, "Use F1 to quit");
	mvprintw(LINES - 3, 0, "Use PAGE_UP, PAGE_DOWN to switch between pages");
	mvprintw(LINES - 2, 0, "Use UP, DOWN arrow keys to switch between fields");
	refresh();

	while ((ch = wgetch(wbody)) != KEY_F(1)) {
		switch (ch) {
			case KEY_DOWN:
				form_driver(form, REQ_NEXT_FIELD);
				form_driver(form, REQ_END_LINE);
				break;

			case KEY_UP:
				form_driver(form, REQ_PREV_FIELD);
				form_driver(form, REQ_END_LINE);
				break;
			
			/*
			 * This is where you want to look to see how
			 * to "scroll" to the next/previous page.
			 */
			case KEY_NPAGE:
				form_driver(form, REQ_NEXT_PAGE);
				set_form_page(form, ++cur_page);
				break;

			case KEY_PPAGE:
				form_driver(form, REQ_PREV_PAGE);
				set_form_page(form, --cur_page);
				break;
		}
	}

	quit();
	endwin();
	return 0;
}

