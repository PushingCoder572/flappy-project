#include "pic32mx.h"
#include "header.h"
#include <stdint.h>

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

void draw_text_block(uint8_t screen[128][32], uint8_t x, int16_t y, const uint8_t text[32][32]) {
    int i, j;
    for (i = 0; i < 32; i++) {
        for (j = 0; j < 32; j++) {
            screen[i + x][j + y] = text[i][31 - j];
        }
    }
}

void draw_object(uint8_t screen[128][32], uint8_t x, uint8_t y, uint8_t num, uint8_t border) {
    int i, j;
    if (border) {
        for (i = 0; i < 14; i++) {
            for (j = 0; j < 10; j++) {
                screen[i + x - 1][j + y - 1] = 1;
            }
        }
    }

    uint8_t start = num * 12;
    for (i = start; i < start + 12; i++) {
        for (j = 0; j < 8; j++) {
            screen[i - start + x][j + y] = numbers[i][7 - j];
        }
    }
}

void matrix_to_display(uint8_t matrix[128][32], uint8_t display[512]) {
    int i, j;

    for (i = 0; i < 128; i++) {
        for (j = 0; j < 32; j++) {
            if (matrix[i][j] == 1) {
                display[i + (j / 8) * 128] |= 1 << (j % 8);
            }
        }
    }
}

void draw_number(uint8_t screen[128][32], uint8_t number) {
    if (number > 99) {
        number = 99;
    }
    if (number / 10 < 1) {
        draw_object(screen, 4, 12, number, 1);
    } else {
        draw_object(screen, 4, 16, number / 10, 1);
        draw_object(screen, 4, 8, number % 10, 1);
    } 
}

void draw_highscore(uint8_t screen[128][32], uint8_t highscore) {
    if (highscore > 99) {
        highscore = 99;
    }
    if (highscore / 10 < 1) {
        draw_object(screen, 68, 12, highscore, 0);
    } else {
        draw_object(screen, 68, 16, highscore / 10, 0);
        draw_object(screen, 68, 8, highscore % 10, 0);
    } 
}

void draw_bird(uint8_t screen[128][32], int16_t x, uint8_t *game_ended) {
    draw_with_col(screen, x, 12, 8, 8, game_ended);
}

void draw_pipe(uint8_t screen[128][32], Pipe *pipe) {
    draw_block(screen, 0, pipe->y, pipe->gap_position, 8);
    draw_block(screen, pipe->gap_position + pipe->gap_size, pipe->y, 128 - pipe->gap_position - pipe->gap_size, 8);
}

void draw_block(uint8_t screen[128][32], uint8_t x, int16_t y, uint8_t width, uint8_t height) {
    int i, j;
    for (i = 0; i < 128; i++) {
        for (j = 0; j < 32; j++) {
            if (i >= x && i < (x + width) && j >= y && j < (y + height)) {
                screen[i][j] = 1;
            }
        }
    }
}

void draw_with_col(uint8_t screen[128][32], uint8_t x, int16_t y, uint8_t width, uint8_t height, uint8_t *game_ended) {
    int i, j;
    for (i = 0; i < 128; i++) {
        for (j = 0; j < 32; j++) {
            if (i >= x && i < (x + width) && j >= y && j < (y + height)) {
                if (screen[i][j] == 1) {
                    *game_ended = 1;
                }
                screen[i][j] = 1;
            }
        }
    }
}

void clear_display(uint8_t screen[128][32]) {
    int i, j;
    for (i = 0; i < 128; i++) {
        for (j = 0; j < 32; j++) {
            screen[i][j] = 0;
        }
    }
}

void draw_screen(uint8_t *data) {
  int i, j;
  for (i = 0; i < 4; i++) {
    DISPLAY_CHANGE_TO_COMMAND_MODE;
    spi_send_recv(0x22);
	spi_send_recv(i);
    spi_send_recv(0x00);
    spi_send_recv(0x10);
    DISPLAY_CHANGE_TO_DATA_MODE;
    for (j = 0; j < 128; j++) {
      spi_send_recv(data[j + i * 128]);
    }
  }
}

void clear_screen(uint8_t display[512], uint8_t screen[128][32]) {
    int i, j;
    for (i = 0; i < 512; i++) {
        display[i] = 0;
    }

    for (i = 0; i < 128; i++) {
        for (j = 0; j < 32; j++) {
            screen[i][j] = 0;
        }
    }
}

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

void draw_menu_block(uint8_t screen[128][32]) {
    int i, j;
    for (i = 0; i < 128; i++) {
        for (j = 0; j < 32; j++) {
            screen[i][j] = menu[i][31 - j];
        }
    }
}

void draw_menu(Game *game) {
    if (getbtns() & 1) {
        game->game_over = 0;
        return;
    }
    clear_screen(game->screen, game->screen_matrix);
    draw_menu_block(game->screen_matrix);
    draw_highscore(game->screen_matrix, game->highscore);
    matrix_to_display(game->screen_matrix, game->screen);
    draw_screen(game->screen);
}

void display_init(void) {
  DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);
	
	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);
	
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);
	
	spi_send_recv(0xAF);
}