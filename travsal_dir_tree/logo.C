int delay()
{
        wtimeout(stdscr,147);
	if(getch() != ERR)
	{
              wtimeout(stdscr,-1);
                return 1;
	}
	else
	{
		wtimeout(stdscr,-1);
		return 0;
	}
}



void intro()
{
   init_pair(1,COLOR_BLACK,COLOR_BLACK);
   init_pair(2,COLOR_BLUE,COLOR_BLACK);
   init_pair(3,COLOR_GREEN,COLOR_BLACK);
   init_pair(4,COLOR_YELLOW,COLOR_BLACK);
   init_pair(5,COLOR_RED,COLOR_BLACK);
   init_pair(6,COLOR_MAGENTA,COLOR_BLACK);
   init_pair(7,COLOR_CYAN,COLOR_BLACK);
   init_pair(8,COLOR_WHITE,COLOR_BLACK);

   WINDOW* logowin;
   PANEL* logopanel;
   logowin = newwin(24,78,0,0);
   logopanel = new_panel(logowin);
   curs_set(0);
   int i = 0;
   int k = 0;
   int q = 0;
   int flg = 0;
   int b = 0,s=0;
   while(1)
   {
     i++;
     if(flg == 0)
       q++;
     else
       q--;

     k++;
     if(k > 8)
         k = 0;

    wbkgd(logowin,COLOR_PAIR(k) | A_BOLD) ;
   mvwprintw(logowin,0,q," 4NM00CS012       coDe        4NM900CS030 ");
   mvwprintw(logowin,1,q,"R.Anoop Kumar <--wArRiOrS-->  Gagan K Goel");
   mvwprintw(logowin,2,q,"~~~~~~~~~~~~~                ~~~~~~~~~~~~~");
   if(b > 4)
   {
	   beep();
	   
   if(b > 5)
   {
       beep();
       b = 0;
    }
   }
   b++;    
   if(s > 9)
   {
	    beep();
	    s = 0;
   }
   s++;
   

   if(q > 34)
      flg = 1;

   if (q < 0)
      flg = 0;

   if(delay())
         break;
//   mvwprintw(logowin,1,1,"                             //|                              ,|         ");
//   mvwprintw(logowin,2,1,"                           //,/                             -~ |         ");
   mvwprintw(logowin,3,1,"                         // / |                         _-~   /  ,       ");
   mvwprintw(logowin,4,1,"                       /'/ / /                       _-~   _/_-~ |       ");
   mvwprintw(logowin,5,1,"                      ( ( / /'                   _ -~     _-~ ,/'        ");
   mvwprintw(logowin,6,1,"                       /~//'/|             __--~~__--/ _-~  _/,          ");
   mvwprintw(logowin,7,1,"               ,,)))))));, //~-_     __--~~  --~~  __/~  _-~ /           ");
   mvwprintw(logowin,8,1,"            __))))))))))))));,>//   /        __--~~  -~~ _-~             ");
   mvwprintw(logowin,9,1,"           -/(((((''''(((((((( >~//     --~~   __--~' _-~ ~|             ");
   mvwprintw(logowin,10,1,"  --==//////((''  .     `)))))), /     ___---~~  ~~/~~__--~              ");
   mvwprintw(logowin,11,1,"          ))| @    ;-.     (((((/           __--~~~'~~/                  ");
   mvwprintw(logowin,12,1,"          ( `|    /  )      )))/      ~~~~~__/__---~~__--~~--_           ");
   mvwprintw(logowin,13,1,"             |   |   |       (/      ---~~~/__-----~~  ,;::'  /         ,");
   mvwprintw(logowin,14,1,"             o_);   ;        /      ----~~/           /,-~~~/  |       /| ");
   mvwprintw(logowin,15,1,"                   ;        (      ---~~/         `:::|      |;|      < > ");
   mvwprintw(logowin,16,1,"|   _      `----~~~~'      /  uNiEdiT       /;/_____//       | |      / / ");
   mvwprintw(logowin,17,1,"            ______///~    | beta version    /        /         ~------~   ");
   mvwprintw(logowin,18,1,"          /~;;.____/;;'  /          ___----(   `;;;/                     ");
   mvwprintw(logowin,19,1,"         / //  _;______;'------~~~~~    |;;//    /       Press any       ");
   mvwprintw(logowin,20,1,"        //  | |                        /  |  /;;,/          key          ");
   mvwprintw(logowin,21,1,"       (<_  | ;     A Versatile Editor/,/-----'  _>     to continue      ");
   mvwprintw(logowin,22,1,"        |_| ||_             for     //~;~~~~~~~~~                        ");
   mvwprintw(logowin,23,1,"            `|_|     Unix and Linux(,~~                                  ");
   mvwprintw(logowin,24,1,"                                    (|~)                                  ");

   update_panels();
   doupdate();

   werase(logowin);

 }

   wclear(logowin);
   del_panel(logopanel);
   delwin(logowin);
//   endwin();
}
