#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>

#define DELAY 10000

struct player {
	int r,c;
	char ch;
};

struct mushroom {
	int r,c;
	int active; /* 1=active 0=inactive */
};

struct centipede {
	int r,c;
	int pr,pc;
	int alive; /* 1=alive 0=destroyed */
	char direction; /* 'l'=left 'r'=right */
	char ch;
};

struct shoot {
	int r,c;
	int active; /* 1=active 0=inactive */
	char ch;
};

struct options {
	int overall,centipede,shots,bombchance;
};

void menu(struct options *settings);
void gameover(int win);


/* The main function handles user input, the game visuals, and checks for win/loss conditions */
int main() {
   struct player tank;
   struct centipede aliens[30];
   struct shoot shot[3];
   struct mushroom M[25];
   struct options settings;
   unsigned int input, loops=0, i=0, j=0, currentshots=0, currentcentipede=30;
   int random=0, score=0, win=-1;
   char tellscore[30];
   
   initscr();
   clear();
   noecho();
   cbreak();
   nodelay(stdscr,1);
   keypad(stdscr,1);
   srand(time(NULL)); 

   /* Set default settings */
   settings.centipede = 12;
   settings.shots = 3;

   /* Set tank settings */
   tank.r = LINES - 1;
   tank.c = COLS / 2;
   tank.ch = '^';

   /* Set aliens settings */
   for (i=0; i<10; ++i) {
      aliens[i].r = 1;
      aliens[i].c = i*2;
      aliens[i].pr = 0;
      aliens[i].pc = 0;
      aliens[i].ch = '#';
      aliens[i].alive = 1;
      aliens[i].direction = 'r';
   }
    
   /* Set shot settings */
   for (i=0; i<3; ++i) {
      shot[i].active = 0;
      shot[i].ch = '*';
   }
   
   //mushroom setting
   for(i=0;i<25;i++)
   {
	   M[i].active = 1;
	   M[i].c = rand() % COLS + 2;
       M[i].r = rand() % (LINES-3) + 2;
   }
   
    //MUSHROOM
       for(i=0;i<25;i++)
     {
	   move(M[i].r, M[i].c);
	   addch('Y');
      }
    
   /* Display game title,score header,options */
   move(0,(COLS/2)-9);
   addstr("--CENTIPEDES GAME--");
   move (0,1);
   addstr("SCORE: ");
   move(0,COLS-19);
   addstr("m = menu  q = quit");
   
   while(1) {
      /* Show score */
      sprintf(tellscore, "%d", score);
      move(0,8);
      addstr(tellscore);
      
      /* Move tank */
      move(tank.r,tank.c);
      addch(tank.ch);
          
      /* Move shots */
      if (loops % settings.shots == 0)
      for (i=0; i<3; ++i) {
         if (shot[i].active == 1) {
            if (shot[i].r > 0) {
               if (shot[i].r < LINES - 2) {
                  move(shot[i].r + 1,shot[i].c);
                  addch(' ');
               }
               
               for (j=0; j<10; ++j) {
                  if (aliens[j].alive == 1 && aliens[j].r == shot[i].r && aliens[j].c == shot[i].c || aliens[j].alive == 1 && aliens[j].pr == shot[i].r && aliens[j].pc == shot[i].c) {
                     score += 20;
                     aliens[j].alive = 0;
                     shot[i].active = 0;
                     --currentshots;
                     --currentcentipede;
                     move(aliens[j].pr,aliens[j].pc);
                     addch(' ');
                     break;
                  }
               }
               
               if (shot[i].active == 1) {
                  move(shot[i].r,shot[i].c);
                  addch(shot[i].ch);
                  
                  --shot[i].r;
               }
            }
            else {
               shot[i].active = 0;
               --currentshots;
               move(shot[i].r + 1,shot[i].c);
               addch(' ');
            }
         }
      }     
   
      /* Move aliens */
      if (loops % settings.centipede == 0)
      for (i=0; i<10; ++i) {
         if (aliens[i].alive == 1) {
			 for (j=0; j<25; ++j) {
            if ( M[i].active == 1 && M[i].c == aliens[i].c && M[i].r == aliens[i].r){
               ++aliens[i].r;
               if (aliens[i].direction == 'l'){
				aliens[i].direction = 'r';
			   }
			   if (aliens[i].direction == 'r'){
				aliens[i].direction = 'l';
			   }
		   }
	   }
            
            move(aliens[i].pr,aliens[i].pc);
            addch(' ');
            
            move(aliens[i].r,aliens[i].c);
            addch(aliens[i].ch);
            
            aliens[i].pr = aliens[i].r;
            aliens[i].pc = aliens[i].c;
                    
            /* Set alien's next position */
            if (aliens[i].direction == 'l')
               --aliens[i].c;
            else if (aliens[i].direction == 'r')
               ++aliens[i].c;
            
             
               
            /* Check alien's next positions */
            if (aliens[i].c == COLS - 2) {
               ++aliens[i].r;
               aliens[i].direction = 'l';
            }
            else if (aliens[i].c == 0) {
               ++aliens[i].r;
               aliens[i].direction = 'r';
            }
         }
      }
      
      
      /* See if game has been won or lost*/
      if (currentcentipede == 0) {
         win = 1;
         break;
      }
      for (i=0; i<30; ++i) {
         if (aliens[i].r == LINES-1) {
            win = 0;
            break;
         }
      }
        
      move(0,COLS-1);
      refresh();
      usleep(DELAY);
      ++loops;
      
      input = getch();
      move(tank.r,tank.c);
      addch(' ');
      
      /* Check input */
      if (input == 'q')
         win = 2;
      else if (input == KEY_LEFT)
         --tank.c;
      else if (input == KEY_RIGHT)
         ++tank.c;
      else if (input == ' ' && currentshots < 3) {
         for (i=0; i<3; ++i) {
            if (shot[i].active == 0) {
               shot[i].active = 1;
               ++currentshots;
               --score;
               shot[i].r = LINES - 2;
               shot[i].c = tank.c;
               break;
            }
         }
      }
      else if (input == 'm')
         menu(&settings); 
      
      if (win != -1)
         break;
      
      /* Check for valid tank position */
      if (tank.c > COLS-2)
         tank.c = COLS - 2;
      else if (tank.c < 0)
         tank.c = 0;     
   }
   
   gameover(win);
   endwin();
   return 0;
}

