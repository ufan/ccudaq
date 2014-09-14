#include "curses.h"

int main(int argc,char* argv[])
{
    initscr();

    WINDOW* form1=newwin(26,40,0,0);
    box(form1,ACS_BOARD,ACS_BOARD);
    wrefresh(form1);

    WINDOW* form2=newwin(26,40,0,40);
    box(form2,ACS_DIAMOND,ACS_BLOCK);
    wrefresh(form2);



/*
    printf("LINES=%d\n",LINES);
    printf("COLS=%d\n",COLS);
    printf("COLORS=%d\n",COLORS);
    printf("COLOR_PAIRS=%d\n",COLOR_PAIRS);
    printf("TABSIZE=%d\n",TABSIZE);
    printf("acs_amp: %s\n",acs_map);
    printf("ttytype: %s\n",ttytype);
*/
    while(1){}
    return 0;
}
