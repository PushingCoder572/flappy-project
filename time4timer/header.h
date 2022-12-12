/* mipslab.h
   Header file for all labs.
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   Latest update 2015-08-28 by F Lundevall

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */

/* Constants */
#define GRAVITY 1
#define MAX_SPEED 4
#define JUMP_SPEED -5

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b; })


/* Game structs */
typedef struct pipe {
   int16_t y;
   uint8_t gap_size;
   uint8_t gap_position;
} Pipe;

typedef struct bird {
   int16_t x;
   int16_t speed;
} Bird;

typedef struct game {
   Pipe pipe;
   Bird bird;
   uint8_t score;
   uint8_t game_over;
   uint8_t jump_flag;
   uint8_t screen_matrix[128][32];
   uint8_t screen[512];
   uint8_t highscore;
   uint8_t game_ended;
   uint8_t name[3];
   uint8_t highscores[16][4];
   uint8_t name_select;
} Game;

/* Declare display-related functions from mipslabfunc.c */
void display_image(int x, uint8_t data[512]);
void display_string(int line, char *s);
void display_update(void);

/* Game stuff */
void init_game(Game *game);
void run_game(Game *game);
void kill_game(Game *game);
void start_game(Game *game);

/* Name Select */
void run_name(Game *game);
void draw_name(uint8_t x, uint8_t y, uint8_t *name, uint8_t screen[128][32], uint8_t under_line);
/* Highscore */
void run_highscore(Game *game);
void set_highscore(Game *game);

/* Graphics */
void draw_screen(uint8_t *data);
void draw_with_col(uint8_t screen[128][32], uint8_t x, int16_t y, uint8_t width, uint8_t height, uint8_t *game_over);
void draw_block(uint8_t screen[128][32], uint8_t x, int16_t y, uint8_t width, uint8_t height);
void draw_pipe(uint8_t screen[128][32], Pipe *pipe);
void draw_bird(uint8_t screen[128][32], int16_t x, uint8_t *game_over);
void clear_screen(uint8_t display[512], uint8_t screen[128][32]);
void matrix_to_display(uint8_t matrix[128][32], uint8_t display[512]);
void draw_number(uint8_t screen[128][32], uint8_t number);
void draw_bird(uint8_t screen[128][32], int16_t x, uint8_t *game_over);
void draw_text_block(uint8_t screen[128][32], uint8_t x, int16_t y, const uint8_t text[32][32]);
void display_init(void);
uint8_t spi_send_recv(uint8_t data);
void draw_menu(Game *game);
void draw_letter(uint8_t x, uint8_t y, char letter, uint8_t screen[128][32]);
void draw_highscore_line(uint8_t x_pos, uint8_t highscore, uint8_t *name, uint8_t screen[128][32]);
void quicksleep(int cyc);
/* Declare lab-related functions from mipslabfunc.c */

/* Declare display_debug - a function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/

/* Declare bitmap array containing font */
extern const uint8_t numbers[][8];
extern const uint8_t game_text[32][32];
extern const uint8_t over_text[32][32];
extern uint8_t menu[][32];
extern uint8_t letters[][26];

/* Declare text buffer for display output */

/* Declare functions written by students.
   Note: Since we declare these functions here,
   students must define their functions with the exact types
   specified in the laboratory instructions. */
/* Written as part of asm lab: delay, time2string */
/* Written as part of i/o lab: getbtns, getsw, enable_interrupt */
int getbtns(void);
int getsw(void);
void enable_interrupt(void);
