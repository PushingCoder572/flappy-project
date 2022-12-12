#include <stdint.h>
#include "pic32mx.h"
#include "header.h"

uint8_t name[3] = {0, 0, 0};
uint8_t name_index = 0;
uint8_t prev3_signal = 0;
uint8_t btn3_signal = 0;
uint8_t prev4_signal = 0;
uint8_t btn4_signal = 0;
uint8_t prev5_signal = 0;
uint8_t btn5_signal = 0;

void draw_name(uint8_t x, uint8_t y, uint8_t *name, uint8_t screen[128][32], uint8_t under_line) {
    int i, j;
    for (i = 0; i < 3; i++) {
        draw_letter(x, y + i * 8, name[i], screen);
        if (i == name_index && under_line) {
            draw_block(screen, x + 9, y + 2 + i * 8, 2, 6);
        }
    }
}

void update_name() {
    name[name_index] = (name[name_index] + 1) % 26;
}

void run_name(Game *game) {
    btn3_signal = getbtns() & 0x2;
    btn4_signal = getbtns() & 0x4;
    btn5_signal = getbtns() & 1;
    if (btn3_signal != prev3_signal && btn3_signal) {
        update_name();
    }
    if (btn4_signal != prev4_signal && btn4_signal) {
        name_index = (name_index + 1) % 3;
    }
    if (btn5_signal != prev5_signal && btn5_signal) {
        game->name[0] = name[0];
        game->name[1] = name[1];
        game->name[2] = name[2];
        game->name_select = 0;
        set_highscore(game);
    }

    clear_screen(game->screen, game->screen_matrix);
    draw_name(56, 4, name, game->screen_matrix, 1);
    matrix_to_display(game->screen_matrix, game->screen);
    draw_screen(game->screen);
    prev3_signal = btn3_signal;
    prev4_signal = btn4_signal;
    prev5_signal = btn5_signal;
}

void save_name(Game *game) {
    int i;
    for (i = 0; i < 3; i++) {
        game->name[i] = name[i];
    }
}
