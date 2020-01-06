/*******************************************
* File Name : test.c
* Purpose :
* Creation Date : 29-08-2016
* Last Modified : Mon 29 Aug 2016 03:47:50 PM CST
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


char* left(char *string, const int length);
void malloc_error(void);
int texteditor(char *text,int maxchars,const int startrow, const int startcol,int maxrows,const int maxcols,const int displayrows, const int returnhandler,const char *permitted, bool ins, const bool allowcr);

int main(void)
        {
        int maxchars=40*80+1,startrow=0,startcol=0,maxrows=40,maxcols=80,displayrows=20,returnhandler=1;
        char *message;
        bool ins=false,allowcr=true;
        initscr();
        noecho();
        cbreak();
        keypad(stdscr,TRUE);

        move(20,0);
        hline(ACS_CKBOARD,80);
        mvaddstr(21,0,"Press TAB or BACK TAB to move between fields");
        mvaddstr(21,50,"Press F3 to Clear Text");
        mvaddstr(22,0,"Press CTRL-Y to Delete Current Line");

        message=malloc(maxchars*sizeof(char));


        strcpy(message,"Multi-line Text Editor Demo");

        texteditor(message,maxchars,startrow,startcol,maxrows,maxcols,displayrows,returnhandler,NULL,ins,allowcr);

        clear();
        addstr("Exited text editor");
        getch();
        endwin();
        printf("Message\n-------\n\"%s\"",message);
        free(message);
        return 0;
        }

int texteditor(char *text, int maxchars, const int startrow, const int startcol,int maxrows,const int maxcols,const int displayrows,const int returnhandler,const char *permitted, bool ins, const bool allowcr)
/*
 *  PARAMETERS
 * ----------
 * char * text = text string to edit
 * const int maxchars = maximum number of characters to allow in text string, including final \0 (example: string "ABC"=maxchars 4)
 *                      NOTE: If maxchars==0 then maxchars=maxrows*maxcols+1
 * const int startrow = top row for the text entry window
 * const int startcol = top column number for the text entry window
 * int maxrows = maximum number of rows (lines) to allow in the text string.
 *                      NOTE:  If maxrows > displayrows, the text entry window will scroll vertically
 *                 If maxrows == displayrows, text entry window scrolling is disabled
 *                                 If maxrows ==0 then maxrows=maxchars-1, i.e one character per line 
 * const int maxcols = width of the text entry window
 * const int displayrows = number of rows in the text entry window, if < maxrows the text entry window will scroll
 *                      NOTE:  A row does not necessarily end with \n, it may end with a space
 *                                 or simply wrap to the next row if the continuous length of the word exceeds maxcols
 * const int returnhandler = 0 ignore cr
 *                                                       1 cr=\n
 *                                                       2 replace cr with ' '
 *                                                       3 exit function when cr is received
 * const char *permitted = NULL all regular text characters accepted
 *                                                 if a string, accept only characters included in the string
 * bool ins = initial insert mode on/off (user changable by hitting the ins key while editing)
 * const bool allowcr = allow a \n as the first character in the string, i.e. a blank line at the top of the entered string
 *
 * EDITING CONTROL KEYS
 * INS = insert mode on/off
 * PgUp = goto first character in text string
 * PgDn = goto last character in text string
 * Home = goto first character on current row (line)
 * End = goto last character on current row (line)
 * ctrl-Y = delete current line
 * F3 = delete all text
 * F1 = exit function and discard all changes to the text
 * ESC (twice) = same as F1
 * tab = exit function
 * back tab (shift-tab) = exit function
 * return = exit function if returnhandler==3
 *
 * RETURN VALUE
 * ------------
 * Describes the key that was used to exit the function
 * tab=6
 * back tab (shift-tab) = 4
 * F1 or ESC (twice)= 27
 * return =0 (if returnhandler==3)
 */
        {
        int ky,position=0,row,col,scrollstart=0;
        char *where, *original, *savetext,**display; 
        bool exitflag=false;

        if (!maxchars)
                maxchars=maxrows*maxcols+1;

        if (!maxrows || maxrows > maxchars-1)
                maxrows=maxchars-1;
                
        if (ins)
                curs_set(2);
        else
                curs_set(1);

        if ((display = malloc(maxrows * sizeof(char *))) ==NULL)
                malloc_error();
        for(ky = 0; ky < maxrows; ky++)
                if ((display[ky] = malloc((maxcols+1) * sizeof(char)))==NULL)
                        malloc_error();
                
        if ((original=malloc(maxchars*sizeof(char)))==NULL)
                malloc_error();
        strcpy(original,text);

        if ((savetext=malloc(maxchars*sizeof(char)))==NULL)
                malloc_error();

        
        while (!exitflag)
                {
                int counter;
                do
                        {
                        counter=0;
                        where=text;
                        
                        for (row=0; row < maxrows; row++)
                                {
                                display[row][0]=127;
                                display[row][1]='\0';
                                }
                
                        row=0;
                        while ((strlen(where) > maxcols || strchr(where,'\n') != NULL) &&  (display[maxrows-1][0]==127 || display[maxrows-1][0]=='\n'))
                                {
                                strncpy(display[row],where,maxcols);
                                display[row][maxcols]='\0';
                                if (strchr(display[row],'\n') != NULL)
                                        left(display[row],strchr(display[row],'\n')-display[row]);
                                else
                                        left(display[row],strrchr(display[row],' ')-display[row]);
                                if (display[maxrows-1][0]==127 || display[maxrows-1][0]=='\n')
                                        {
                                        where+=strlen(display[row]);
                                        if (where[0]=='\n' || where[0]==' ' || where[0]=='\0')
                                                where++;
                                        row++;
                                        }
                                }
                        if (row == maxrows-1 && strlen(where) > maxcols) // undo last edit because wordwrap would make maxrows-1 longer than maxcols
                                {
                                strcpy(text,savetext);
                                if (ky==KEY_BACKSPACE)
                                        position++;
                                counter=1;
                                }
                        }
                while (counter);
                        
                strcpy(display[row],where);
                
                ky=0;
                if (strchr(display[maxrows-1],'\n') != NULL) // if we have a \n in the last line then we check what the next character is to insure that we don't write stuff into the last line+1
                        if (strchr(display[maxrows-1],'\n')[1] != '\0')
                                ky=KEY_BACKSPACE; // delete the last character we entered if it would push the text into the last line +1

                col=position;
                row=0;
                counter=0;
                while (col > strlen(display[row]))
                        {
                        col-=strlen(display[row]);
                        counter+=strlen(display[row]);
                        if (text[counter] ==' ' || text[counter]=='\n' || text[counter]=='\0')
                                {
                                col--;
                                counter++;
                                }
                        row++;
                        }
                if (col > maxcols-1)
                        {
                        row++;
                        col=0;
                        }
                
                if (!ky) // otherwise  ky==KEY_BACKSPACE and we're getting rid of the last character we entered to avoid pushing text into the last line +1
                        {
                        if (row < scrollstart)
                                scrollstart--;
                        if (row > scrollstart+displayrows-1)
                                scrollstart++;
                        for (counter=0;counter < displayrows; counter++)
                                {
                                mvhline(counter+startrow,startcol,' ',maxcols);
                                if (display[counter+scrollstart][0] != 127)
                                        mvprintw(counter+startrow,startcol,"%s",display[counter+scrollstart]);
                                }
                        move(row+startrow-scrollstart,col+startcol);
                        
                        ky=getch(); 
                        }
                        
                switch(ky)
                        {
                        case KEY_F(1): // function key 1
                                strcpy(text,original);
                        case 9: // tab
                        case KEY_BTAB: // shift-tab
                                exitflag=true;
                                break;
                        case 27: //esc
                                // esc twice to get out, otherwise eat the chars that don't work
                                //from home or end on the keypad
                                ky=getch();
                                if (ky == 27)
                                        {
                                        strcpy(text,original);
                                        exitflag=true;
                                        }
                                else
                                        if (ky=='[')
                                                {
                                                getch();
                                                getch();
                                                }
                                        else 
                                                ungetch(ky);
                                break;
                        case KEY_F(3):
                                memset(text,'\0',maxchars);
                                position=0;
                                scrollstart=0;
                                break;
                        case KEY_HOME:
                                if (col)
                                        {
                                        position=0;
                                        for (col=0; col < row; col++)
                                                {
                                                position += strlen(display[col]);
                                                if ((strchr(display[row],'\n') != NULL) || (strchr(display[row],' ') != NULL))
                                                        position++;
                                                }
                                        }
                                break;
                        case KEY_END:
                                if (col < strlen(display[row]))
                                        {
                                        position=-1;
                                        for (col=0; col <=row; col++)
                                                {
                                                position+=strlen(display[col]);
                                                if ((strchr(display[row],'\n') != NULL) || (strchr(display[row],' ') != NULL))
                                                        position++;
                                                }
                                        }
                                break;
                        case KEY_PPAGE:
                                position=0;
                                scrollstart=0;
                                break;
                        case KEY_NPAGE:
                                position=strlen(text);
                                for (counter=0; counter < maxrows; counter++)
                                        if(display[counter][0]==127)
                                                break;
                                scrollstart=counter-displayrows;
                                if (scrollstart < 0)
                                        scrollstart=0;
                                break;
                        case KEY_LEFT:
                                if (position)
                                        position--;
                                break;
                        case KEY_RIGHT:
                                if (position < strlen(text) && (row != maxrows-1 || col < maxcols-1))
                                        position++;
                                break;
                        case KEY_UP:
                                if (row)
                                        {
                                        if (col > strlen(display[row-1]))
                                                position=strlen(display[row-1]);
                                        else
                                                position=col;
                                        ky=0;
                                        for (col=0; col < row-1; col++)
                                                {
                                                position+=strlen(display[col]);
                                                ky+=strlen(display[col]);
                                                if ((strlen(display[col]) < maxcols) || (text[ky]==' ' && strlen(display[col])==maxcols))
                                                        {
                                                        position++;
                                                        ky++;
                                                        }
                                                }
                                        }
                                break;
                        case KEY_DOWN:
                                if (row < maxrows-1)
                                        if (display[row+1][0] !=127)
                                                {
                                                if (col < strlen(display[row+1]))
                                                        position=col;
                                                else
                                                        position=strlen(display[row+1]);
                                                ky=0;
                                                for (col=0; col <= row; col++)
                                                        {
                                                        position+=strlen(display[col]);
                                                        ky+=strlen(display[col]);
                                                        if ((strlen(display[col]) < maxcols) || (text[ky]==' ' && strlen(display[col])==maxcols))
                                                                {
                                                                position++;
                                                                ky++;
                                                                }
                                                        }
                                                
                                                }
                                break;
                        case KEY_IC: // insert key
                                ins=!ins;
                                if (ins)
                                        curs_set(2);
                                else
                                        curs_set(1);
                                break;
                        case KEY_DC:  // delete key
                                if (strlen(text))
                                        {
                                        strcpy(savetext,text);
                                        memmove(&text[position],&text[position+1],maxchars-position);
                                        }
                                break;
                        case KEY_BACKSPACE:
                                if (strlen(text) && position)
                                        {
                                        strcpy(savetext,text);
                                        position--;
                                        memmove(&text[position],&text[position+1],maxchars-position);
                                        }
                                break;
                        case 25: // ctrl-y
                                if (display[1][0] != 127)
                                        {
                                        position-=col;
                                        ky=0;
                                        do
                                                {
                                                memmove(&text[position],&text[position+1],maxchars-position);
                                                ky++;
                                                }
                                        while (ky < strlen(display[row]));
                                        }
                                else
                                        memset(text,'\0',maxchars);
                                break;
                        case 10: // return
                                switch (returnhandler)
                                        {
                                        case 1:
                                                if (display[maxrows-1][0] == 127 || display[maxrows-1][0] == '\n')
                                                        {
                                                        memmove(&text[position+1],&text[position],maxchars-position);
                                                        text[position]='\n';
                                                        position++;
                                                        }
                                                break;
                                        case 2:
                                                ky=' ';
                                                ungetch(ky);
                                                break;
                                        case 3:
                                                exitflag=true;
                                        }
                                break;
                        default:
                                if (((permitted==NULL && ky > 31 && ky < 127) || (permitted != NULL && strchr(permitted,ky))) && strlen(text) < maxchars-1 && (row !=maxrows-1 || (strlen(display[maxrows-1]) < maxcols || (ins && (row!=maxrows-1 && col < maxcols )))))
                                        {
                                        if (ins || text[position+1]=='\n' || text[position]== '\n')
                                                memmove(&text[position+1],&text[position],maxchars-position);
                                        text[position]=ky;
                                        if (row != maxrows-1 || col < maxcols-1)
                                                position++;
                                        }
                        }
                if(!allowcr)
                        if (text[0]=='\n')
                                {
                                memmove(&text[0],&text[1],maxchars-1);
                                if (position)
                                        position--;
                                }
                }

        free(original);
        free(savetext);
        for(scrollstart = 0; scrollstart < maxrows; scrollstart++)
                free(display[scrollstart]);
        free(display);
        
        switch(ky)
                {
                case 9: // tab
                        return 6;
                case KEY_BTAB:
                        return 4;
                case KEY_F(1):
                case 27: // esc
                        return 5;
                }
        return 0; // we hit the return key and returnhandler=3
        }
        

char* left(char *string, const int length)
        {
        if (strlen(string) > length)
                string[length]='\0';
        return string;
        }

void malloc_error(void)
        {
        endwin();
        fprintf(stderr, "malloc error:out of memory\n");
        exit(EXIT_FAILURE);
        }
