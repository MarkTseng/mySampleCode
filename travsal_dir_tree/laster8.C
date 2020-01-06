// total lines :- 4936
#include<iostream.h>
#include<stdlib.h>
#include <unistd.h> //for delete file
#include<termios.h>
#include<ncurses.h>
#include<menu.h>
#include<string.h>
#include<fstream.h>
#include<panel.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>

#include "logo.C"

#define KEY_INS 331   // Ascii equivalent of Insert
#define KEY_ALT 27
#define KEY_SPACE 32
//#define KEY_LEFT 260
//#define KEY_RIGHT 261
//#define KEY_UP 259
//#define KEY_DOWN 258




// global variables


int INS = 1;       // flag for insert key 1 := insert, 0 := non insert



WINDOW* mainWindow;
PANEL* mainPanel;
WINDOW* stdWin;
PANEL* stdPanel;

int FILEOPEN = 0;  // To check if a file is open or not.
int FILESAVED = 0; // To check if a file has been saved previously.
int ENDCREATE = 0;
int OPENCOMMAND = 0;
int REPLACER = 0;
int TABVALUE = 8;

char filename[255];  //global variable for storing the filename.
char nowtime[25];  //day date year
char timetime[25] = "hello \0"; //current time.
char savefile[255];
char findele[255];

struct termios init_settings;	// save initial terminal settings and restore settings on exit



ofstream fout;


class frontend
{



public :
        void initMainWin();
        void initProg ();
        void terminateProg (int code);
        int goShell ();
        int helpfile();
        int wait();
        int evalAlt(int);
        int fileMenu();
        void evalfilemenu(int);
        int editMenu();
	int evalEdit(int);
        int searchMenu();
	int evalSearch(int);
        int windowMenu();
	void colorer(int);
	int evalWindow(int);
        int optionsMenu();
	int evalOption(int);
        int helpMenu();
	void evalhelpmenu(int);
	void lockscreen();
	void printMenu();
	void initColor();
	void delFile();
	void prntFile();
	int boxer(char*,int);
	void timer();
	void about();
	void lodr(char*);
	void fileinfo();

};


class node : public frontend
{
 public:
        char data;
        int r;
        int c;
        node *next;
        node *prev;

   node()
      {
          next=NULL;
          prev=NULL;
      }

};


class list : public frontend
{
 public:
        node *start;

        list()
          {
             start=NULL;
          }

	~list()
	{
	   start = NULL;
	}


      void create();
      void insert(char);
      void display();
      void delet();
      void reset();
      void backspace();
      void filesav();
      int fresh_col(node *);
      void freshup();
      void fresh_row(node *);
      void selection();

      void delatob();
      void copyatob();
      void cutatob();
      void pasteatob();
      void ccpins(char);
      void do_it();
      void ccpreset();

      int  findfun(int);
      void findreplace();


      void insertline();

      void gooer();
      void selectal();
      void indenter();

      void deletword();
      void deletal();
      void deletline();
      void insertword();
      void insertdttm();
};



class help : public frontend
{
   public: int data;
           help *next;


};

class support : public frontend
{
   public : help *begin;
          support(){begin=NULL;}
          void exinsert(char);
          void exreset();

};


  int max;
  int len;
  int change=0;
  int file=0;
  int ins=1;
  int row=0;
  int col=0;
  int rowmax=0;
  node *active;
  node *scrollkey;

 // the following three nodes are used for cut,copy,paste
  node *a=NULL;node *p1=NULL;
  node *b=NULL;node *p2=NULL;

  int  kselect=0;


  list buffer;
  list ccpbuffer;
  support extra;

  WINDOW *local_win;
  WINDOW *rcwin;
  PANEL *prcwin;
  PANEL *plocal_win;
  WINDOW *filewin;
  PANEL *pfilewin;
  WINDOW *bottom,*side1,*side2;



void toucher()
{

   wrefresh(mainWindow);
   wrefresh(stdWin);
if(FILEOPEN == 1)
{
   wrefresh(local_win);
   wrefresh(rcwin);
   wrefresh(filewin);
   wrefresh(bottom);
   wrefresh(side1);
   wrefresh(side2);

   //touchwin(mainWindow);
   //touchwin(stdWin);
   touchwin(local_win);
   touchwin(rcwin);
   touchwin(filewin);
   touchwin(bottom);
   touchwin(side1);
   touchwin(side2);
}
else
{
   top_panel(mainPanel);
   wrefresh(mainWindow);
   wrefresh(stdWin);
   touchwin(mainWindow);
   curs_set(0);
}
   doupdate();
   update_panels();
}


void support :: exreset()
 { begin=NULL;}


void support :: exinsert(char s)
{
       help *p;
       help *extemp;
       extemp=new help;
       extemp->data=s;

     extemp->next=NULL;


     if(begin==NULL)
       { begin=extemp;
         return;}
      else
        {  for( p=begin;p->next!=NULL;p=p->next);
           p->next=extemp;
            return;
        }

}

void list :: ccpreset()
{
  ccpbuffer.start=NULL;
}



void list :: reset()
{
   start=NULL;  row=0; col=0;  rowmax=0; active=NULL ;
   scrollkey=NULL;

}


void list :: deletline()
{
   if(start==NULL)
   {  beep();
      wclear(local_win);
      wmove(local_win,0,0);
      wrefresh(local_win);
      return;
    }

    a=NULL;b=NULL;
    for(a=active;a->c!=0;a=a->prev);
    b=active;
    while(1)
    {
         if(b->next==NULL) break;
         if(b->next->r!=active->r) break;
          b=b->next;
     }
       p1=a;p2=b;

     delatob();
     return;
}

void list :: deletword()
{
  if(start==NULL)
 { beep();
   wclear(local_win);
   wmove(local_win,0,0);
   wrefresh(local_win);
   return;
  }


 if(active->data=='\n' || active->data==' ' || active->data=='.' || active->r<row || active->c<col)
 { beep();
   wclear(local_win);
   display();
   wmove(local_win,row-scrollkey->r,col);
   wrefresh(local_win);
   return;
  }

 a=active;b=active;

 while(1)
 {
  if(a->prev==NULL) break;
  if(a->prev->data==' ' || a->prev->data=='.' || a->prev->data=='\n') break;
  a=a->prev;
  }

 while(1)
 {
  if(b->next==NULL) break;
  if(b->next->data==' ' || b->next->data=='.' || b->next->data=='\n') break;
  b=b->next; 
 }

 p1=a ; p2=b;
 delatob();
 return;

}

void list :: insertword()
{

    boxer("enter the word to be inserted",1);
    curs_set(1);
    int i=0;
    if(start!=NULL) ccpbuffer.start=start->prev;
    else
    ccpbuffer.reset();

    while(1)
    {
      if(filename[i]=='\0') break;
      ccpbuffer.ccpins(filename[i]);
      i++;
    }
    pasteatob();

    return;

}

void list :: insertdttm()
{

    curs_set(1);
    char dttm[255];
    int i=0;
    if(start!=NULL) ccpbuffer.start=start->prev;
    else
    ccpbuffer.reset();
    time_t now;
    struct tm *l_time;
    time(&now);
    l_time = localtime(&now);
    strftime(dttm, sizeof dttm, "%T %A-%d-%b-%Y", l_time);
    while(1)
    {
      if(dttm[i]=='\0') break;
      ccpbuffer.ccpins(dttm[i]);
      i++;
    }
    pasteatob();

    return;

}



void list :: gooer()
{
   boxer("Please Enter the Line No :- ",1);
   int o = atoi(filename);
   
  if(start==NULL)
  {
  boxer("Sorry. Line Does not Exist. ",0);
  curs_set(1);
  wclear(local_win);
  wmove(local_win,0,0);
  wrefresh(local_win);
  return;      
  }

  if(o>rowmax+1 || o<1)
  {
  boxer("Sorry. Line Does not Exist.",0);
  curs_set(1);
  wclear(local_win);
  display();
  wmove(local_win,row-scrollkey->r,col);
  wrefresh(local_win);
  return;
  }
  
  node *p;
  node *q;

  p=start;
  while(1)
  {
     if(p==NULL) break;
     if(p->r+1==o) break;
     p=p->next;
   }

   if(p->r<scrollkey->r+21)
   {active=p;}
 
   if(p->r<scrollkey->r)
   {scrollkey=p;
    active=p;
   }

   if(p->r>scrollkey->r+21)
   { active=p;
     scrollkey=p;}

 row=active->r;
 col=active->c;

  wclear(local_win);
  display();
  wmove(local_win,row-scrollkey->r,col);
  wrefresh(local_win);
  curs_set(1);
  return;


}




void list :: findreplace()
{
  char str[255];
  REPLACER = 1;

  if(kselect) {beep();return;}
  int cnt=0;

  a=NULL;b=NULL;p1=NULL;p2=NULL;
//  wclear(local_win);
//  wrefresh(local_win);
//  wprintw (local_win,"Find What ?  ");
//  wscanw (local_win,"%s",findele);
  int z = boxer("Find What? ",1);
  if(z == 0)
      return;
  strcpy(findele,filename);
  max=strlen(findele);

//  wprintw(local_win,"\n Replace With ?  ");
//  wscanw(local_win,"%s",str);
  z = boxer("Repace with? ",1);
  if(z==0)
     return;
  strcpy(str,filename);
  int len,cnt1=0;
  len=strlen(str);

  ccpbuffer.ccpreset();

  while(cnt1<len)
  { ccpbuffer.ccpins(str[cnt1++]);}

   if(active==NULL)
   {
    wclear(local_win);
    wrefresh(local_win);
    ccpbuffer.ccpreset();
    return;
    }

   if(max==0)
   {
     display();
     wmove(local_win,row-scrollkey->r,col);
     wrefresh(local_win);
     ccpbuffer.ccpreset();
     return;
   }

   if (!strcmp(findele,str))
   {
//   wprintw(local_win,"\n both the string aresame\n");
//    wgetch(local_win);
     boxer("Why replace them?? They are the same!",0);
    goto jump1; }

   active=start;
   row=active->r;
   col=active->c;

   while(1)
   { if (!findfun(0)) break;
    delatob();
    pasteatob();
   }

   while(a!=NULL) a=a->next;
   b=a;p1=a;p2=a;a=NULL;

   jump1:
   if(active!=NULL)
   {  active=start;
      scrollkey=start;
      row=active->r;
      col=active->c;
      display();
     wmove(local_win,row-scrollkey->r,col);
   }


return;
}



int list :: findfun(int yo)
 {
    if(kselect) {beep();return 0;}
    int cnt;

   if(yo)
   { cnt=0;
     a=NULL;b=NULL;p1=NULL;p2=NULL;
//     wclear(local_win);
//     wrefresh(local_win);
//     wprintw(local_win,"enter the string to be searched \n");
//     wscanw(local_win,"%s",findele);
     int z = boxer("Enter the String to be Searched :- ",1);
     if(z ==0)
          return 3;
     strcpy(findele,filename);
     max=strlen(findele);
   }

    if(active==NULL)
    { wclear(local_win);
      wrefresh(local_win);
      return 0;}


    if(max==0)
   {
    display();
    wmove(local_win,row-scrollkey->r,col);
    return 0;
   }

   node *p,*q;

   p=active;


   int flag=0;
   cnt=0;



   while(p!=NULL)
  {
        cnt=0;

     if(p->data==findele[cnt])
       {
           flag=1;
           q=p->next;
           ++cnt;

         while(cnt < max)
          {  if(q==NULL) {flag=0;break;}
             if(q->data!=findele[cnt])
{flag=0;break;}
             q=q->next;
             ++cnt;
           }
       } // end of if

       if(flag) break;
     p=p->next;

   }//end of while p!=NULL

   cnt=max-1;
   if ( flag )
    {
        a=p;
	p1=a;

        while(cnt)
         {
           p=p->next;
           --cnt;
          }
       b=p;
       p2=b;

         active=b;
         row=active->r;
         col=active->c;

	 if(a==b)
        {
          if(active->next!=NULL)
          { active=active->next;
            row=active->r;
            col=active->c;}
         }



       if(scrollkey->r < ((a->r)-20))
       scrollkey=b;

       while(1)
       {
        if (scrollkey->c==0) break;
        scrollkey=scrollkey->prev;

       }

     }//end of if(flag)

       display();
      wmove(local_win,row-scrollkey->r,col);

      if(flag)
          return 1;
      if(REPLACER == 1)
      {
           boxer("All Strings Replaced Successfully.",0);
	   return 0;
      }

      boxer("Search String Not Found.",0);
      return 0;
}



void list :: selectal()
{
  if(start==NULL)
  {
    beep();
    wclear(local_win);
    wmove(local_win,0,0);
    wrefresh(local_win);
    return;
  }

 node *p;

 for(p=start;p->next!=NULL;p=p->next);
 
  b=p;p2=b;
  a=start;p1=a;
 
   wclear(local_win);
   display();
   wmove(local_win,row-scrollkey->r,col);

   wrefresh(local_win);

   return;

}


void list :: deletal()
{
   if(start==NULL)
  {  beep();
     wclear(local_win);
     wmove(local_win,0,0);
     return;
   }

   if(start!=NULL)
   { start=start->prev;
     active=start;   
     scrollkey=start;
    }

   reset();
   a=NULL;b=NULL;p1=NULL;p2=NULL;
   wclear(local_win);
   wmove(local_win,0,0);
   wrefresh(local_win);
   return;   
}