/* This function handles the menu options available to the user */
void menu(struct options *settings) {
   char option, buf[30];
   int new;

   clear();
   echo();
   nocbreak();
   nodelay(stdscr,0);

   move(0,0);
   addstr("1. Change overall game speed");
   move(1,0);
   addstr("2. Change alien motion speed");
   move(2,0);
   addstr("3. Change tank shot speed");
   move(3,0);
   addstr("4. Return to the game");
   move(4,0);
   addstr("5. Exit the game");
   move(5,0);
   addstr("Enter your option: ");
   refresh();
   
   while(1) {
      move(8,19);
      option = getch();
      move(9,0);
      deleteln();
      move(10,0);
      deleteln();
      move(11,0);
      deleteln();
      
      if (option == '1') {
         sprintf(buf,"Current value: %d\n", settings->overall);
         
         move(9,0);
         addstr(buf);
         move(10,0);
         addstr("Enter new value: ");
         move(10,17);
         refresh();
         getch();
         getstr(buf);
         
         new = atoi(buf);
         
         /* Check for valid new value */
         if (new < 0) {
            move(11,0);
            addstr("ERROR: Inalid value");
         }
         else {
            settings->overall = new;
         }  
      }
      else if (option == '2') {
         sprintf(buf,"Current value: %d\n", settings->centipede);
         
         move(9,0);
         addstr(buf);
         move(10,0);
         addstr("Enter new value: ");
         move(10,17);
         refresh();
         getch();
         getstr(buf);
         
         new = atoi(buf);
         
         /* Check for valid new value */
         if (new <= 0) {
            move(11,0);
            addstr("ERROR: Inalid value");
         }
         else {
            settings->centipede = new;
         }  
      }
      else if (option == '3') {
         sprintf(buf,"Current value: %d\n", settings->shots);
         
         move(9,0);
         addstr(buf);
         move(10,0);
         addstr("Enter new value: ");
         move(10,17);
         refresh();
         getch();
         getstr(buf);
         
         new = atoi(buf);
         
         /* Check for valid new value */
         if (new <= 0) {
            move(11,0);
            addstr("ERROR: Inalid value");
         }
         else {
            settings->shots = new;
         }  
      }
      else if (option == '4') {
         break;
      }
      else if (option == '5') {
         endwin();
         exit(0);
      }
      else {
         move(9,0);
         addstr("ERROR: Invalid selection");
         move(8,19);
         addstr(" ");
         refresh();        
      }
   }
   
   clear();
   noecho();
   cbreak();
   nodelay(stdscr,1);
   
   move(0,(COLS/2)-9);
   addstr("--SPACE INVADERS--");
   move (0,1);
   addstr("SCORE: ");
}

/* This function handles displaying the win/lose screen */
void gameover(int win) {

   nodelay(stdscr, 0);
   
   if (win == 0) {
      clear();
      move((LINES/2)-1,(COLS/2)-5);
      addstr("YOU LOSE!");
      move((LINES/2),(COLS/2)-11);
      addstr("PRESS ANY KEY TO EXIT");
      move(0,COLS-1);
      refresh();
      getch();
   }
   
   else if (win == 1) {
      clear();
      move((LINES/2)-1,(COLS/2)-5);
      addstr("YOU WIN!");
      move((LINES/2),(COLS/2)-11);
      addstr("PRESS ANY KEY TO EXIT");
      move(0,COLS-1);
      refresh();
      getch();
   }
}
