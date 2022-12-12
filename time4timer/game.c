#include "pic32mx.h"
#include "header.h"

uint8_t timeoutcounter = 0;
uint8_t prev_signal = 0;
uint8_t btn_signal = 0;

void user_isr( void ) {
  return;
}

void init_pipe(Pipe *pipe) {
    pipe->y = -7;
    pipe->gap_size = 40;
    pipe->gap_position = 32;
}

void init_bird(Bird *bird) {
    bird->x = 60;
    bird->speed = 0;
}

uint8_t get_random() {
  return (TMR2 % 16) * 4;
}

void move_pipe(Pipe * pipe, uint8_t *score) {
    pipe->y += 1;
    if (pipe->y > 32) {
        pipe->y = -7;
        pipe->gap_position = get_random() + 16;
    }

    if (pipe->y == 12) {
        *score = *score + 1;
    }
}

void move_bird(Bird *bird, uint8_t *game_ended, uint8_t *game_over) {
    bird->speed += GRAVITY;
    bird->speed = min(bird->speed, MAX_SPEED);
    bird->x += bird->speed;
    if (bird->x > 120) {
        bird->x = 120;
        bird->speed = 0;
        *game_over = 1;
    }

    if (bird->x < 0) {
        bird->x = 0;
        bird->speed = 0;
        *game_ended = 1;
    }
}

void jump_bird(Bird *bird) {
    bird->speed = JUMP_SPEED;
}

void start_game(Game *game) {
    game->highscore = 0;
    game->score = 0;
    game->game_over = 1;
    int i, j;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 4; j++) {
            game->highscores[i][j] = 0;
        }
    }
}

void init_game(Game *game) {
    init_pipe(&game->pipe);
    init_bird(&game->bird);

    clear_screen(game->screen, game->screen_matrix);
    game->score = 0;
    game->game_over = 0;
    game->jump_flag = 0;
    game->game_ended = 0;
    game->name_select = 0;

    PR2 = 18549; // 31250 = 80MHz / 256 / 10Hz
    T2CONSET = 0x70; // 0x70 = 0111 0000
    TMR2 = 0; // Reset timer
    T2CONSET = 0x8000; // 0x8000 = 1000 0000 0000 0000

    PR3 = 15625; // 15625 = 80MHz / 256 / 20Hz
    T3CONSET = 0x70; // 0x70 = 0111 0000
    TMR3 = 0; // Reset timer
    T3CONSET = 0x8000; // 0x8000 = 1000 0000 0000 0000
}

void kill_game(Game *game) {
    T2CONCLR = 0x8000; // 0x8000 = 1000 0000 0000 0000
    T3CONCLR = 0x8000; // 0x8000 = 1000 0000 0000 0000

    if (game->score > 0 && game->score > game->highscores[15][0]) {
        game->name_select = 1;
    }

    game->highscore = max(game->highscore, game->score);
}

void run_game(Game *game) {
    if (!game->game_ended) {
        btn_signal = getbtns() & 1;
        if (btn_signal != prev_signal && btn_signal) {
            game->jump_flag = 1;
        }

        if (IFS(0) & 0x1000) {
            IFSCLR(0) = 0x1000;
            clear_screen(game->screen, game->screen_matrix);
            if (game->jump_flag && btn_signal) {
                jump_bird(&game->bird);
                game->jump_flag = 0;
            }
            if (timeoutcounter == 2) {
                move_pipe(&game->pipe, &game->score);
                timeoutcounter = 0;
            }
            draw_pipe(game->screen_matrix, &game->pipe);
            move_bird(&game->bird, &game->game_ended, &game->game_over);
            draw_bird(game->screen_matrix, game->bird.x, &game->game_ended);
            draw_number(game->screen_matrix, game->score);
            matrix_to_display(game->screen_matrix, game->screen);
            draw_screen(game->screen);
            timeoutcounter++;
        }
        prev_signal = btn_signal;
    } else {
        if (IFS(0) & 0x1000) {
        IFSCLR(0) = 0x1000;
        clear_screen(game->screen, game->screen_matrix);
        draw_pipe(game->screen_matrix, &game->pipe);
        move_bird(&game->bird, &game->game_ended, &game->game_over);
        draw_bird(game->screen_matrix, game->bird.x, &game->game_ended);
        draw_number(game->screen_matrix, game->score);
        draw_text_block(game->screen_matrix, 32, 0, game_text);
        draw_text_block(game->screen_matrix, 64, 0, over_text);
        matrix_to_display(game->screen_matrix, game->screen);
        draw_screen(game->screen);
        }
    }
}