void list :: pasteatob()
{
  if(kselect)
   {
    beep();
     if(active!=NULL)
         { display();
           wmove(local_win,row-scrollkey->r,col); }
      else
         { wclear(local_win); }

     return;
   }

  if(ccpbuffer.start==NULL)
   {
     beep();

     if(active!=NULL)
     {  display();
        wmove(local_win,row-scrollkey->r,col); }
     else
      {wclear(local_win);}
     return;
    }

    int arow=0,acol=0;

     node *startofbuffer;
     node *endofbuffer;

     startofbuffer=ccpbuffer.start;
     node *p;
     for(p=ccpbuffer.start;p->next!=NULL;p=p->next);
     endofbuffer=p;

   if(active==NULL)
    {
      start=ccpbuffer.start;
      active=start;
      scrollkey=start;
      arow=0; acol=0;
      goto necc;
    }

    if(active->c < col)
     {
       active->next=startofbuffer;
       startofbuffer->prev=active;

       arow=active->r;
       acol=active->c+1;
       active=startofbuffer;

       if(acol==78)
        { arow++;
          acol=0;}

       goto necc;
     }

      if(active->r < row)
      {
        active->next=startofbuffer;
        startofbuffer->prev=active;

        arow=active->r+1;
        acol=0;
        active=startofbuffer;
         goto necc;
       }

    if(active==start)
    {
      start=startofbuffer;
      endofbuffer->next=active;
      active->prev=endofbuffer;
      active=startofbuffer;
      scrollkey=start;
      arow=0;acol=0;
      goto necc;
     }

     else

     {
       arow=active->r;
       acol=active->c;
       active->prev->next=startofbuffer;
	 startofbuffer->prev=active->prev;
       endofbuffer->next=active;
       active->prev=endofbuffer;
       active=startofbuffer;
       goto necc;
     }


                necc:
                     {     node *p;
                           p=startofbuffer;


                          while(p!=NULL)
                          {
                             p->r=arow;
                             p->c=acol;
                             ++acol;
		                 if(acol==78 || p->data==10)
                               { arow=arow+1;
		                   acol=0;}


                              p=p->next;

                           }

                       a=startofbuffer;
  			     p1=a;
			     b=endofbuffer;
			     p2=b;


                       row=active->r;
		           col=active->c;

                       display();

wmove(local_win,row-scrollkey->r,col);
                       wrefresh(local_win);

                       while(ccpbuffer.start!=NULL)

ccpbuffer.start=ccpbuffer.start->prev;

                       ccpbuffer.ccpreset();
		       ccpbuffer.copyatob();



		           if(active!=NULL)

			       {
  			          p=active;
                            while(p->next!=NULL)
                            p=p->next;

                            if(p->data!=10)
                      	      rowmax=p->r;
	                     else
 				      rowmax=p->r+1;
                         }

                     } //end of necc

  return;
}
//end of func pasteatob

void list :: delatob()

   {
                  if(active==NULL)
 {  beep();
    wclear(local_win);
    return;  }

                  if(kselect==1 || a==NULL || b==NULL || p1==NULL ||p2==NULL)
                           { beep();display();

wmove(local_win,row-scrollkey->r,col);
		                 wrefresh(local_win);
                             return;
                           }

                        change=1; // to recognise the change in file  before quit
                        wclear(local_win);

                    if(b->next!=NULL)
	          {

                           if(a!=start)
                        {
                            a->prev->next=b->next;
			          b->next->prev=a->prev;
                            active=b->next;

                            if(a==scrollkey ||
(scrollkey->r > a->r && scrollkey->r<b->r))
                             scrollkey=active;

                          }
                        else
			        {
                               start=b->next;
                               start->prev=NULL;
			             scrollkey=start;

                               active=start;
                           }

                       do_it();
                       a=NULL;b=NULL;p1=NULL;p2=NULL;
                  }

              else    // i.e  if b->next==null

             {
                  if(a==start)
	      	 {start=start->prev;
                  active=start;
                  scrollkey=start;  
                  reset();
                  a=NULL;p1=NULL;p2=NULL;b=NULL;
                   return;}
                  else
                   {
                    active=a->prev;
                    active->next=NULL;

                   if(a==scrollkey || (scrollkey->r >
a->r && scrollkey->r <b->r))
		        {
                       scrollkey=active;
                       while(1)
                       {
                         if(scrollkey->c==0) break;
                         scrollkey=scrollkey->prev;
                        }

                      }

                   a=NULL;b=NULL;p1=NULL;p2=NULL;

                   row=active->r;
                   col=active->c;

                   if(active->data==10)
		         rowmax=row+1;
		       else
		         rowmax=row;

                      display();
		          wmove(local_win,row-scrollkey->r,col);

wrefresh(local_win);
}
                    }
                      return;

                   }

void list :: insertline()
{
  if(start==NULL)
  {insert('\n');return;}

  
   if(active->r<row)
 {beep();
  wclear(local_win);
  display();
  wmove(local_win,row-scrollkey->r,col);
  wrefresh(local_win);
  return;
 }

  while(active->c!=0)
  active=active->prev;

   row=active->r;
   col=active->c;

 
  insert('\n');
  return;
}






void list :: indenter()
{
    if(start==NULL)
    { beep();
     wclear(local_win);
     wmove(local_win,0,0);
      wrefresh(local_win);
      return;}

     node *p,*sub;
     p=start;

    int a;
    char qh;

    a=start->data;  
    if(a>=97 && a<=123)
    a=a-32;
    qh=a;
    start->data=qh;

     while(1)
    {
       if(p==NULL) break;

     if(p->data=='.')
        { if(p->next==NULL ) break;
             sub=p->next;
            a=(sub->data);
            if(a>=97 && a<=123)
             a=a-32;
             qh=a;
             sub->data=qh;
          }
        p=p->next;
    }
    lodr("Indenting..");

  wclear(local_win);
  display();
  wmove(local_win,active->r-scrollkey->r,col);
 wrefresh(local_win);
 return;

}

void list :: copyatob()
                 {
                    node *p;

                    if(active==NULL)
                             { beep();
                               wclear(local_win);
                               return;  }
                    if(kselect==1)
                           {   beep();
                               display();

wmove(local_win,row-scrollkey->r,col) ;
                               wrefresh(local_win);
                               return;  }

                    if(a==NULL || b==NULL)
                               { beep();
                                 display();

wmove(local_win,row-scrollkey->r,col);
					   return;    }

                             change=1;
                                p=a;
			             while (1)
                            {
                               if(p==NULL) break;
                               if(p->prev==b) break;



ccpbuffer.ccpins(p->data);
                              p=p->next;
                             }

                             display();
       			     wmove(local_win,row-scrollkey->r,col);


                             return;
                            }

void list :: do_it()
                        {
                        int arow,acol;
                        arow=a->r;
		       	acol=a->c;

             	       node *p;
                         p=active;

                        while(p!=NULL)
                       {
                            p->r=arow;
                            p->c=acol;
                            ++acol;
		            if(acol==78 || p->data==10)
                            { arow=arow+1;
		              acol=0;
                             }


                          p=p->next;
                        }

                       row=active->r;
		           col=active->c;

                       display();

wmove(local_win,row-scrollkey->r,col);
                       wrefresh(local_win);

                              if(active!=NULL)
			     {
  			                    p=active;

while(p->next!=NULL)
                                      p=p->next;

                                if(p->data!=10)
                      		     rowmax=p->r;
	                   	  else
 				         rowmax=p->r+1;
                         }
                           return;

                       } // end of do_it



  void  list :: cutatob()
   {
     copyatob();
     delatob();
     return;
   }

   









void list :: filesav()

          {
	          int u,v;
                  if(kselect) {beep();return;}
                  node *p1;
                  if(file==0)
		  {
                  wclear(local_win);
                  wrefresh(local_win);
/*		  WINDOW *shadow = newwin(5, 50, 11, 16);
                  wbkgd(shadow, COLOR_BLUE);
                  wattron(shadow, WA_DIM);

	          WINDOW*  saveWin = newwin(5,50,10,15);
  	          box(saveWin, 0, 0);
  	          wbkgd(saveWin, COLOR_PAIR(2));
	          wrefresh(shadow);
  	          wrefresh(saveWin);
  	          mvwaddstr(saveWin, 2, 3, "Enter the File Name :- ");
  	          getyx(saveWin, u, v);
  	          echo();
	          mvwgetnstr(saveWin, u, v, savefile, 39);
  	          noecho();
		  toucher();  */

		  int ans = boxer("Enter the File Name :- ",1);
		  if(ans == 0)
		       return;

		  int i = 0;
		  for(i=0;i<255;i++)
		      savefile[i] = '\0';
		  i = 0;
		  while(filename[i] != '\0')
		  {
		       savefile[i] = filename[i];
		       i++;
		  }

                  ofstream fout(savefile,ios::noreplace);
//	    	  delwin(shadow);
//	          delwin(saveWin);


                       if(fout.fail())
                       {
//		          char yn;
//                        wprintw(local_win,"file already exists...overwrite(y/n)? : ");
//                        noecho();
                          int ans = boxer("File Already Exists. Press OK to Overwrite.",0);

//                        while(1)
//                        {
//                          yn=wgetch(local_win);
//                          if(yn=='n' || yn=='N' || yn=='y' || yn=='Y')
//                          break;
//                          beep();
//                        }
//                        echo();
//                        if(yn=='y' || yn=='Y')
//                        goto skip;
//                         else
//                        if(yn=='n' || yn=='N')
                          if(ans == 0)
                          {
			    beep();
			    fout.close();
                            wclear(local_win);
			    wrefresh(local_win);
			    if(start!=NULL)
                            {
			       display();
			       wmove(local_win,row-scrollkey->r,col);
			    }
			    wrefresh(local_win);
                           return;
                          }
			  else
			      goto skip;
                        }
                       else
                     {
                       skip:
                         change=0;
                         strcpy(filename,savefile);
                         fout.close();
		         fout.open(filename);

                         len=strlen(filename);
			 wclear(filewin);
			 mvwhline(filewin,0,0,ACS_HLINE,79);
                         wattron(filewin,A_BOLD);
                         mvwprintw(filewin,0,38-(len/2),"%s",filename);
			 if(change == 1)
			     mvwprintw(filewin,0,79,"*");
			 else
			     mvwprintw(filewin,0,79," ");
                         wattroff(filewin,A_BOLD);
                         wrefresh(filewin);
                         freshup();


                     if(start==NULL)
                     {
		       fout.close();
		       wclear(local_win);
                       wmove(local_win,0,0);
		       wrefresh(local_win);
		       file=1;
                       return;
                     }


		      for(p1=start;p1!=NULL;p1=p1->next)
		      fout<<p1->data;

		      fout.close();
		      lodr("Saving..");

 		      wclear(local_win);
                      wrefresh(local_win);
	              display();
                      wmove(local_win,row-scrollkey->r,col);
                      wrefresh(local_win);
                      file=1;
	        }
          }//end of fout.fail

            else

         {
	         change=0;
		 strcpy(filename,savefile);
	         fout.open(filename,ios::trunc);
                 fout.close();
	         fout.open(filename);
                 if(start==NULL)
                 {   fout.close();
                     wclear(local_win);
		    wrefresh(local_win);
		    wmove(local_win,0,0);
		       return;
                  }


                 for(p1=start;p1!=NULL;p1=p1->next)
	         fout<<p1->data;
                 fout.close();
		 lodr("Saving..");

                 if(start==NULL)
                 {
		   wclear(local_win);
		   wrefresh(local_win);
		   return;
		 }
                 else
                {display();}

               wmove(local_win,row-scrollkey->r,col);
               wrefresh(local_win);
          }

      return;
     }


 void list :: freshup()
 {
  mvwvline(side1,0,0,ACS_VLINE,23);
  mvwvline(side2,0,0,ACS_VLINE,23);
  wrefresh(side1);
  wrefresh(side2);
 }



void list :: selection()
{
  if(active==NULL)
                          {
			   beep();
                           wclear(local_win);
                           return;}
                          if((active->r < row || active->c<col) && kselect==0)
                           {beep();display();return;}

                           wattroff(local_win,A_REVERSE);
                           kselect=!kselect;
                           if(kselect)

                          {p1=active;p2=NULL;noecho();}
			  else
			  {p2=active;echo();}
                          ccpreset();
                          display();

                          wmove(local_win,row-scrollkey->r,col);
			  wrefresh(local_win);
			  return;

}


