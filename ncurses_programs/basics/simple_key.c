#include <stdio.h>
#include <ncurses.h>

#define WIDTH 80
#define HEIGHT 20 

int startx = 0;
int starty = 0;

char *choices[] = { 
			"Choice 1",
			"Choice 2",
			"Choice 3",
			"Choice 4",
			"Choice 5",
			"Choice 6",
			"Choice 7",
			"Choice 8",
			"Choice 9",
			"Choice 10",
			"Choice 11",
			"Choice 12",
			"Choice 13",
			"Choice 14",
			"Choice 15",
			"Choice 16",
			"Choice 17",
			"Choice 18",
			"Choice 19",
			"Choice 20",
			"Choice 21",
			"Choice 22",
			"Choice 23",
			"Choice 24",
			"Choice 25",
			"Choice 26",
			"Choice 27",
			"Choice 28",
			"Choice 29",
			"Choice 30",
			"Choice 31",
			"Choice 32",
			"Choice 33",
			"Choice 34",
			"Choice 35",
			"Choice 36",
			"Choice 37",
			"Choice 38",
			"Choice 39",
			"Choice 40",
			"Choice 41",
			"Choice 42",
			"Choice 43",
			"Choice 44",
			"Choice 45",
			"Choice 46",
			"Choice 47",
			"Choice 48",
			"Choice 49",
			"Choice 50",
			"Choice 51",
			"Choice 52",
			"Choice 53",
			"Choice 54",
			"Choice 55",
			"Choice 56",
			"Choice 57",
			"Choice 58",
			"Choice 59",
			"Choice 60",
			"Choice 61",
			"Choice 62",
			"Choice 63",
			"Exit",
		  };
int n_choices = sizeof(choices) / sizeof(char *);
void print_menu(WINDOW *menu_win, int highlight);

int main()
{	WINDOW *menu_win;
	int highlight = 1;
	int choice = 0;
	int c;

	initscr();
	clear();
	noecho();
	cbreak();	/* Line buffering disabled. pass on everything */
	startx = (80 - WIDTH) / 2;
	starty = (24 - HEIGHT) / 2;
	//scrollok(menu_win,TRUE);
		
	menu_win = newwin(n_choices + 3, WIDTH, starty, startx);
	keypad(menu_win, TRUE);
	mvprintw(0, 0, "Use arrow keys to go up and down, Press enter to select a choice");
	refresh();
	print_menu(menu_win, 0);
	while(1)
	{	c = wgetch(menu_win);
		switch(c)
		{	case KEY_UP:
				if(highlight == 1)
					highlight = n_choices;
				else
					--highlight;
				break;
			case KEY_DOWN:
				if(highlight == n_choices)
					highlight = 1;
				else 
					++highlight;
				break;
			case 10:
				choice = highlight;
				break;
			default:
				mvprintw(24, 0, "Charcter pressed is = %3d Hopefully it can be printed as '%c'", c, c);
				refresh();
				break;
		}
		print_menu(menu_win, highlight);
		if(choice != 0)	/* User did a choice come out of the infinite loop */
			break;
	}	
	mvprintw(23, 0, "You chose choice %d with choice string %s\n", choice, choices[choice - 1]);
	clrtoeol();
	refresh();
	endwin();
	return 0;
}


void print_menu(WINDOW *menu_win, int highlight)
{
	int x, y, i;	

	x = 2;
	y = 2;
	box(menu_win, 0, 0);
	for(i = highlight - 1; i < n_choices; ++i)
	{	
		if(highlight == i + 1) /* High light the present choice */
		{	wattron(menu_win, A_REVERSE); 
			mvwprintw(menu_win, y, x, "%s", choices[i]);
			wattroff(menu_win, A_REVERSE);
		}
		else
			mvwprintw(menu_win, y, x, "%s", choices[i]);
		++y;
	}
	wrefresh(menu_win);
}

