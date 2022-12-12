#include "pic32mx.h"
#include "header.h"
#include <stdint.h>

uint8_t name[3];
uint8_t score;
uint8_t page = 0;
uint8_t prev2_signal = 0;
uint8_t btn2_signal = 0;

void set_highscore(Game *game) {
    game->highscores[0][0] = game->score;
    game->highscores[0][1] = game->name[0];
    game->highscores[0][2] = game->name[1];
    game->highscores[0][3] = game->name[2];
}

void run_highscore(Game *game) {
    clear_screen(game->screen, game->screen_matrix);
    btn2_signal = getbtns() & 1;
    if (btn2_signal != prev2_signal && btn2_signal) {
        page = (page + 1) % 4;
    }

    int i;
    prev2_signal = btn2_signal;
    for (i = 4 * page; i < 4 * (page + 1); i++) {
        if (game->highscores[i][0] == 0) {
            break;
        }
        score = game->highscores[i][0];
        name[0] = game->highscores[i][1];
        name[1] = game->highscores[i][2];
        name[2] = game->highscores[i][3];

        draw_highscore_line(10 + (i % 4) * 26, score, name, game->screen_matrix);
    }
    matrix_to_display(game->screen_matrix, game->screen);
    draw_screen(game->screen);
}