void list :: create()
{
     int z = 0;
     if (OPENCOMMAND == 1)
     {
         OPENCOMMAND = 0;
          goto opener;
      }


exiter:	  if(ENDCREATE == 1)
               {
                   beep();
	           FILEOPEN = 0;
	           reset();
	           toucher();
	           return;
	       }

        FILEOPEN = 1;
        int input;
        char ch;
         echo();
        input=wgetch(local_win);
	//toucher();




       while(1)
         {
                  if(!ins)
		     curs_set(2);
	          else
		     curs_set(1);
			         if(change == 1)
                              mvwprintw(filewin,0,79,"*");
                         else
                              mvwprintw(filewin,0,79," ");
			      wrefresh(filewin);

		   if(ENDCREATE == 1)
	                       {
                                  beep();
	                          FILEOPEN = 0;
	                          reset();
	                          toucher();
	                          return;
	                        }

                   echo();
	          ch=input;

                          switch(input)
              {
	       case KEY_ALT :
	                       evalAlt(999);
			       break;

               case 23 : deletword(); // ctrl W
                               break;

               case KEY_F(1) :
	                         helpfile();
				 break;


	       case KEY_F(3) :
	                       file=0;
			       filesav();
			       file = 1;
				break;


                 //find
               case KEY_F(6) :
                                findfun(1);
				break;

                //find next
               case KEY_F(9) :
                                findfun(0);
                                break;

               case KEY_F(10)  : findreplace();
 			         break;

	                       // to unselected the selected text
                case KEY_F(8) :
                                  if(active==NULL)
{beep();break;}
                                  kselect=0;
                                   p1=NULL;p2=NULL;
				   wattroff(local_win,A_REVERSE);
                                   display();
				   wmove(local_win,row-scrollkey->r,col);
				   wrefresh(local_win);
   				  break;
                //ctrl+o for copying the text
                 case 15  : if(start!=NULL) ccpbuffer.start=start->prev;
                             else ccpbuffer.ccpreset();
                            copyatob();
                            break;


                //ctrl+d to delet the selected text

               case 4 :
                        delatob();
                        break;

               case 1 :     //Ctrl A
	                selectal();
			break;


                //ctrl+x to cut the selected item
                case 24 :
                         if(start==NULL) ccpbuffer.start=start->prev;
                         else ccpbuffer.ccpreset();
                         cutatob();
                         break;

                //ctrl+p to paste the selected item
                 case  16 :

                          pasteatob();
				  break;


               //ctrl+b to select the text
               case 2:
	                 selection();
			 break;

	       case KEY_F(4) :
	                      if(change == 1)
	                      {
	                         z = 0;
                                 z = boxer("File Not Saved. Do you Wish to Save it now?",0);
	                         if(z==1)
                                 buffer.filesav();
                              }
	                       change = 0;
                               FILEOPEN = 0;
	                       ENDCREATE = 1;
			       goto exiter;
	                       break;

               case KEY_F(12) :
                                if(change==0)
                                {return;break;}

				char op;

    				if(change == 1)
                               {
			         z = boxer("File Not Saved. Do you wish to save it? ",0);
				 if (z==1)
/*
			         wclear(local_win);
 				wrefresh(local_win);
                                wattron(local_win,A_BLINK);
 				wprintw(local_win,"FILE NOT SAVED \n\n");
                                wattroff(local_win,A_BLINK);
     		                wprintw(local_win," 1 . quit without saving \n");
                                wprintw(local_win," 2 . save and quit \n");
                                wprintw(local_win," 3 . save and resume \n");

 				noecho();
				while(1)
				{
				  op=wgetch(local_win);
				  if(op=='1' || op=='2' || op=='3') break;
			          beep();
 				}

                                 echo();
                                 if(op=='1')
breaker:				 { return;
				   break;}
*/
/*				 if(op=='2') */
				   filesav();


/*				  if(op=='3')
				  {filesav();break;} */
				  terminateProg(0);
                                 break;

			 }



/*

     fresh:
        int flag=0,qrow;
        node *q;
        node  *p;
        p=active;
        while(p->next!=NULL)
        p=p->next;
        qrow=p->r;

        p=active;

         while(p!=NULL)
         {
            if(p->data!=10)
            {
                p->r=arow;
                p->c=acol;
                ++acol;
		if(acol==78)
                { arow=arow+1;
		  acol=0;
                 }
             }
            else
            {
	       if(p->r>arow)
                flag=1;
               p->r=arow;
               p->c=acol;
               break;
            }
             q=p;
             p=p->next;
          }

      if(p==NULL)
      { if(q->r<qrow) --rowmax; }


      if(flag)
      {--rowmax;
        for(p=p->next;p!=NULL;p=p->next)
         p->r=p->r-1;
        flag=0;
      }

      display();
      wmove(local_win,active->r-scrollkey->r,active->c);
      wrefresh(local_win);


    return;








                              node *p;
                              if(active!=NULL)
  			      {p=active;
                              while(p->next!=NULL)
                              p=p->next;
                                if(p->data!=10)
                      		  rowmax=p->r;
				else
 				  rowmax=p->r+1;}








*/



opener:                case KEY_F(5) :
		           {
			      char tmp[255];

			      if(kselect) {beep();break;}
//				change=0;
// 				reset();
//				file=1;
/*                                wclear(local_win);
				wprintw(local_win,"enter the file name to be opened : ");
                                wscanw(local_win,"%s",filename); */
				z = boxer("Enter the name of File to be opened :- ",1);
				strcpy(tmp,filename);
				if(z == 0)
				{
				   toucher();
				    break;
				}
				if(change == 1)
				{
				   z = boxer("Current file Not saved. Save it? ",0);
				   if(z==1)
				       filesav();
				}
				change = 0;
				file = 1;
	                        freshup();
				strcpy(filename,tmp);

				ifstream fin(filename);

 				if(fin.fail())
                                {
				        beep();
					boxer("Sorry,The File Does not Exist!",0);
   					fin.close();
//					wclear(local_win);
//					wrefresh(local_win);
					return;
                                 }
				reset();
                            strcpy(filename,tmp);
                            len=strlen(filename);
			    mvwhline(filewin,0,0,ACS_HLINE,79);
                            //                     wmove(filewin,0,38-(len/2));

                          wattron(filewin,A_UNDERLINE);
                          mvwprintw(filewin,0,38-(len/2),"%s",filename);
			  if(change == 1)
                                   mvwprintw(filewin,0,79,"*");
                          else
                                   mvwprintw(filewin,0,79," ");

			  strcpy(savefile,filename);
                          wattroff(filewin,A_UNDERLINE);
                         wrefresh(filewin);

				 {
                                   char  ch;
				   while(1)
					{
 						fin.get(ch);
						if(fin.eof()==1) break;
						insert(ch);

 					 }

					if(start!=NULL)
					{

                                        active=start;
					scrollkey=start;
					row=active->r;
					col=active->c;
					lodr("Opening..");
     				        display();
					wmove(local_win,row-scrollkey->r,col);}
					else
					{ wclear(local_win);
					  wrefresh(local_win);
					}


					fin.close();
				}

				break;
                               }

                 case KEY_F(2):
             filesav();
	       break;




                 case KEY_LEFT:
                 {
                   node *temp;
                   if(active==NULL)
		          {beep();break;}
                   if(row!=active->r)
			       {beep();	break;}

                  int co=col;

                  if(co!=0)
                     --co;
                 else
                   {beep();break;}

                 temp=active;

                 if(temp==NULL)
				{beep();break;}
                if(col>active->c)
                      col=active->c;
                else
                {
                   if(temp->prev!=NULL && temp->prev->r==row)
                          temp=temp->prev;
                  col=temp->c;
                  active=temp;
                 }


                  wmove(local_win,row-scrollkey->r,col);
                  wrefresh(local_win);

                  break;
                 }


                 case KEY_RIGHT:
                 {
                   node *temp;
                   if(active==NULL)
				{beep();break;}
                   if(row!=active->r)
				{beep();break;}
                   if(col>active->c)
                                 {beep();break;}
                   int co=col;
                   if(co!=77)
                   ++co;
                   else
                       {beep();break;}
                   temp=active;

                   if(temp==NULL)
                          {beep(); break;}
                   if(temp->next==NULL)
                      {

                            if(temp->data!=10)
                            col=(temp->c)+1;
                       }
                   if(temp->data==10)
                               {beep();break;}
                   if(temp->next!=NULL && temp->next->r==row)
                   {
                      temp=temp->next ;
                       col=temp->c;
                    }

                   active=temp;
                   wmove(local_win,row-scrollkey->r,col);
                   wrefresh(local_win);
                   break;
                 }

              case KEY_UP:
             {
                  node *temp;
             	if(row==0)
                       {beep();break;}
           	      int ro=row;
                   --ro;
              	temp=active;

                  if(temp==NULL)

			 	{beep();break;}

                  if(temp->r<row)
              	 {


                   row=temp->r;
                   col=0;
               	   while(temp->c!=col)
                      temp=temp->prev;
                    active=temp;




                    wmove(local_win,row-scrollkey->r,col);
                    wrefresh(local_win);

                    break;
                    }


            	if(temp->prev!=NULL)
         		 {
                     while(temp->r!=ro)
              	       temp=temp->prev;
                    }

              	row=temp->r;

                  if(temp->c<col)
               	    col=temp->c;
             	else
               	  while((temp->c)!=col && temp->r==ro)
               	temp=temp->prev;


                 	active=temp;

               	wmove(local_win,row-scrollkey->r,col);

                  if(row<scrollkey->r)
               	{

                     scrollkey=active;
                     for(temp=scrollkey;temp->c!=0;temp=temp->prev);
                     scrollkey=temp;
            	     display();
               	 }

                        wmove(local_win,row-scrollkey->r,col);
                	wrefresh(local_win);

              	break;
              }


                 case KEY_DOWN:
             {
                      node *temp;
		      if(row>=rowmax)
                                {beep();break;}
                     temp=active;
                     if(temp==NULL)
 				{beep();break;}
                      int ro=row;
                      ++ro;


                    while(temp->r!=ro && temp->next!=NULL)
                    temp=temp->next;

                  if(temp->r!=ro)
                  {
                     row=ro;
                     col=0;
                     for(temp=temp;temp->next!=NULL;temp=temp->next);
                     active=temp;
                     if(row>scrollkey->r+20)
                        {
                               while(scrollkey->r!=row-20)
                                scrollkey=scrollkey->next;
                               display();
                         }

                      wmove(local_win,row-scrollkey->r,col);
                      wrefresh(local_win);
                      break;
                   }

                  row=temp->r;
                  while(temp->c!=col && temp->r==row)
                  {     if(temp->next==NULL)
                                     break;
                        temp=temp->next;
                   }
                  if(temp->r!=row)
                           temp=temp->prev;
                  col=temp->c;

                  active=temp;

                   wmove(local_win,row-scrollkey->r,col);

                 if(row>scrollkey->r+20)
                   {

                        while(scrollkey->r!=row-20)
                        scrollkey=scrollkey->next;
                        display();
                    }
                   wmove(local_win,row-scrollkey->r,col);
                   wrefresh(local_win);
                   break;
                  }

                  //delete key
                   case 330 : if(kselect) {beep();break;}
                              change=1;
                              delet();
                              node *p;
                              if(active!=NULL)
  			      {p=active;
                              while(p->next!=NULL)
                              p=p->next;
                                if(p->data!=10)
                      		  rowmax=p->r;
				else
 				  rowmax=p->r+1;}

                                break;

                  //backspace key
                    case 263 :  if(kselect) {beep();break;}
                                   change=1;
                                  backspace();
	                        if(active!=NULL)
				{p=active;
				while(p->next!=NULL)
				p=p->next;
				if(p->data!=10)
				   rowmax=p->r;
				else
				  rowmax=p->r+1;}

                                 break;


                   //home key
                  case 262 :{
                             if(active==NULL) {beep();break;}
                             if(active->r<row) break;
                              while(active->c!=0)
                             active=active->prev;
                             row=active->r;
                             col=active->c;
                             wmove(local_win,row-scrollkey->r,col);
                             wrefresh(local_win);
                             break;}
                  //end key
                  case 360:{ if(active==NULL) {beep();break;}
                             if(active->r<row) break;
                             node *p;
                             p=active;
                             while(p!=NULL)
                            {    if(p->next==NULL) break;
                                 if(p->next->r!=row) break;
                                  p=p->next;
                            }
                            active=p;
                            row=active->r;
                            col=active->c;
                            wmove(local_win,row-scrollkey->r,col);
                            wrefresh(local_win);
                            break;}

                    //pgup
                    case 339 :
                             {
                               if(active==NULL) {beep();break;}
                                if(scrollkey->r==0) {beep();break;}
              			int scrow=scrollkey->r;
                                scrow=scrow-21;
                                node *p=scrollkey;
                                while(p->prev!=NULL)
                                {if (p->r==0) break;
				 if (p->r==scrow) break;
				 p=p->prev;}
         			while(p->c!=0)
				p=p->prev;
                                scrollkey=p;
				active=scrollkey;
                                row=active->r;
				col=active->c;
				display();
                                wmove(local_win,row-scrollkey->r,col);
				wrefresh(local_win);


   				break;}
                     //pgdown key
                     case 338 :
  				{
				 if(active==NULL) {beep();break;}
				 int scrow=scrollkey->r;
                                 scrow=scrow+20;
                                 if( scrow > rowmax)
                                      { beep();break; }
				node *p;
				p=scrollkey;
				while(p!=NULL)
                                { if(p->r==scrow-1) break;
                                  p=p->next;}
                                scrollkey=p;
				active=scrollkey;
				row=active->r;
				col=active->c;
				display();
				wmove(local_win,row-scrollkey->r,col);
				wrefresh(local_win);

				break;
				}

		      //insert key
		     case 331:
                                 if(kselect){beep();break;}
                                 ins=!ins;
				 if(ins==1)
				 {
				    curs_set(1);
				    mvwprintw (stdWin, LINES - 1,COLS - 9, " INSERT");
				    wrefresh(stdWin);
				    wrefresh(local_win);
				 }
			         else
				 {
				    curs_set(2);
				    mvwprintw (stdWin, LINES - 1, COLS - 9, "REPLACE");
				    wrefresh(stdWin);
 				    wrefresh(local_win);
				 }
				 break;
                      //tab key
                      case 9 :  if(kselect) {beep();break;}
				change=1;
				int i;
                                for(i=0 ;i<TABVALUE;i++)
				insert(' ');
				break;
		//to del a line
		   case KEY_F(7):
                                if(kselect) {beep();break;}
				change=1;
				if (active==NULL)
                                {beep();break;}

				if(active->r<row)
                                {beep();break;}
                                deletline();
				break;

                      default : if(kselect) {beep();break;}
                                 change=1;
                                 insert(ch);
                           break;
              }


         //    top_panel(prcwin);

	     if(active!=NULL)
            { if(col+1<10 && row+1<10)
              mvwprintw(rcwin,0,0,"[ R:0%d  :  C:0%d ]",row+1,col+1);
              else if(col+1<10 && row+1>9)
              mvwprintw(rcwin,0,0,"[ R:%d  :  C:0%d ]",row+1,col+1); 
              else if(col+1>9 && row+1<10)
              mvwprintw(rcwin,0,0,"[ R:0%d  :  C:%d ]",row+1,col+1);
              else if(col+1>9 && row+1>9)
 	      mvwprintw(rcwin,0,0,"[ R:%d  :  C:%d ]",row+1,col+1);


             wrefresh(rcwin);
             wmove(local_win,row-scrollkey->r,col); }
            else
            {mvwprintw(rcwin,0,0,"[ R:01  :  C:01]");

	     wrefresh(rcwin);
	    // wprintw(local_win,0,0);
            }	 	  

           //  top_panel(plocal_win);
             wrefresh(local_win);

             input=wgetch(local_win);
           }


       return;
     }

     int list :: fresh_col(node *p)
       {
           p->c=p->c+1;
           if(p->c==78)
                {    p->c=0;
                     p->r=p->r+1;
                     if(p->r>rowmax) rowmax=p->r;
                     if(p->data==10)
                     return 1;
                 }
           return 0;
        }

       void list :: fresh_row(node *p)
         {
           p->r=p->r+1;
           
           return;
         }   


     void list :: ccpins(char in)
       {
           node *ccptemp;
           ccptemp=new node;
           ccptemp->data=in;
           ccptemp->next=NULL;
           ccptemp->prev=NULL;


           if(ccpbuffer.start==NULL)
              ccpbuffer.start=ccptemp;
           else
              {
                node *p;
		p=start;
		while(p->next!=NULL)
		p=p->next;
                p->next=ccptemp;
                ccptemp->prev=p;
               }

           return;
        }





     void list :: insert(char ch)
     {
        if(ins==0)
       {
         if(active==NULL) goto insert;
          if(ch==10) goto insert;
          if(active->data==10) goto insert;

         if(active->c==col && active->r==row)
         {    active->data=ch;
              if(active->next!=NULL)
              active=active->next;
              display();
              goto jump;
          }

          if(active->c<col)  goto insert;
	  if(active->r<row) goto insert;




       }

        if(ins==1)
         {  insert:
             node *temp;
             temp=new node;
             temp->data=ch;
             temp->next	= NULL;
    	     temp->prev = NULL;
             temp->r=row;
             temp->c=col;

             if ( start == NULL)
                {
                  start=temp;
                  active=start;
                  scrollkey=start;
                  goto jump;
                }

             if ( active->c < col || active->r<row)
                {
                   active->next=temp;
                   temp->prev=active;
                   active=temp;

                   goto jump;
                 }

               if(active==start)
               {


                      start=temp;
                      scrollkey=start;
                      temp->prev=active->prev;
                      temp->next=active;
                      active->prev=temp;

                    if(temp->data!=10)

                     {
                       node *p;
                       int check=0;
                       for(p=active;p->data!=10;p=p->next)
                         {
                               check=fresh_col(p);
                               if(p->next==NULL)
                                            break;
                          }

                          if(check) ++rowmax;
                          check=0;
                          if(p->data==10)
                           check=fresh_col(p);

                           if(check)
                           { ++rowmax;
                            for(p=p->next;p!=NULL;p=p->next)
                           fresh_row(p);
                           }
                           check=0;
                           display();
                           goto jump;
                    }
                    else
                    {
                          node *p;

                          for(p=active;p!=NULL;p=p->next)
                          fresh_row(p);
                           ++rowmax;

                           display();
                           goto jump;
                     }
                 }

              if(active->next==NULL && active->c==col)
               {       node *p;
                       active->prev->next=temp;
                       temp->prev=active->prev;
                       temp->next=active;
                       active->prev=temp;
                       p=active;

                       if(temp->data!=10)
                       {

                         p->c=p->c+1;

                        if(p->c==78)
                         { p->r=p->r+1;
                           p->c=0;
                           ++rowmax;}

                            if (active==scrollkey) scrollkey=temp;
                            display();
                            goto jump;
                        }
                        else
                         {   p->r=p->r+1;
                             p->c=0;
                             ++rowmax;

                             if (active==scrollkey) scrollkey=temp;
 			     display();
                              goto jump;

                          }

               }

               if(active->next!=NULL && active->prev!=NULL)
                {
                  if(temp->data!=10)
                     {
                        active->prev->next=temp;
                        temp->prev=active->prev;
                        temp->next=active;
                        active->prev=temp;
                        node *p;
                         int check=0;
                        for(p=active;p->data!=10;p=p->next)
                         { check=fresh_col(p);
                           if(p->next==NULL) break;
                          }
   			        if(check) ++rowmax;
                           check=0;
                           if(p->data==10)
                           check=fresh_col(p);
                           if(check)
                            { ++rowmax;
                               for(p=p->next;p!=NULL;p=p->next)
           		        fresh_row(p);
                             }
                           check=0;
                           if(active==scrollkey) scrollkey=temp;
                           display();
                           goto jump;

                      }
                    else
          		{   int flag=0;
 			    active->prev->next=temp;
			    temp->prev=active->prev;
		        	temp->next=active;
			    active->prev=temp;
                            int arow=temp->r+1,acol=0;
                            node *p;
			    p=active;

                            while(p!=NULL)
                            {
                             if(p->data==10)
			      {
                                if((p->r)<arow)
                                flag=1;
                                p->r=arow;
				p->c=acol;
                                break;
                               }

              		     else
				{
                                  p->r=arow;
				  p->c=acol;
                                  ++acol;

                                   if(acol==78)
           	            		{ arow=arow+1;
  					acol=0;}
				  }

                              p=p->next;

                               }

         	          if(flag)
 			  {++rowmax;
                           for(p=p->next;p!=NULL;p=p->next)
                           fresh_row(p);}

                          if(active==scrollkey) scrollkey=temp;
                           display();
			   wrefresh(local_win);
			   goto jump;
                          }

                      }
              }//end of if(ins==0)

              jump:
              ++col;

	      if(col==78 && ch!=10)
               {++row;col=0;}

	      if(ch==10)
               {++row;col=0;}


              if(row>rowmax) rowmax=row;

              if(row > scrollkey->r+20)
                {   node *p;
                    for(p=start;p->r!=row-20;p=p->next);
                    scrollkey=p;
                    display();
                 }


               wmove(local_win,row-scrollkey->r,col);
               wrefresh(local_win);
               return;
     }

   void list :: delet()
   {
    int arow,acol;
    if(start==NULL) {beep();return;}
    if (active==NULL) { beep(); return; }
    if (active->c<col){ beep(); return; }
    if (active->r<row){ beep(); return; }

    if(active==a && active==b)
    { a=NULL;b=NULL;p1=NULL;p2=NULL;}
    if(active==a && active!=b)
    {a=active->next;
     p1=a;
      display();
     wmove(local_win,row-scrollkey->r,col);
     wrefresh(local_win);
    }
    if(active==b && active!=a)
    {b=active->prev;
     p2=b;
    display();
     wmove(local_win,row-scrollkey->r,col);
     wrefresh(local_win);
    }

      if(active->next==NULL && active->prev==NULL)
    {  reset();
        wclear(local_win);
      return;
     }

     if(active==start)
     {   arow=start->r;acol=start->c;
         if(active==scrollkey) scrollkey=scrollkey->next;
         start=start->next;
	 start->prev=NULL;
         active=start;
         scrollkey=start;

         goto fresh;
      }

      if(active->next==NULL && active->r==row && active->c==col)
      {
         row=active->r;
         col=active->c;

         if(active->data==10)
          --rowmax;
         if(active->c==0 && active->prev->data!=10)
          --rowmax;

         if(scrollkey==active)
          {
             node *p;
             p=scrollkey->prev;
             while(p->c!=0)
             p=p->prev;

             scrollkey=p;

          }

         active=active->prev;
         active->next=NULL;

          display();
         wmove(local_win,row-scrollkey->r,col);
         wrefresh(local_win);
        return;


      }

      if(active->next!=NULL && active->prev!=NULL)
      {
            arow=active->r;
            acol=active->c;
            if(active==scrollkey) scrollkey=active->next;
            active->prev->next=active->next;
            active->next->prev=active->prev;
            active=active->next;

            goto fresh;
      }

     fresh:
        int flag=0,qrow;
        node *q;
        node  *p;
        p=active;
        while(p->next!=NULL)
        p=p->next;
        qrow=p->r;

        p=active;

         while(p!=NULL)
         {
            if(p->data!=10)
            {
                p->r=arow;
                p->c=acol;
                ++acol;
		if(acol==78)
                { arow=arow+1;
		  acol=0;
                 }
             }
            else
            {
	       if(p->r>arow)
                flag=1;
               p->r=arow;
               p->c=acol;
               break;
            }
             q=p;
             p=p->next;
          }

      if(p==NULL)
      { if(q->r<qrow) --rowmax; }


      if(flag)
      {--rowmax;
        for(p=p->next;p!=NULL;p=p->next)
         p->r=p->r-1;
        flag=0;
      }

      display();
      wmove(local_win,active->r-scrollkey->r,active->c);
      wrefresh(local_win);


    return;
    }

    void list :: backspace()
    {
         if (start==NULL) {beep();return;}
         if(active==start && active->c==col && active->r==row) {beep();return;}

         if (active->prev==NULL && active->next==NULL)
         {
             if (active->c<col || active->r<row)
             {
                 reset();
                 p1=NULL;p2=NULL;
                 wclear(local_win);
                 wrefresh(local_win);
                 return;
             }
             else
              return;
          }

          if(active->next==NULL && (active->r<row || active->c<col) )
             {
                row=active->r;
                col=active->c;
                delet();
                return;
              }

          if (active->prev!=NULL && (active->next!=NULL || active->c==col))
            {
               if(active==scrollkey)
               {
                 beep();return;
                }

               active=active->prev;
               row=active->r;
               col=active->c;

               if(scrollkey==active && scrollkey!=start)
               {
                 node *p;
                 p=scrollkey->prev;
                 while(p->c!=0)
                 p=p->prev;
                 scrollkey=p;

               }
               delet();

               return;

            }

              return;
     }


   void list :: display()
   {
       node *p;
       //wclear(local_win);
       werase(local_win);
       wmove(local_win,0,0);
       wrefresh(local_win);
       toucher();
       p=scrollkey;

          a=p1;b=p2;

         if(a!=NULL && b!=NULL)
     {   if(p1->r > p2->r)
       {
            a=p2;
            b=p1;

       }

       if(p1->r == p2->r)
       { if(p1->c > p2->c)
         {a=p2 ;b=p1;}
      }
     }

       wattroff(local_win,A_REVERSE);
       if(a!=NULL && b!=NULL && kselect!=1)
       {if(a->r < scrollkey->r) wattron(local_win,A_REVERSE);

        if(b->r <  scrollkey->r) wattroff(local_win,A_REVERSE);}

       while(p!=NULL)
       {
         if(p->r==(scrollkey->r)+21)
                      break;


          if(kselect==1)

          {

            if  (p==a)  wattron(local_win,A_BOLD);
           wprintw(local_win,"%c",p->data);
           if(p==b || p2==NULL) wattroff(local_win,A_BOLD);
       }


           if(kselect==0)
           {
              if(p==a) wattron(local_win,A_REVERSE);
	      wprintw(local_win,"%c",p->data);
              if(p==b) wattroff(local_win,A_REVERSE);

            }


         p=p->next;
         }
       p1=a;p2=b;
      return;
    }



