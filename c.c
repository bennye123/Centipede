#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>

#define DELAY 50000

//player object
struct player {
	int row,column;
	char character;
};
// Mushroom Object
struct mushroom {
	int row,column;
	int active; 
};
//Centipede Object
struct centipede {
	int row,column;
	int pr,pc;
	int alive; 
	char direction; 
	char character;
};
//Shoot Object
struct shoot {
	int row, column;
	int active; 
	char character;
};

//declaring all variables
int main() {
   struct player hero;
   struct centipede cent[10];
   struct shoot shot[3];
   struct mushroom M[25];
   unsigned int input; 
   int loops=0;
   int i=0;
   int j=0;
   int currentshots=0;
   int currentcentipede=10;
   int random=0;  
   int score=0;  
   int win=-1;
   char showscore[30];

   initscr();
   clear();
   noecho();
   cbreak();
   nodelay(stdscr,1);
   keypad(stdscr,1);
   srand(time(NULL));
   


  //hero character and movement
   hero.row = LINES - 1;
   hero.column = COLS / 2;
   hero.character = '^';

   //centipede movement
   for (i=0; i<10; ++i) {
      cent[i].row = 1;
      cent[i].column = i*2;
      cent[i].pr = 0;
      cent[i].pc = 0;
      cent[i].character = 'o';
      cent[i].alive = 1;
      cent[i].direction = 'r';
   }
    
	// Bullets Movement
   for (i=0; i<3; ++i) {
      shot[i].active = 0;
      shot[i].character = '.';
   }
   
   //mushrooms to be randomnly placed throughout the interface
   for(i=0;i<25;i++)
   {
	   M[i].active = 1;
	   M[i].column = rand() % COLS + 2;
       M[i].row = rand() % (LINES-3) + 2;
   }
   
    //Mushroom being generated
       for(i=0;i<25;i++)
     {
	   move(M[i].row, M[i].column);
	   addch('+');
      }
    
	//Displaying the game name and score
   move(0,(COLS/2)-9);
   addstr("--CENTIPEDES GAME--");
   move (0,1);
   addstr("SCORE: ");
   
   while(1) {
      //This is to show the score
      sprintf(showscore, "%d", score);
      move(0,8);
      addstr(showscore);
      
      //This is to move your hero/shooter
      move(hero.row,hero.column);
      addch(hero.character);
      
      if(currentcentipede == 0){
	    move((LINES/2),(COLS/2)-10);
		addstr("You win!!!!!!!!!!!!");
	  }

          
     // This is for the bullets that are moving upo the screen when shot at centipede
      for (i=0; i<3; ++i) {
         if (shot[i].active == 1) {
            if (shot[i].row > 0) {
               if (shot[i].row < LINES - 2) {
                  move(shot[i].row + 1,shot[i].column);
                  addch(' ');
               }
               
               for (j=0; j<3; ++j) {
                  if (cent[j].alive == 1 && cent[j].row == shot[i].row && cent[j].column == shot[i].column || cent[j].alive == 1 && cent[j].pr == shot[i].row && cent[j].pc == shot[i].column) {
                     score += 30;
                     cent[j].alive = 0;
                     shot[i].active = 0;
                     --currentshots;
                     --currentcentipede;
                     move(cent[j].pr,cent[j].pc);
                     addch(' ');
                     
      
                     break;
                  }
               }
               
               if (shot[i].active == 1) {
                  move(shot[i].row,shot[i].column);
                  addch(shot[i].character);
                  
                  --shot[i].row;
               }
            }
            else {
               shot[i].active = 0;
               --currentshots;
               move(shot[i].row + 1,shot[i].column);
               addch(' ');
            }
         }
      }     
   
      //Centipede moving side to side and down the screen
      for (i=0; i<10; ++i) {
         if (cent[i].alive == 1) {
			 for (j=0; j<25; ++j) {
            if ( M[i].active == 1 && M[i].column == cent[i].column && M[i].row == cent[i].row){
               ++cent[i].row;
               if (cent[i].direction == 'l'){
				cent[i].direction = 'r';
			   }
			   if (cent[i].direction == 'r'){
				cent[i].direction = 'l';
			   }
		   }
	   }
            
            move(cent[i].pr,cent[i].pc);
            addch(' ');
            
            move(cent[i].row,cent[i].column);
            addch(cent[i].character);
            
            cent[i].pr = cent[i].row;
            cent[i].pc = cent[i].column;
                    
            //Placing where the centipede starts
            if (cent[i].direction == 'l')
               --cent[i].column;
            else if (cent[i].direction == 'r')
               ++cent[i].column;
            
             
               
            //Checking the centipedes positions to see if it hits the side
            if (cent[i].column == COLS - 2) {
               ++cent[i].row;
               cent[i].direction = 'l';
            }
            else if (cent[i].column == 0) {
               ++cent[i].row;
               cent[i].direction = 'r';
            }
         }
      }
      
      //How fast the game is moving  
      move(0,COLS-1);
      refresh();
      usleep(DELAY);
      ++loops;
      
      input = getch();
      move(hero.row,hero.column);
      addch(' ');
      
      //This is where you click on right and left button to control your shooter
      if (input == 'q')
         win = 2;
      else if (input == KEY_LEFT)
         --hero.column;
      else if (input == KEY_RIGHT)
         ++hero.column;
      else if (input == ' ' && currentshots < 7) {
         for (i=0; i<3; ++i) {
            if (shot[i].active == 0) {
               shot[i].active = 1;
               ++currentshots;
               --score;
               shot[i].row = LINES - 2;
               shot[i].column = hero.column;
               break;
            }
         }
      }
      //Placing the shooter position
      if (hero.column > COLS-2)
         hero.column = COLS - 2;
      else if (hero.column < 0)
         hero.column = 0;     
   }
   
   return 0;
}

