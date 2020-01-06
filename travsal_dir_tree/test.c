/*******************************************
* File Name : test.c
* Purpose :
* Creation Date : 29-08-2016
* Last Modified : Tue 30 Aug 2016 05:20:01 PM CST
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
#include <ncurses/ncurses.h>			/* ncurses.h includes stdio.h */  
#include <ncurses/panel.h>

#define CTRL(c) ((c) & 037)

/* Global value of current cursor position */
int row, col;

int main(int argc,char **argv)
{
   extern void perror(), exit();
   int i, n, l;
   int c;
   int line = 0;
   FILE *fd;

   if (argc != 2)
   {
       fprintf(stderr, "Usage: %s file\n", argv[0]);
       exit(1);
   }

   fd = fopen(argv[1], "r");
   if (fd == NULL)
   {
       perror(argv[1]);
       exit(2);
   }

   initscr();
   cbreak();
   nonl();
   noecho();
   idlok(stdscr, TRUE);
   keypad(stdscr, TRUE);

   /* Read in the file */
   while ((c = getc(fd)) != EOF)
   {
       if (c == '\n')
           line++;
       if (line > LINES - 2)
           break;
       addch(c);
   }
   fclose(fd);

   move(0,0);
   refresh();
   edit();

   /* Write out the file */
   fd = fopen(argv[1], "w");
   for (l = 0; l < LINES - 1; l++)
   {
       n = len(l);
       for (i = 0; i < n; i++)
           putc(mvinch(l, i) & A_CHARTEXT, fd);
       putc('\n', fd);
   }
   fclose(fd);

   endwin();
   return 0;
}

len(int lineno)
{
   int linelen = COLS - 1;

   while (linelen >= 0 && mvinch(lineno, linelen) == ' ')
       linelen--;
   return linelen + 1;
}
edit()
{
   int c;

   while(1)
   {
       move(row, col);
       refresh();
       c = getch();

       /* Editor commands */
       switch (c)
       {

       /* hjkl and arrow keys: move cursor
        * in direction indicated */
       case 'h':
       case KEY_LEFT:
           if (col > 0)
               col--;
           else
               flash();
           break;

       case 'j':
       case KEY_DOWN:
           if (row < LINES - 1)
               row++;
           else
               flash();
           break;

       case 'k':
       case KEY_UP:
           if (row > 0)
               row--;
           else
               flash();
           break;

       case 'l':
       case KEY_RIGHT:
           if (col < COLS - 1)
               col++;
           else
               flash();
           break;
       /* i: enter input mode */
       case KEY_IC:
       case 'i':
           input();
           break;

       /* x: delete current character */
       case KEY_DC:
       case 'x':
           delch();
           break;

       /* o: open up a new line and enter input mode */
       case KEY_IL:
       case 'o':
           move(++row, col = 0);
           insertln();
           input();
           break;

       /* d: delete current line */
       case KEY_DL:
       case 'd':
           deleteln();
           break;

       /* ^L: redraw screen */
       case KEY_CLEAR:
       case CTRL('L'):
           wrefresh(curscr);
           break;

       /* w: write and quit */
       case 'w':
           return;
       /* q: quit without writing */
       case 'q':
           endwin();
           exit(2);
       default:
           flash();
           break;
       }
   }
}

/*
* Insert mode: accept characters and insert them.
*  End with ^D or EIC
*/
input()
{
   int c;

   standout();
   mvaddstr(LINES - 1, COLS - 20, "INPUT MODE");
   standend();
   move(row, col);
   refresh();
   for (;;)
   {
       c = getch();
       if (c == CTRL('D') || c == KEY_EIC)
           break;
       insch(c);
       move(row, ++col);
       refresh();
   }
   move(LINES - 1, COLS - 20);
   clrtoeol();
   move(row, col);
   refresh();
}