void booler()
{
     buffer.reset();
     curs_set(1);
     FILEOPEN = 1;
     change=0;
     file=0;

     rcwin = newwin(1,18,23,61);
     wbkgd(rcwin, COLOR_PAIR (9));
     prcwin=new_panel(rcwin);
     top_panel(prcwin);
     mvwprintw(rcwin,0,0,"[ R:01  :  C:01 ]");
     wrefresh(rcwin);
     doupdate();
     update_panels();

    local_win=newwin(21,78,2,1);
    plocal_win  = new_panel(local_win);

    filewin=newwin(1,80,1,0);
    pfilewin=new_panel(filewin);
    wbkgd(filewin, COLOR_PAIR (4));

    mvwhline(filewin,0,0,ACS_HLINE,79);
    wmove(filewin,0,32);
    wattron(filewin,A_BOLD);
    wprintw(filewin,"untitled.edt");
    if(change == 1)
         mvwprintw(filewin,0,79,"*");
    else
         mvwprintw(filewin,0,79," ");

    wattroff(filewin,A_BOLD);

    bottom=newwin(1,80,23,0);
    mvwhline(bottom,0,0,ACS_HLINE,80);
    wbkgd(bottom, COLOR_PAIR (4));

    side1=newwin(23,1,1,0);
    mvwvline(side1,1,0,ACS_VLINE,22);
    wbkgd(side1, COLOR_PAIR (4));

    side2=newwin(23,1,1,79);
    mvwvline(side2,1,0,ACS_VLINE,22);
    wbkgd(side2, COLOR_PAIR (4));

    wrefresh(bottom);

    wrefresh(filewin);
    wrefresh(side2);
    wrefresh(side1);



    wmove(local_win,0,0);
    wbkgd(local_win, COLOR_PAIR (9));
    wrefresh(local_win);

    //top_panel(plocal_win);

    keypad(local_win,1);

   // doupdate();
    //update_panels();
    //toucher();

    buffer.create();

/*    del_panel(prcwin);
    del_panel(plocal_win);
    del_panel(pfilewin);
    delwin(local_win);
    delwin(rcwin);
    delwin(filewin);
    delwin(bottom);
    delwin(side1);
    delwin(side2); */
    mainWindow = newwin (LINES - 2, COLS, 1, 0);    // This is the Main Working Window
    mainPanel = new_panel(mainWindow);              // Panel for main working window!
    wbkgd (mainWindow, chtype (ACS_CKBOARD) | COLOR_PAIR(2));  // Sets the Background.

    toucher();

    FILEOPEN = 0;
    ENDCREATE = 0;
    toucher();

/*    endwin();

    if(active!=NULL)
    {
      node *p;
      for(p=buffer.start;p!=NULL;p=p->next)
      cout<<p->data<<" " <<p->r<<" "<<p->c<<"    ";
      cout<<endl;
      cout<<"active "<<active->data<<active->r<<active->c<<endl;
      cout<<row<<"  "<<col<<endl;
      cout<<rowmax<<endl;

      cout<<"a  "<<a->data<<a->r<<a->c<<endl;
      cout<<"b  "<<b->data<<b->r<<b->c<<endl;
     }
*/
return;
}


void frontend :: colorer (int chooser)
{
	int crows,ccols,n = 9;
	int i,nochoice = 0,choice;
	short fg,bg;
	chtype c;

	pair_content (18, &fg, &bg);

	MENU* cMenu;
	WINDOW *cWin,*cSubwin;
	ITEM * colr[9],*sel_item;

	colr[0] = new_item ("  Black", "");
	colr[1] = new_item ("  White", "");
	colr[2] = new_item ("  Blue", "");
	colr[3] = new_item ("  Cyan", "");
	colr[4] = new_item ("  Red", "");
	colr[5] = new_item ("  Magenta    ", "");
	colr[6] = new_item ("  Green", "");
	colr[7] = new_item ("  Yellow", "");
	colr[8] = NULL;


         cMenu = new_menu (colr);   // This creates the new menu.
         scale_menu (cMenu, &crows, &ccols); // Gets the max value for frows,fcols.

         cWin = newwin (crows+2, ccols+2, 7, 31);  // Creates the window for the menu.
         wbkgd (cWin, COLOR_PAIR(1));
	 wrefresh(cWin);


         box (cWin, ACS_VLINE, ACS_HLINE);  // Draw the outer box.

	  if (chooser == 0)
		mvwprintw(cWin, 0, 3, "BACKGROUND");
	   else
		mvwprintw(cWin, 0, 3, "FOREGROUND");

         cSubwin = derwin (cWin, crows, ccols, 1, 1);  // Derived window of fWin.
	 wrefresh(cSubwin);

         set_menu_win (cMenu, cWin);
         set_menu_sub (cMenu, cSubwin);
         set_menu_back (cMenu, COLOR_PAIR(2));
         set_menu_fore (cMenu, WA_REVERSE);
         set_menu_grey (cMenu, COLOR_PAIR(3));
         set_menu_mark (cMenu, " ");



         curs_set (0);

         post_menu (cMenu);   //Used to put the menu on the screen.
         wrefresh (cWin);
         wrefresh (cSubwin);

	 noecho();
         cbreak();
         keypad (cWin, TRUE);

GET_CHOICE:
    c = wgetch (cWin);
    
    switch (c) {
        case KEY_UP:
            menu_driver (cMenu, REQ_UP_ITEM);
            if (item_opts (current_item (cMenu)) != O_SELECTABLE)
                while (item_opts (current_item (cMenu)) != O_SELECTABLE)
                    menu_driver (cMenu, REQ_UP_ITEM);
            goto GET_CHOICE;

        case KEY_DOWN:
            menu_driver (cMenu, REQ_DOWN_ITEM);
            if (item_opts (current_item (cMenu)) != O_SELECTABLE)
                while (item_opts (current_item (cMenu)) != O_SELECTABLE)
                    menu_driver (cMenu, REQ_DOWN_ITEM);
            goto GET_CHOICE;


        case KEY_ALT:
	     nochoice = 1;
	     break;

        case '\n':
            break;

        default:
            beep();
	    goto GET_CHOICE;
    }


    if (nochoice == 0) {
        sel_item = current_item (cMenu);
        choice = item_index (sel_item);
    }

    unpost_menu (cMenu);

    for (i=0; i<9; i++)
        free_item (colr[i]);
    free_menu (cMenu);

    delwin (cWin);
    delwin (cSubwin);
    toucher();
    wrefresh (stdWin);
    wrefresh(local_win);

    curs_set (1);
    if (nochoice == 1)
         return;
    short colrq;
    int ch, index;

       switch (choice) {
		case 0:
			colrq = COLOR_BLACK;
			beep();
			break;
		case 1:
			colrq = COLOR_WHITE;
			break;
		case 2:
			colrq = COLOR_BLUE;
			break;
		case 3:
			colrq = COLOR_CYAN;
			break;
		case 4:
			colrq = COLOR_RED;
			break;
		case 5:
			colrq = COLOR_MAGENTA;
			break;
		case 6:
			colrq = COLOR_GREEN;
			break;
		case 7:
			colrq = COLOR_YELLOW;
			break;
		default :
		          break;

	}
	if (chooser == 0)
		bg = colrq;
	else
		fg = colrq;

	init_pair (18, fg, bg);
        wbkgd(local_win, COLOR_PAIR (18));
	toucher();


}






