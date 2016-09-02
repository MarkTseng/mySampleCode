#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ncurses/ncurses.h>

int count = 0;
int x = 0;

int readAttribute(char const *path, char * value)
{
	FILE *fd;
	
	fd = fopen(path,"rb");
	if(fd == NULL)
	{
		perror("open file fail\n");
		return -1;
	}
	fread(value,1,32,fd);
	fclose(fd);
	return 0;
}

static int
display_info(const char *fpath, const struct stat *sb,
             int tflag, struct FTW *ftwbuf)
{
	char value[32];
	int length = 0;
#if 0
    printf("%-3s %2d %7jd   %-40s %d %s\n",
        (tflag == FTW_D) ?   "d"   : (tflag == FTW_DNR) ? "dnr" :
        (tflag == FTW_DP) ?  "dp"  : (tflag == FTW_F) ?   "f" :
        (tflag == FTW_NS) ?  "ns"  : (tflag == FTW_SL) ?  "sl" :
        (tflag == FTW_SLN) ? "sln" : "???",
        ftwbuf->level, (intmax_t) sb->st_size,
        fpath, ftwbuf->base, fpath + ftwbuf->base);
#endif

	if((tflag == FTW_F)&&(strstr(fpath,".mmap")==NULL)) {
		memset(value, 0, 32);
		readAttribute(fpath, value);
		//mvprintw(count,0,"%d - %-40s : %s",count, fpath,value);	/* Print Hello World		  */
		printw("%d - %-40s : %s",count, fpath,value);	/* Print Hello World		  */
		count++;
	}
    return 0;           /* To tell nftw() to continue */
}

int
main(int argc, char *argv[])
{
    int flags = 0;
	int ch;
	int row,col;
#if 0
    int i = 0;

    initscr();

	scrollok(stdscr,TRUE);
    while(1)
    {
        printw("%d - lots and lots of lines flowing down the terminal\n", i);
        ++i;
        refresh();
		
		if(i%40==0) {
			scrl(40);
			//move(0,0);
		}
		//usleep(100000);
    }

    endwin();
    return 0;
#endif
   if (argc > 2 && strchr(argv[2], 'd') != NULL)
        flags |= FTW_DEPTH;
    if (argc > 2 && strchr(argv[2], 'p') != NULL)
        flags |= FTW_PHYS;

	initscr();			/* Start curses mode 		  */
	scrollok(stdscr,TRUE);
	scroll (stdscr);
	cbreak();			/* Line buffering disabled, Pass on everty thing to me 		*/
	noecho();
	keypad(stdscr, TRUE);		/* I need that nifty F1 	*/
	timeout(1000);
	getmaxyx(stdscr,row,col);
	while((ch = getch())!='q'){
		count=0;
		if (nftw((argc < 2) ? "." : argv[1], display_info, 20, flags)
				== -1) {
			perror("nftw");
			exit(EXIT_FAILURE);
		}
		switch(ch)
		{   case KEY_DOWN:
				scrl(10);
			break;
			case KEY_UP:
				scrl(-10);
			break;
		}
		move(0, 0);
		//scrl(x);

		refresh();			/* Print it on to the real screen */
	}
	endwin();			/* End curses mode		  */
    exit(EXIT_SUCCESS);
}