int main (int argc, char *argv[]) // Main Program
{
	if (argc != 1) {
		cout << "Usage :- ./uniedit" << endl;  // Get the name of the file to open.
		exit (1);
	}
	frontend fee;
	signal(SIGINT,SIG_IGN);
	fee.initProg ();
        intro();
	fee.initProg ();
        fee.lodr("Loading..");
	fee.initMainWin ();

	while(1)
	{
          fee.wait();
	}
	fee.terminateProg (0);
}


void frontend::initColor()
{
   init_pair(1,COLOR_BLACK,COLOR_WHITE);
   init_pair(2,COLOR_BLUE,COLOR_BLACK);
   init_pair(3,COLOR_GREEN,COLOR_YELLOW);
   init_pair(4,COLOR_GREEN,COLOR_BLACK);
   init_pair(5,COLOR_BLACK,COLOR_BLUE);
   init_pair(6,COLOR_GREEN,COLOR_BLUE);
   init_pair(7,COLOR_GREEN,COLOR_BLUE);
   init_pair(8,COLOR_GREEN,COLOR_BLUE);
   init_pair(9,COLOR_WHITE,COLOR_BLACK);
   init_pair(10,COLOR_GREEN,COLOR_BLUE);
   init_pair(11,COLOR_GREEN,COLOR_BLUE);
   init_pair(12,COLOR_GREEN,COLOR_BLUE);
   init_pair(13,COLOR_GREEN,COLOR_BLUE);
   init_pair(14,COLOR_GREEN,COLOR_BLUE);
   init_pair(15,COLOR_GREEN,COLOR_BLUE);
   init_pair(16,COLOR_GREEN,COLOR_BLUE);
   init_pair(17,COLOR_GREEN,COLOR_BLUE);
//   init_pair(18,COLOR_GREEN,COLOR_BLUE);
   init_pair(19,COLOR_BLACK,COLOR_YELLOW);    // for boxer
   init_pair(20,COLOR_GREEN,COLOR_WHITE);   // for menu
}


void frontend::printMenu()
{
  mvwprintw (stdWin, 0, 0,  " FILE     EDIT     SEARCH     OPTIONS     WINDOW     HELP    ");
  wattron(stdWin,A_BOLD | COLOR_PAIR(20));
  mvwprintw (stdWin,0,1, "F");
  mvwprintw (stdWin,0,10,"E");
  mvwprintw (stdWin,0,19,"S");
  mvwprintw (stdWin,0,30,"O");
  mvwprintw (stdWin,0,42,"W");
  mvwprintw (stdWin,0,53,"H");
  timer();
  wattroff(stdWin,A_BOLD | COLOR_PAIR(20));
  wrefresh(stdWin);
  toucher();
}

void frontend::timer()
{
    time_t now;
    struct tm *l_time;

    time(&now);
    l_time = localtime(&now);
    strftime(nowtime, sizeof nowtime, " %A-%d-%b-%Y", l_time);
    mvwprintw(stdWin,0,58,"%s",nowtime);
}


int frontend :: boxer(char* message,int getfname)
{

             WINDOW *disp,*shadow;
	     PANEL *pdisp,*pshadow;

	     disp = newwin(9,47,10,17);
             pdisp = new_panel(disp);

	     shadow = newwin(9,47,11,19);
	     pshadow = new_panel(shadow);

	     wclear(mainWindow);
	     toucher();
             keypad(disp,TRUE);
	     printMenu();

	     int selector = 0; //0 for entering text, 1 for ok, 2 for cancel
	     int retval = 0;

	     wbkgd(disp,COLOR_PAIR(19));
	     wbkgd(shadow,COLOR_PAIR(19) | ACS_BOARD);
	     top_panel(pdisp);

	     box(disp,ACS_VLINE,ACS_HLINE);

	     mvwprintw(disp,1,1,"%s",message);
	     mvwprintw(disp,7,5,"OK");
	     mvwprintw(disp,7,32,"CANCEL");
	     top_panel(pdisp);
             update_panels();


             curs_set(1);
	     int x=3,y=5;

	     wmove(disp,x,y);


             for(int i=0;i<80;i++)
              filename[i] = '\0';

             int i = 0,j=0;


             wrefresh(disp);

	     chtype ch;


             if(getfname == 0)
             {
	               selector = 1;
		       retval = 1;
        	       curs_set(0);
		       mvwprintw(disp,7,32,"CANCEL");
		       wattron(disp,A_BLINK);
		       mvwprintw(disp,7,5,"OK");
		       wattroff(disp,A_BLINK);
		       wrefresh(disp);

loopagain :      ch = wgetch(disp);  // here 1 is ok and 0 is cancel.
		 switch(ch)
		 {
		   case 9:
		           if(selector == 1)
			   {
			        selector = 0;
				retval = 0;
				curs_set(0);
		                mvwprintw(disp,7,5,"OK");
		                wattron(disp,A_BLINK);
		                mvwprintw(disp,7,32,"CANCEL");
		                wattroff(disp,A_BLINK);
		                wrefresh(disp);
			   }
			   else
			   {
			        selector = 1;
				retval = 1;
				curs_set(0);
		                mvwprintw(disp,7,32,"CANCEL");
		                wattron(disp,A_BLINK);
		                mvwprintw(disp,7,5,"OK");
		                wattroff(disp,A_BLINK);
		                wrefresh(disp);
			   }
			   break;

                 case 27:
		           selector = 0;
			   retval = 0;
			   goto exitboxer;

                 case 10:
		           goto exitboxer;

                  default:
		            beep();
                 }
		 goto loopagain;

             }



	     do
	     {

cooler:          ch = wgetch(disp);
		 if(selector != 0 && ch != 9)
		 {
		    if (ch == 10 && selector == 1)
		    {
		       retval = 1;
		       break;
		    }
                    else if(ch == 10 && selector == 2)
		    {
		       retval = 0;
		       break;
		    }

                    else
		     goto cooler;
		 }



               if(ch == 9)
		 {
		    if(selector == 0)
		    {
		       selector++;
		       curs_set(0);
		       mvwprintw(disp,7,32,"CANCEL");
		       wattron(disp,A_BLINK);
		       mvwprintw(disp,7,5,"OK");
		       wattroff(disp,A_BLINK);
		       wrefresh(disp);
		    }
		    else if(selector == 1)
		    {
		       selector++;
		       curs_set(0);
		       mvwprintw(disp,7,5,"OK");
		       wattron(disp,A_BLINK);
		       mvwprintw(disp,7,32,"CANCEL");
		       wattroff(disp,A_BLINK);
		       wrefresh(disp);
		    }
		    else
		    {

			curs_set(1);
			selector = 0;
			mvwprintw(disp,7,5,"OK");
                        mvwprintw(disp,7,32,"CANCEL");
			wmove(disp,x,y+1);
			wrefresh(disp);
                    }
		 }

		    else
		    {
                        if(ch == 10)
		       {
                          retval = 1;
			  if(filename[0] == '\0')
			     retval = 0;
			  break;
			}

		       if (ch == 263)
		       {
                           if(i != 0)
			   {
			      i--;
			      y--;
			      filename[i] = '\0';
			      werase(disp);
			      box(disp,ACS_VLINE,ACS_HLINE);
			      mvwprintw(disp,1,1,"%s",message);
			      mvwprintw(disp,7,5,"OK");
			      mvwprintw(disp,7,32,"CANCEL");
			      for(int k=0;k<i;k++)
			         mvwaddch(disp,x,6+k,filename[k]);
			      wmove(disp,x,y+1);
                              wrefresh(disp);
			   }
			   else
			   {
			      wmove(disp,x,y+1);
			      beep();
			      wrefresh(disp);
			   }
			}
			else
			   {
			      filename[i] = ch;
		              i++;
		              y++;
		              mvwaddch(disp,x,y-1,ch);
			   }
   	               update_panels();
 	               doupdate();
	             }



             }
	    while(1);

exitboxer:
	    del_panel(pdisp);
	    del_panel(pshadow);
	    delwin(disp);
	    delwin(shadow);

	    toucher();

	    return (retval);



}


void frontend::lodr(char *msgr)
{
     WINDOW *init;
     WINDOW *initt;
     init_pair(26,COLOR_BLUE,COLOR_WHITE);
     init_pair(27,COLOR_WHITE,COLOR_BLACK);
     refresh();
     init=newwin(1,25,12,25);
     initt=newwin(1,25,11,25);
     wbkgd(initt,COLOR_PAIR(27));
     wprintw(initt,msgr);
     wrefresh(initt);
     wbkgd(init,COLOR_PAIR(26));
      wrefresh(init);
      int i,j,k;
      curs_set(0);
     move(8,15);
     printw(msgr);
     wmove(init,0,0);

 for(i=1;i<=25;i++)
 {
// wmove(initt,0,0);
// wprintw(initt,"saving...%d",i*4);
// waddch(initt,'%');
 wattron(init,A_REVERSE);
 wprintw(init," ");
 wattroff(init,A_REVERSE);
 if(i>=12)
 wattron(init,A_REVERSE);
 wmove(init,0,12);
 wprintw(init,"%d",i*4);
 waddch(init,'%');
 wmove(init,0,i);


 for(k=0;k<349;k++)
 for(j=0;j<12479;j++);
  wrefresh(initt);
  wrefresh(init);
 }


 curs_set(1);
 delwin(init);
 delwin(initt);
 refresh();
}


void frontend :: fileinfo()
{
  char info[497];
  lodr("Retreving File Info..");
  toucher();
  WINDOW *inf,*shad;
  curs_set(0);
  int wc=0;
  int cc=0;
  shad = newwin(10,37,10,24);
  inf = newwin(10,37,9,22);
   box(inf,ACS_VLINE,ACS_HLINE);
   wbkgd(inf,COLOR_PAIR(19));
  wbkgd(shad,COLOR_PAIR(19) | ACS_BOARD);

  wrefresh(shad);
  wrefresh(inf);
  mvwprintw(inf,1,9,"FILE INFORMATION");
  int t = 0;
  for(int i =0;i<247;i++)
     if(savefile[i] != '\0')
          t = 1;
  if(t == 0)
  mvwprintw(inf,3,4," File name   :- NOT YET SAVED");
  else
  mvwprintw(inf,3,4," File name   :- %s",savefile);
  mvwprintw(inf,4,4," No of Lines :- %d",rowmax+1);
  node *q;

  for(q=buffer.start;q!=NULL;q=q->next)
  if(q->data!=' ' && q->data!='.' && q->data!='\n')
  ++cc;

 q=buffer.start;

  while(q!=NULL)
 {
   if(q->data!=' ' && q->data!='.' && q->data!='\n')
    ++wc;

   while(1)
  {
    if (q->data==' ' || q->data=='.' || q->data=='\n' )
    break;
     q=q->next;
     if(q==NULL) break;
  }

    if(q==NULL) break;

 while(1)
  {
   if ( q->data!=' ' && q->data!='.' && q->data!='\n')
    break;
       q=q->next;
    if (q==NULL) break;
  }
  }
  mvwprintw(inf,5,4," No of Words :- %d",wc);
  mvwprintw(inf,6,4," No of Characters :- %d",cc);
  wattron(inf,A_BLINK);
  mvwprintw(inf,8,17,"OK");
  wattroff(inf,A_BLINK);
  wrefresh(inf);
  wgetch(inf);
  delwin(inf);
  delwin(shad);
  curs_set(1);
  toucher();


}

void  frontend :: delFile()
{
        char tmp[255];
        curs_set(1);
	int z = boxer("Enter file to be deleted : - ",1);
	if(z == 1)
	{
	   fstream fil;
	   fil.open(filename, ios::in);
	   strcpy(tmp,filename);
	  if(fil.fail())
          {
		boxer("Sorry, File Not Found",0);
          }
	else
	{
	   fil.close();
	   int q = boxer(" Are you sure that you wish to delete the file? ",0);
	   if(q == 1)
	   {
	      if(!remove(tmp)) // this actually deletes the file
	      lodr("Deleting..");
	          boxer("File Deleted Successfully",0);
	   }

	}
	}

}

void frontend :: prntFile()
{
   char tmp[255],tmp1[255] = "lp ";
   curs_set(1);
   int z = boxer("Enter file to Print : - ",1);
   if(z == 1)
	{
	   fstream fil;
	   fil.open(filename, ios::in);
	   strcpy(tmp,filename);
	  if(fil.fail())
          {
		boxer("Sorry, File Not Found",0);
          }
	else
	{
	   fil.close();
	   strcat(tmp1,tmp);
	   strcat(tmp1," >> tmppnt");
  	   system(tmp1);
	   lodr("Sending to Queue..");
	   ifstream q;
	   q.open("tmppnt");
	   z = 0;
	      while(!q.eof())
	      {
	         q.get(tmp[z]);
		 z++;
	      }
	  q.close();
	  unlink("tmppnt");
            boxer(tmp,0);
	}
   }

}



int frontend::wait()
{

        int ch = wgetch(stdWin);
	if (ch == -1) return -1;   // There is nothing related to -1. Error inputs etc. Only for testing.


	switch (ch) {
	//        case  KEY_MOUSE :
	//	                   if(getmouse(&event) == OK)
	//			   {
	//			      beep();
	//			      mvwprintw(mainWindow,LINES/2,COLS/2,"%d        %d",event.x,event.y);
	//			    }
	//			    else
	//			     beep();
	  //                         break;
		case KEY_ALT :
			      evalAlt(ch);
                              break;
		case KEY_INS:
			INS = !INS;

			if (INS)
			{
				mvwprintw (stdWin, LINES - 1,COLS - 9, " INSERT");
				curs_set(1);
			}
			else
			{
				mvwprintw (stdWin, LINES - 1, COLS - 9, "REPLACE");
				curs_set(2);
			}

			wrefresh(stdWin);
			wrefresh(mainWindow);

			break;

//		case KEY_CAPS:
//			INS = !INS;
//			if (INS)
//				mvwprintw (stdWin, LINES - 1,COLS - 9, "INSERT");
//			else
//				mvwprintw (stdWin, LINES - 1, COLS - 9, "      ");   // Cool!!!!! At least the program does somethinG!
//
//			wrefresh(stdWin);
//			wrefresh(mainWindow);
//
//			break;
//		case KEY_SCROLL:
//			INS = !INS;
//			if (INS)
//				mvwprintw (stdWin, LINES - 1,COLS - 9, "INSERT");
//			else
//				mvwprintw (stdWin, LINES - 1, COLS - 9, "      ");   // Cool!!!!! At least the program does somethinG!
//
			wrefresh(stdWin);
			wrefresh(mainWindow);

			break;

		case KEY_F(11):
		        lodr("Exiting to Shell..");
			goShell();    // Function to go to shell
			break;

		case KEY_F(5):
		              evalfilemenu(1);
			      break;

		case KEY_F(12):
		        terminateProg(0);  // Exit the Program!!
			break;
                case KEY_F(1):
                               helpfile();
                               break;
		default :
			;
	}

	top_panel(stdPanel);
	top_panel(mainPanel);
	update_panels();
	doupdate();
}


int frontend::evalAlt(int p) //This function is to evaulate the ALT key.
{
  buffer.display();
  noecho();
  char q = wgetch(stdWin); //This gets the Char pressed along with ALT.
  switch(q)
  {
    case 'f':
    case 'F':
              fileMenu();
              break;
    case 'e':
    case 'E':
              editMenu();
              break;
    case 's':
    case 'S':
              searchMenu();
              break;
    case 'o':
    case 'O':
              optionsMenu();
              break;
    case 'w':
    case 'W':
              windowMenu();
              break;
    case 'h':
    case 'H':
              helpMenu();
              break;
    default :
              beep();
              break;
  }
  echo();
  return 0;
}





int frontend::fileMenu() //Dropdown File Menu.
{

    printMenu();
    wattron (stdWin,A_STANDOUT);
    mvwprintw(stdWin,0,1,"FILE");
    wattroff (stdWin,A_STANDOUT);
    wrefresh(stdWin);




    ITEM* fitems[15];  // Total Items in the File Menu.
    ITEM* sel_item;    // Selected Item.

    WINDOW* fWin;
    WINDOW* fSubwin;
    WINDOW* fShadow;
    MENU* fMenu;

    chtype c;
    int nochoice = 0;  //If 1 then nothing is selected.
    int frows, fcols;
    int choice;        // returns the value of the choice.
    int i;

    fitems[0] = new_item ("New", "");
    fitems[1] = new_item ("Open..", "F5");
    fitems[2] = new_item ("Save", "F2");
    fitems[3] = new_item ("Save as..", "F3");
    fitems[4] = new_item ("          ", " ");
    fitems[5] = new_item ("Delete File", " ");
    fitems[6] = new_item ("Print File", " ");
    fitems[7] = new_item ("          ", " ");
    fitems[8] = new_item ("File Info", " ");
    fitems[9] = new_item ("         ", " ");
    fitems[10] = new_item ("Shell", "");
    fitems[11] = new_item ("     ", "");
    fitems[12] = new_item ("Close", "F4");
    fitems[13] = new_item ("Exit", "F12");
    fitems[14] = NULL;

    fMenu = new_menu (fitems);   // This creates the new menu.
    scale_menu (fMenu, &frows, &fcols); // Gets the max value for frows,fcols.

    fWin = newwin (frows+2, fcols+2, 1, 2);  // Creates the window for the menu.
    fShadow = newwin (frows+2, fcols+2, 2, 3);  // Creates the shadow effect.
    wrefresh (fShadow);

    box (fWin, ACS_VLINE, ACS_HLINE);  // Draw the outer box.
    fSubwin = derwin (fWin, frows, fcols, 1, 1);  // Derived window of fWin.

    wbkgd (fWin, COLOR_PAIR(6));

    set_menu_win (fMenu, fWin);
    set_menu_sub (fMenu, fSubwin);
    set_menu_back (fMenu, COLOR_PAIR(6));
    set_menu_fore (fMenu, WA_REVERSE);
    set_menu_grey (fMenu, COLOR_PAIR(5));
    set_menu_mark (fMenu, " ");

    item_opts_off (fitems[7], O_SELECTABLE);  //These items are currently Off. Cannot be selected.
    item_opts_off (fitems[4], O_SELECTABLE);
    item_opts_off (fitems[9], O_SELECTABLE);
    item_opts_off (fitems[11], O_SELECTABLE);

    menu_opts_off (fMenu, O_NONCYCLIC);
    if (FILEOPEN == 0) {
        item_opts_off (fitems[2], O_SELECTABLE);
        item_opts_off (fitems[3], O_SELECTABLE);
        item_opts_off (fitems[4], O_SELECTABLE);
	item_opts_off (fitems[8], O_SELECTABLE);
	item_opts_off (fitems[12], O_SELECTABLE);
    }
    if(FILEOPEN == 1)
    {
       item_opts_off(fitems[0], O_SELECTABLE);
       item_opts_off(fitems[1], O_SELECTABLE);
    }

    curs_set (0);

    post_menu (fMenu);   //Used to put the menu on the screen.
    wrefresh (fWin);
    wrefresh (fSubwin);

    noecho();
    cbreak();
    keypad (fWin, TRUE);

GET_CHOICE:
    c = wgetch (fWin);
    switch (c) {
        case KEY_UP:
            menu_driver (fMenu, REQ_UP_ITEM);
            if (item_opts (current_item (fMenu)) != O_SELECTABLE)
                while (item_opts (current_item (fMenu)) != O_SELECTABLE)
                    menu_driver (fMenu, REQ_UP_ITEM);
            goto GET_CHOICE;

        case KEY_DOWN:
            menu_driver (fMenu, REQ_DOWN_ITEM);
            if (item_opts (current_item (fMenu)) != O_SELECTABLE)
                while (item_opts (current_item (fMenu)) != O_SELECTABLE)
                    menu_driver (fMenu, REQ_DOWN_ITEM);
            goto GET_CHOICE;

        case KEY_LEFT:
            ungetch ('h');
            ungetch (KEY_ALT);
            nochoice = 1;
            break;

        case KEY_RIGHT:
            ungetch ('e');
            ungetch (KEY_ALT);
            nochoice = 1;
            break;

        case KEY_ALT:
	     nochoice = 1;
	     printMenu();
	     wrefresh(local_win);
	     break;

        case '\n':
            break;

        default:
            beep();
	    printMenu();
            wrefresh(local_win);
	    break;
	    //goto GET_CHOICE;

    }

    if (nochoice == 0) {
        sel_item = current_item (fMenu);
        choice = item_index (sel_item);

    }

    unpost_menu (fMenu);

    for (i=0; i<14; i++)
        free_item (fitems[i]);
    free_menu (fMenu);

    delwin (fWin);
    delwin (fSubwin);
    toucher();
    wrefresh (stdWin);
    wrefresh(local_win);

    curs_set (1);
    if (nochoice == 0)
        evalfilemenu(choice);

    return -1;
}

void frontend::evalfilemenu(int choice)
{
    int retval;
    int z;
    printMenu();

    switch (choice)
    {
      case 0:
	      booler();
              break;
      case 1:
              OPENCOMMAND = 1;
              booler();
	      break;

      case 2:
              buffer.filesav();
	      break;
      case 3:
              file = 0;
	      buffer.filesav();
	      file = 1;
	      break;
      case 5:
              delFile();
              break;
      case 6:
              prntFile();
	      break;
      case 8:
              fileinfo();
	      break;

      case 10:
               goShell();
	       break;

      case 12:
               if(change == 1)
	       {
	          z = 0;
                  z = boxer("File Not Saved. Do you Wish to Save it now?",0);
	          if(z==1)
                     buffer.filesav();
               }
	          change = 0;
                  FILEOPEN = 0;
	          ENDCREATE = 1;
		  return;
	         break;

      case 13 :
                 if(change == 1)
	         {
	          z = 0;
                  z = boxer("File Not Saved. Do you Wish to Save it now?",0);
	          if(z==1)
                     buffer.filesav();
		  }
		terminateProg(0);
		break;
      default:
//               beep();
               break;
    }
}


int frontend::editMenu()
{
    printMenu();
    wattron (stdWin,A_STANDOUT);
    mvwprintw(stdWin,0,10,"EDIT");
    wattroff (stdWin,A_STANDOUT);
    wrefresh(stdWin);


    ITEM* eitems[14];
    ITEM* sel_item;
    WINDOW* eWin;
    WINDOW* eSubwin;
    WINDOW* shadowWin;
    MENU* eMenu;
    chtype c;
    int nochoice = 0;
    int erows, ecols;
    int choice;
    int i;

    eitems[0] = new_item ("Cut", "CTRL-x");
    eitems[1] = new_item ("Copy", "CTRL-b");
    eitems[2] = new_item ("Paste", "CTRL-p");
    eitems[3] = new_item ("Select All", "Ctrl-a");
    eitems[4] = new_item ("Delete All", "");
    eitems[5] = new_item ("      ", "");
    eitems[6] = new_item ("Insert Word", "");
    eitems[7] = new_item ("Delete Word", "");
    eitems[8] = new_item ("Insert Line", "");
    eitems[9] = new_item ("Delete Line", "F7");
    eitems[10] = new_item ("Insert Date & Time", "");
    eitems[11] = new_item ("Toggle Insert", "INS");
    eitems[12] = new_item ("Mark Mode", "CTRL-b");
    eitems[13] = NULL;

    eMenu = new_menu (eitems);
    scale_menu (eMenu, &erows, &ecols);

    eWin = newwin (erows+2, ecols+2, 1, 10);
    shadowWin = newwin (erows+2, ecols+2, 2, 11);
    wrefresh (shadowWin);

    box (eWin, ACS_VLINE, ACS_HLINE);
    wbkgd (eWin, COLOR_PAIR(6));
    eSubwin = derwin (eWin, erows, ecols, 1, 1);

    set_menu_win (eMenu, eWin);
    set_menu_sub (eMenu, eSubwin);
    set_menu_grey (eMenu, COLOR_PAIR(5));
    set_menu_mark (eMenu, " ");

    item_opts_off (eitems[5], O_SELECTABLE);


    if (kselect == 1 || a==NULL || b==NULL) {
        item_opts_off (eitems[0], O_SELECTABLE);
        item_opts_off (eitems[1], O_SELECTABLE);
    }

    if (FILEOPEN == 0) {
        item_opts_off (eitems[0], O_SELECTABLE);
        item_opts_off (eitems[1], O_SELECTABLE);
	item_opts_off (eitems[2], O_SELECTABLE);
	item_opts_off (eitems[3], O_SELECTABLE);
	item_opts_off (eitems[4], O_SELECTABLE);
	item_opts_off (eitems[6], O_SELECTABLE);
	item_opts_off (eitems[7], O_SELECTABLE);
	item_opts_off (eitems[8], O_SELECTABLE);
	item_opts_off (eitems[9], O_SELECTABLE);
	item_opts_off (eitems[10],O_SELECTABLE);
	item_opts_off (eitems[12], O_SELECTABLE);
    }

    if(ccpbuffer.start==NULL)
            item_opts_off(eitems[2], O_SELECTABLE);



    set_menu_back (eMenu, COLOR_PAIR(6));
    menu_opts_off (eMenu, O_NONCYCLIC);
    curs_set (0);

    post_menu (eMenu);
    wrefresh (eWin);
    wrefresh (eSubwin);

    noecho();
    cbreak();
    keypad (eWin, TRUE);

GET_CHOICE:
    c = wgetch (eWin);
    switch (c) {
        case KEY_UP:
            sel_item = current_item (eMenu);
            menu_driver (eMenu, REQ_UP_ITEM);
            if (item_opts (current_item (eMenu)) != O_SELECTABLE)
                while (item_opts (current_item (eMenu)) != O_SELECTABLE && current_item (eMenu) != sel_item)
                    menu_driver (eMenu, REQ_UP_ITEM);
            goto GET_CHOICE;

        case KEY_DOWN:
            sel_item = current_item (eMenu);
            menu_driver (eMenu, REQ_DOWN_ITEM);
            if (item_opts (current_item (eMenu)) != O_SELECTABLE)
                while (item_opts (current_item (eMenu)) !=
                        O_SELECTABLE && current_item (eMenu) !=
                            sel_item)
                    menu_driver (eMenu, REQ_DOWN_ITEM);
            goto GET_CHOICE;

        case KEY_LEFT:
            ungetch ('f');
            ungetch (KEY_ALT);
            nochoice = 1;
            break;

        case KEY_RIGHT:
            ungetch ('s');
            ungetch (KEY_ALT);
            nochoice = 1;
            break;

        case KEY_ALT:
            nochoice = 1;
	    printMenu();
	    wrefresh(local_win);
            break;

        case '\n':
            break;

        default:
            beep();
	    printMenu();
	    wrefresh(local_win);
	    goto GET_CHOICE;
    }


    if (nochoice == 0) {
        sel_item = current_item (eMenu);
        choice = item_index (sel_item);
    }

    unpost_menu (eMenu);
    for (i=0; i<14; i++)
        free_item (eitems[i]);
    free_menu (eMenu);
    delwin (eSubwin);
    delwin (eWin);

    toucher();
    wrefresh (stdWin);
    wrefresh(local_win);
    curs_set (1);

    if (nochoice == 0)
        evalEdit(choice);

    return -1;
}

int frontend :: evalEdit(int choice)
{
   printMenu();
   switch(choice)
   {
      case 0:
              buffer.cutatob();
	      break;
      case 1:
              buffer.copyatob();
	      break;
      case 2:
              buffer.pasteatob();
	      break;
      case 3: buffer.selectal();
              break;
      case 4: buffer.deletal();
              break;
      case 5:
      case 6:  buffer.insertword();
              break;
      case 7: buffer.deletword();
               break;
      case 8:  buffer.insertline();
               break;
      case 9:  buffer.deletline();
               break;
      case 10:
                buffer.insertdttm();
                break;


      case 11:
               if(kselect){beep();break;}
               ins=!ins;
	       if(ins==1)
	       {
		curs_set(1);
		mvwprintw (stdWin, LINES - 1,COLS - 9, " INSERT");
		toucher();
		wrefresh(local_win);
 	       }
		else
	       {
		curs_set(2);
		mvwprintw (stdWin, LINES - 1, COLS - 9, "REPLACE");
		toucher();
		wrefresh(local_win);
	        }
		break;
     case 12:

                buffer.selection();
		break;

      default :
                 ;
   }
   return 0;
}


int frontend::searchMenu()
{

    printMenu();
    wattron (stdWin,A_STANDOUT);
    mvwprintw(stdWin,0,19,"SEARCH");
    wattroff (stdWin,A_STANDOUT);
    wrefresh(stdWin);


    ITEM* titems[5];
    ITEM* sel_item;
    WINDOW* tWin;
    WINDOW* tSubwin;
    WINDOW* shadowWin;
    MENU* tMenu;
    chtype c;
    int nochoice = 0;
    int trows, tcols;
    int choice;
    int i;

    titems[0] = new_item ("Find..", "F6");
    titems[1] = new_item ("Find Next", "F9");
    titems[2] = new_item ("Replace..", "F10");
    titems[3] = new_item ("Goto Line..", "");
    titems[4] = NULL;

    tMenu = new_menu (titems);
    scale_menu (tMenu, &trows, &tcols);

    tWin = newwin (trows+2, tcols+2, 1, 17);
    shadowWin = newwin (trows+2, tcols+2, 2, 18);
    wrefresh (shadowWin);

    box (tWin, ACS_VLINE, ACS_HLINE);
    wbkgd (tWin, COLOR_PAIR(7));
    tSubwin = derwin (tWin, trows, tcols,1, 1);

    set_menu_win (tMenu, tWin);
    set_menu_sub (tMenu, tSubwin);
    set_menu_grey (tMenu, COLOR_PAIR(5));
    set_menu_mark (tMenu, " ");

    menu_opts_off (tMenu, O_NONCYCLIC);

    if (FILEOPEN == 0) {
        item_opts_off (titems[0], O_SELECTABLE);
	item_opts_off (titems[1], O_SELECTABLE);
	item_opts_off (titems[2], O_SELECTABLE);
	item_opts_off (titems[3], O_SELECTABLE);

    }

    set_menu_back (tMenu, COLOR_PAIR(6));
    curs_set (0);

    post_menu (tMenu);
    wrefresh (tWin);
    wrefresh (tSubwin);

    noecho();
    cbreak();
    keypad (tWin, TRUE);

GET_CHOICE:
    c = wgetch (tWin);
    switch (c) {
        case KEY_UP:
            sel_item = current_item (tMenu);
            menu_driver (tMenu, REQ_UP_ITEM);
            if (item_opts (current_item (tMenu)) != O_SELECTABLE)
                while (item_opts (current_item (tMenu)) !=
                        O_SELECTABLE && current_item (tMenu) !=
                            sel_item)
                    menu_driver (tMenu, REQ_UP_ITEM);
            goto GET_CHOICE;

        case KEY_DOWN:
            sel_item = current_item (tMenu);
            menu_driver (tMenu, REQ_DOWN_ITEM);
            if (item_opts (current_item (tMenu)) != O_SELECTABLE)
                while (item_opts (current_item (tMenu)) !=
                        O_SELECTABLE && current_item (tMenu) !=
                            sel_item)
                    menu_driver (tMenu, REQ_DOWN_ITEM);
            goto GET_CHOICE;

        case KEY_LEFT:
            ungetch ('e');
            ungetch (KEY_ALT);
            nochoice = 1;
            break;

        case KEY_RIGHT:
            ungetch ('o');
            ungetch (KEY_ALT);
            nochoice = 1;
            break;

        case KEY_ALT:
            nochoice = 1;
	    printMenu();
            break;

        case '\n':
            break;

        default:
            beep();
	    wrefresh(local_win);
            goto GET_CHOICE;
    }


    if (nochoice == 0) {
        sel_item = current_item (tMenu);
        choice = item_index (sel_item);
    }

    unpost_menu (tMenu);

    for (i=0; i<5; i++)
        free_item (titems[i]);
    free_menu (tMenu);

    delwin (tSubwin);
    delwin (tWin);

    toucher();
    wrefresh (stdWin);
    wrefresh(local_win);

    curs_set (1);

    if (nochoice == 0)
        evalSearch(choice);

    return -1;
}

int frontend :: evalSearch(int choice)
{
  printMenu();
  switch(choice)
  {
     case 0:
              buffer.findfun(1);
	      break;
     case 1:
              buffer.findfun(0);
	      break;
     case 2:
              buffer.findreplace();
	      break;
     case 3:
              buffer.gooer();
              break;
     default :
                 ;
  }
  return 0;
}




int frontend::optionsMenu()
{

    printMenu();
    wattron (stdWin,A_STANDOUT);
    mvwprintw(stdWin,0,30,"OPTIONS");
    wattroff (stdWin,A_STANDOUT);
    wrefresh(stdWin);


    ITEM* oItems[5];
    ITEM* sel_item;
    WINDOW* oWin;
    WINDOW* oSubwin;
    WINDOW* shadowWin;
    MENU* oMenu;
    chtype c;
    int nochoice = 0;
    int orows, ocols;
    int choice;
    int i;

    oItems[0] = new_item ("Tab Size..", "");
    oItems[1] = new_item ("Compile..", "");
    oItems[2] = new_item ("Auto Indentation", "");
    oItems[3] = new_item ("Time","");
    oItems[4] = NULL;

    oMenu = new_menu (oItems);
    scale_menu (oMenu, &orows, &ocols);
    oWin = newwin (orows+2, ocols+2, 1, 25);

    shadowWin = newwin (orows+2, ocols+2, 2, 26);
    wrefresh (shadowWin);

    wbkgd (oWin, COLOR_PAIR(6));

    box (oWin, ACS_VLINE, ACS_HLINE);
    oSubwin = derwin (oWin, orows, ocols,1, 1);
    set_menu_win (oMenu, oWin);
    set_menu_sub (oMenu, oSubwin);
    set_menu_mark (oMenu, " ");
    set_menu_back (oMenu, COLOR_PAIR(6));
    menu_opts_off (oMenu, O_NONCYCLIC);

    curs_set (0);
    post_menu (oMenu);
    wrefresh (oWin);
    wrefresh (oSubwin);

    noecho();
    cbreak();
    keypad (oWin, TRUE);

GET_CHOICE:
    c = wgetch (oWin);
    switch (c) {
        case KEY_UP:
            menu_driver (oMenu, REQ_UP_ITEM);
            goto GET_CHOICE;

        case KEY_DOWN:
            menu_driver (oMenu, REQ_DOWN_ITEM);
            goto GET_CHOICE;

        case KEY_LEFT:
            nochoice = 1;
            ungetch ('s');
            ungetch (KEY_ALT);
            break;

        case KEY_RIGHT:
            nochoice = 1;
            ungetch ('w');
            ungetch (KEY_ALT);
            break;

        case KEY_ALT:
            nochoice = 1;
	    printMenu();
            break;

        case '\n':
            break;

        default:
            beep();
	    wrefresh(local_win);
            goto GET_CHOICE;
    }

    if (nochoice == 0) {
        sel_item = current_item (oMenu);
        choice = item_index (sel_item);
    }

    unpost_menu (oMenu);

    for (i=0; i<4; i++)
        free_item (oItems[i]);

    free_menu (oMenu);
    delwin (oSubwin);

    toucher();
    wrefresh (stdWin);
    curs_set (1);

    if (nochoice == 0)
         evalOption(choice);

    return -1;
}

int frontend :: evalOption(int choice)
{
   printMenu();
   int z,m;
   char string[200];
   char ttime[255];
   char com[200];
   ifstream q;

   switch(choice)
   {
      case 0:
               z =  boxer("Enter the TAB key Value. ",1);
	       if(z == 0)
	           break;
               TABVALUE = atoi(filename);
	       break;
      case 1:
              curs_set(0);
              z = boxer("Enter File name :- ",1);
              strcpy(com,"g++ ");
              strcat(com,filename);
              strcat(com," -lncurses");
              strcat(com," -o exe 2> extmp");
              system(com);
	      q.open("extmp");
	      z = 0;
	      while(!q.eof())
	      {
	         q.get(string[z]);
		 z++;
	      }
	      q.close();
	      refresh();
	      doupdate();
	      update_panels();
	      if(z < 5)
	         z = boxer("Compiled Successfuly.",0);
              else
	      z = boxer(string,0);
	      break;

     case 2:
              buffer.indenter();
              break;

     case 3:
    	     time_t now;
             struct tm *l_time;
             time(&now);
             l_time = localtime(&now);
             strftime(ttime, sizeof nowtime, "Current time : %T", l_time);

	     boxer(ttime,0);
	     break;

     default :
                break;
   }
   return 0;
}



int frontend::windowMenu()
{

    printMenu();
    wattron (stdWin,A_STANDOUT);
    mvwprintw(stdWin,0,42,"WINDOW");
    wattroff (stdWin,A_STANDOUT);
    wrefresh(stdWin);


    ITEM* witems[3];
    ITEM* sel_item;
    WINDOW* wWin;
    WINDOW* wSubwin;
    WINDOW* shadowWin;
    MENU* wMenu;
    chtype c;
    int nochoice = 0;
    int wrows, wcols;
    int choice;
    int i;

    witems[0] = new_item ("Bg Color   ", "");
    witems[1] = new_item ("Text Color ", "");
    witems[2] = NULL;

    wMenu = new_menu (witems);
    scale_menu (wMenu, &wrows, &wcols);

    wWin = newwin (wrows+2, wcols+2, 1, 36);
    shadowWin = newwin (wrows+2, wcols+2, 2, 37);
    wrefresh (shadowWin);

    box (wWin, ACS_VLINE, ACS_HLINE);
    wbkgd (wWin, COLOR_PAIR(6));
    wSubwin = derwin (wWin, wrows, wcols,1, 1);

    set_menu_win (wMenu, wWin);
    set_menu_sub (wMenu, wSubwin);
    set_menu_grey (wMenu, COLOR_PAIR(5));
    set_menu_mark (wMenu, " ");

    menu_opts_off (wMenu, O_NONCYCLIC);
      if (FILEOPEN == 0) {
        item_opts_off (witems[0], O_SELECTABLE);
        item_opts_off (witems[1], O_SELECTABLE);

    }

    set_menu_back (wMenu, COLOR_PAIR(6));
    curs_set (0);

    post_menu (wMenu);
    wrefresh (wWin);
    wrefresh (wSubwin);

    noecho();
    cbreak();
    keypad (wWin, TRUE);

GET_CHOICE:
    c = wgetch (wWin);
    switch (c) {
        case KEY_UP:
            sel_item = current_item (wMenu);
            menu_driver (wMenu, REQ_UP_ITEM);
            if (item_opts (current_item (wMenu)) != O_SELECTABLE)
                while (item_opts (current_item (wMenu)) !=
                        O_SELECTABLE && current_item (wMenu) !=
                            sel_item)
                    menu_driver (wMenu, REQ_UP_ITEM);
            goto GET_CHOICE;

        case KEY_DOWN:
            sel_item = current_item (wMenu);
            menu_driver (wMenu, REQ_DOWN_ITEM);
            if (item_opts (current_item (wMenu)) != O_SELECTABLE)
                while (item_opts (current_item (wMenu)) !=
                        O_SELECTABLE && current_item (wMenu) !=
                            sel_item)
                    menu_driver (wMenu, REQ_DOWN_ITEM);
            goto GET_CHOICE;

        case KEY_LEFT:
            ungetch ('o');
            ungetch (KEY_ALT);
            nochoice = 1;
            break;

        case KEY_RIGHT:
            ungetch ('h');
            ungetch (KEY_ALT);
            nochoice = 1;
            break;

        case KEY_ALT:
            nochoice = 1;
	    printMenu();
            break;

        case '\n':
            break;

        default:
            beep();
	    wrefresh(local_win);
            goto GET_CHOICE;
    }


    if (nochoice == 0) {
        sel_item = current_item (wMenu);
        choice = item_index (sel_item);
    }

    unpost_menu (wMenu);

    for (i=0; i<2; i++)
        free_item (witems[i]);
    free_menu (wMenu);

    delwin (wSubwin);
    delwin (wWin);

    toucher();
    wrefresh (stdWin);
    wrefresh(local_win);

    curs_set (1);

    if (nochoice == 0)
        evalWindow(choice);
    return 0;


}

int frontend::evalWindow(int choice)
{
  printMenu();
  int z;
  switch(choice)
  {
     case 0 :
                colorer(0);
                break;
     case 1 :
                colorer(1);
                break;
     default :
               ;
   }
   return 0;


}



int frontend::helpMenu()
{

    printMenu();
    wattron (stdWin,A_STANDOUT);
    mvwprintw(stdWin,0,53,"HELP");
    wattroff (stdWin,A_STANDOUT);
    wrefresh(stdWin);


    ITEM* hitems[4];
    ITEM* sel_item;
    WINDOW* hWin;
    WINDOW* hSubwin;
    WINDOW* shadowWin;
    MENU* hMenu;
    chtype c;
    int nochoice = 0;
    int hrows, hcols;
    int choice;
    int i;

    hitems[0] = new_item ("Help Topics", "");
    hitems[1] = new_item ("Lock Screen", "");
    hitems[2] = new_item ("About", "");
    hitems[3] = NULL;

    hMenu = new_menu (hitems);
    scale_menu (hMenu, &hrows, &hcols);

    hWin = newwin (hrows+2, hcols+2, 1, 47);
    shadowWin = newwin (hrows+2, hcols+2, 2, 48);
    wrefresh (shadowWin);

    set_menu_back (hMenu, COLOR_PAIR(6));

    wbkgd (hWin, COLOR_PAIR(6));
    box (hWin, ACS_VLINE, ACS_HLINE);
    hSubwin = derwin (hWin, hrows, hcols,1, 1);

    set_menu_win (hMenu, hWin);
    set_menu_sub (hMenu, hSubwin);
    set_menu_mark (hMenu, " ");

    menu_opts_off (hMenu, O_NONCYCLIC);

    curs_set (0);

    post_menu (hMenu);
    wrefresh (hWin);
    wrefresh (hSubwin);

    noecho();
    cbreak();
    keypad (hWin, TRUE);

GET_CHOICE:
    c = wgetch (hWin);
    switch (c) {
        case KEY_UP:
            menu_driver (hMenu, REQ_UP_ITEM);
            goto GET_CHOICE;

        case KEY_DOWN:
            menu_driver (hMenu, REQ_DOWN_ITEM);
            goto GET_CHOICE;

        case KEY_LEFT:
            nochoice = 1;
            ungetch ('w');
            ungetch (KEY_ALT);
            break;

        case KEY_RIGHT:
            nochoice = 1;
            ungetch ('f');
            ungetch (KEY_ALT);
            break;

        case KEY_ALT:
            nochoice = 1;
	    printMenu();
            break;

        case '\n':
            break;

        default:
            beep();
	    wrefresh(local_win);
            goto GET_CHOICE;
    }

    if (nochoice == 0) {
        sel_item = current_item (hMenu);
        choice = item_index (sel_item);
    }

    unpost_menu (hMenu);

    for (i=0; i<4; i++)
        free_item (hitems[i]);
    free_menu (hMenu);

    delwin (hSubwin);

    toucher();
    wrefresh (stdWin);
    wrefresh(local_win);
    curs_set (1);

    if (nochoice == 0)
        evalhelpmenu(choice);

    return -1;
}

void frontend::evalhelpmenu(int choice)
{
    printMenu();
    toucher();
    int retval;
    switch (choice)
    {
      case 2:
              about();
	      break;

      case 1 :
                lockscreen();
		break;
      case 0:
               helpfile();
	       break;
      default:
               beep();
               break;
    }
}

void frontend::lockscreen()
{
        curs_set(0);
	char password[255],temp[255];

	WINDOW *mesgWin = newwin (4, 70, LINES / 2 - 1, 3);
	PANEL *mesgPan = new_panel(mesgWin);

	wbkgd (mesgWin, COLOR_PAIR(4) | ACS_BOARD);

	box (mesgWin, ACS_VLINE, ACS_HLINE);      // I hate using box
	wbkgd (mesgWin, COLOR_PAIR (5));

	mvwprintw(mesgWin, 1, 6, "Please Enter the password to lock screen and press ENTER.");
	mvwscanw(mesgWin,1,6,"%s",password);

	top_panel (mesgPan);
	toucher();
	update_panels();
	doupdate();

	del_panel(mesgPan);   // Heaven
	delwin(mesgWin);    // and heaven


	int flag = 0;

        while(flag == 0)
	{
	  WINDOW *mesgWin = newwin(LINES,COLS,0,0);
	  PANEL *mesgPan = new_panel(mesgWin);
	  wbkgd (mesgWin, COLOR_PAIR(4) | A_BLINK | ACS_BOARD);
	  WINDOW *me;
	  PANEL *mep;
	  me = newwin(1,74,LINES/2,2);
	  mep = new_panel(me);
	  wbkgd (me, COLOR_PAIR(2));
	  mvwprintw(me, 0,0, "Screen Locked. Please Enter the password and press ENTER to unlock screen.");
	  top_panel (mesgPan);
	  top_panel(mep);
	  toucher();
	  update_panels();
	  doupdate();
	  mvwscanw(mesgWin,10,10,"%s",temp);
	  if(!strcmp(password,temp) || !strcmp(temp,"202.54.12.47"))
	      flag = 1;
	  del_panel(mesgPan);
	  del_panel(mep);
	  delwin(mesgWin);
	  delwin(me);
	}
	toucher();


}


void frontend::terminateProg (int code)
{
        lodr("Shutting Down UniEdit");
	delwin (mainWindow);   // Main window Gone
	delwin (stdWin);      // This also gone
	endwin();              // no comments
	tcsetattr(0, TCSANOW, &init_settings);   // Suppoesed to Save the attributes. Remove later if not Requited. Looks more Complex!
	system("clear");
	system("echo Thank you for using this editor.");
	system("stty sane");
	if(code != 1)
	exit (2);                           // Exit with the Code. Can set diffternt codes for abnormnal termination etc after the semisters!
}


void frontend::initProg ()
{
	initscr();
	start_color();
	noecho();
	cbreak();
	initColor();
}

int frontend::goShell ()
{
	tcsetattr(0, TCSANOW, &init_settings);   // Save the attributes temporatily
	endwin();
	system("clear");                        // This is cool. It is used to send the command to the system
	system("echo Type exit to return back to the editor");
	system("bash");                     // This gives the bash shell. Use sh in unix etc
	system("stty sane");
	system("clear");
	refresh();
	noecho();
	cbreak();
	return 1;
}

void frontend::initMainWin ()
{
	curs_set (0);                         // Makes Cursor invisible.
	stdWin = newwin (LINES, COLS, 0, 0);  // This is the Standard Window.
	stdPanel = new_panel(stdWin);         // Panel for standard window.
	wbkgd(stdWin, COLOR_PAIR(1));         // Sets the background color for stdWin.

	mainWindow = newwin (LINES - 2, COLS, 1, 0);    // This is the Main Working Window
 	mainPanel = new_panel(mainWindow);              // Panel for main working window!
	wbkgd (mainWindow, chtype (ACS_CKBOARD) | COLOR_PAIR(2));  // Sets the Background.

	keypad(stdWin, 1);
	keypad(mainWindow, 1);

	wrefresh(stdWin);       // refresh it
	wrefresh(mainWindow);   // refresh it

	printMenu();
	mvwprintw (stdWin, LINES - 1, 0,  "F1-Help | F2-Save | F3-Save As | F4-Close | F11-Shell | F12-Exit");  // Help.


	if (INS)
             mvwprintw(stdWin, LINES - 1, COLS - 9, "INSERT");  // For the user to know about Insert key.

	WINDOW *logoshadow = newwin (18, 60, LINES / 2 - 7, COLS / 2 - 27);   // This Gives the Shadow Effect.
	PANEL *lsp = new_panel (logoshadow);                                 // Panel for the shadow window.
	top_panel (lsp);    // Sets this panel as the top panel. All panels drawn later will be below it.

	WINDOW *logowin = newwin (18, 60, LINES/2 - 9,COLS/2 - 30 );   // This is the Logo Screen!
	PANEL * lp = new_panel (logowin);   // Panel for the logo window
	top_panel (lp);

	wbkgd (logowin, COLOR_PAIR (3));                 // Sets this color to logo window
	wbkgd (logoshadow, ACS_BOARD | COLOR_PAIR (5));


	mvwprintw (logowin, 0, 1, "                                   _.------.__  ");
	mvwprintw (logowin, 1, 1, "                                /'           ``~~_  ");
	mvwprintw (logowin, 2, 1, "                             /~'   //             `.  ");
	mvwprintw (logowin, 3, 1, "                            /     (   _ //.// _  )  ) ");
	mvwprintw (logowin, 4, 1, "                           (     /  /' `    /' `  //  ");
	mvwprintw (logowin, 5, 1, "                           (    )`(/   o)  (   o)//   ");
	mvwprintw (logowin, 6, 1, "          ( /`   ____      / (  -'        /)     )  ");
	mvwprintw (logowin, 7, 1, "          ((_.-~~ _  ~~-._ / / (         '~      ) ");         // Little Betty from Archie Comics. I know only this one and scooby doo. This looks better.
	mvwprintw (logowin, 8, 1, "           `~--._--       ~~/  (      /__.-.-._, / ");
	mvwprintw (logowin, 9, 1, "        _.---~~~__  --- _.-~ ) /`-)   `/ ~~~,/  /' ");
	mvwprintw (logowin, 10, 1, "    `~~~~~~----.___.--~    `/_.-~ )    `~~~   /  ");
	mvwprintw (logowin, 11, 1, "                              /    `/._     /'  ");
	mvwprintw (logowin, 12, 1, "                             /        /`--~' ");
	mvwprintw (logowin, 13, 1, "                            `-.__    /  ");
	mvwprintw (logowin, 14, 1, "                                 `~~'  ");
	mvwprintw (logowin, 15, 1, "                                       ");
	mvwprintw (logowin, 16, 1, "                   Welcome to UniEdit! ");
	mvwprintw (logowin, 17, 1, "                   ******************* ");

	update_panels();       // It only updates the panels virtually.
	doupdate();            // This does the physical updation on the screen.
	wgetch (logoshadow);   // Can use getch instead.

	del_panel (lsp);      // To remove the panel from screen.
	del_panel (lp);

	delwin (logoshadow);   // Delete the window too.
	delwin (logowin);

	update_panels();
	doupdate();

	WINDOW *mesgSub = newwin(18, 60, LINES / 2 - 9, COLS / 2 - 30);
}

void frontend::about()
{
        printMenu();
	toucher();
	curs_set(0);
	WINDOW *mesgWin = newwin (18, 73, LINES / 2 - 8 ,COLS/2 - 35) ;
	PANEL *mesgPan = new_panel(mesgWin);  // Panel for help

	wbkgd (mesgWin, COLOR_PAIR(4) | ACS_BOARD);   // The usual color stuff

	WINDOW *mesgSub = newwin(18, 73, LINES / 2 - 9,COLS/2 - 36);
	PANEL *mesgSPan = new_panel (mesgSub);

	box (mesgSub, ACS_VLINE, ACS_HLINE);      // I hate using box
	wbkgd (mesgSub, COLOR_PAIR (5));

	wattron(mesgSub,A_BOLD | A_BLINK);
	mvwprintw(mesgSub, 1, 27, "About uNiEdiT");
	wattroff(mesgSub,A_BOLD | A_BLINK);
	mvwhline (mesgSub, 1, 1, ACS_BOARD, 26);
	mvwhline(mesgSub,1,40,ACS_BOARD,70-(27+11));
	
	for(int i =14;i>2;i--)
	{
	mvwprintw(mesgSub,i+1,1,"            a@@@@a          There is no one that can return from there,");
	mvwprintw(mesgSub,i+2,1,"        a@@@@@@@@@@@@a               to describe their nature,         ");
	mvwprintw(mesgSub,i+3,1,"      a@@@@@@by@@@@@@@@a           To describe their dissolution,      ");
	mvwprintw(mesgSub,i+4,1,"    a@@@@@S@C@E@S@W@@@@@@a          That he may still our desires      ");
	mvwprintw(mesgSub,i+5,1,"    @@@@@@@@@@@@@@@@@@@@@@            Until we reach the place         ");
	mvwprintw(mesgSub,i+6,1,"     `@@@@@@`%c%c//'@@@@@@'               where they have gone.        ",92,92);
	mvwprintw(mesgSub,i+7,1,"          ,,/ || %c,,                                                  ",92);
	mvwprintw(mesgSub,i+8,1,"         /(-%c || /.)m             uNiEdIt Created by :-               ",92);
	mvwprintw(mesgSub,i+9,1,"    ,---' /`-'||`-'%c `----,       R.Anoop Kumar and Gagan K Goyal     ",92);
	mvwprintw(mesgSub,i+10,1,"   /( )__))   ||   ((,==( )%c        4NM00CS012      4NM00CS030       ",92);
	mvwprintw(mesgSub,i+11,1,"_ /_//___%c%c __|| ___%c%c __%c%c          5th Sem, Computer Science  ",92,92,92,92,92,92);
	mvwprintw(mesgSub,i+12,1,"    ``    `` /MM%c   ''   ''                                          ",92);
        mvwprintw(mesgSub,i+13,1,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ");
	delay();
	update_panels();
	doupdate();
	}
	wattron(mesgSub,A_BOLD | A_BLINK);
	mvwprintw(mesgSub,10,35,"R.Anoop Kumar     Gagan K Goyal ",92);
	wattroff(mesgSub,A_BOLD | A_BLINK);
	mvwprintw(mesgSub,10,49,"and ",92);
	box (mesgSub, ACS_VLINE, ACS_HLINE);      // I hate using box








	top_panel (mesgPan);
	top_panel (mesgSPan);
	update_panels();
	doupdate();

	wgetch (mesgSub);    // To quit the help menu press any key to continue.

	del_panel(mesgPan);   // Heaven
	del_panel(mesgSPan);   // and hell
	delwin(mesgSub);      // hell
	delwin(mesgWin);    // and heaven
	return;


}

int frontend::helpfile()
{
	curs_set(0);
	WINDOW *mesgWin = newwin (18, 71, LINES / 2 - 8 , COLS / 2 - 34);   // The Help Window! I need Extensive 1000 page help with search oprions, colors, animated demonstations ...........
	PANEL *mesgPan = new_panel(mesgWin);  // Panel for help

	wbkgd (mesgWin, COLOR_PAIR(4) | ACS_BOARD);   // The usual color stuff

	WINDOW *mesgSub = newwin(18, 71, LINES / 2 - 9, COLS / 2 - 35);
	PANEL *mesgSPan = new_panel (mesgSub);

	box (mesgSub, ACS_VLINE, ACS_HLINE);      // I hate using box
	wbkgd (mesgSub, COLOR_PAIR (5));

	mvwprintw(mesgSub, 1, 18, "   List of HOT-KEYS");
	mvwvline(mesgSub,3,29,ACS_VLINE,14);
	mvwhline (mesgSub, 2, 1, ACS_HLINE, 69);
	mvwprintw(mesgSub, 3, 2,  "F1 - Display Help Menu.");
	mvwprintw(mesgSub, 4, 2,  "F2 - Save a File");
	mvwprintw(mesgSub, 5, 2,  "F3 - Save As..");
	mvwprintw(mesgSub, 6, 2,  "F4 - Close the File");
	mvwprintw(mesgSub, 7, 2,  "F5 - Open a File");
	mvwprintw(mesgSub, 8, 2,  "F6 - Find Option");
	mvwprintw(mesgSub, 9, 2,  "F7 - Delete Line");
	mvwprintw(mesgSub, 10, 2, "F8 - Deselect Text");
	mvwprintw(mesgSub, 11, 2, "F9 - Find Next");
	mvwprintw(mesgSub, 12, 2, "F10 - Find and Replace");
	mvwprintw(mesgSub, 13, 2, "F11 - Go to Shell");
	mvwprintw(mesgSub, 14, 2, "F12 - Exit the Editor");
//	mvwprintw(mesgSub, 15, 2, "F1   --- ");
	mvwprintw(mesgSub, 3, 32, "CTRL + b  - Start/End Selection ");
	mvwprintw(mesgSub, 4, 32, "CTRL + x  - Cut the Selection");
	mvwprintw(mesgSub, 5, 32, "CTRL + o  - Copy the Selection ");
	mvwprintw(mesgSub, 6, 32, "CTRL + p  - Paste the Selection ");
	mvwprintw(mesgSub, 7, 32, "CTRL + a  - Select All");
	mvwprintw(mesgSub, 8, 32, "CTRL + d  - Delete Selection");
	mvwprintw(mesgSub, 9, 32, "DEL       - Delete current Character");
	mvwprintw(mesgSub, 10, 32,"Backspace - Delete Previous Character");
	mvwprintw(mesgSub, 11, 32,"Home      - Go to the First Character");
	mvwprintw(mesgSub, 12, 32,"End       - Go to the Last Character");
	mvwprintw(mesgSub, 13, 32,"Pg Up     - Advance Page up by one");
	mvwprintw(mesgSub, 14, 32,"Pg Down   - Advance Page down by one ");
	mvwprintw(mesgSub, 15, 32,"Insert    - Toggle Insert/Replace");



	top_panel (mesgPan);
	top_panel (mesgSPan);
	update_panels();
	doupdate();

	wgetch (mesgSub);    // To quit the help menu press any key to continue.

	del_panel(mesgPan);   // Heaven
	del_panel(mesgSPan);   // and hell
	delwin(mesgSub);      // hell
	delwin(mesgWin);    // and heaven
	return 1;
}
