#include "sequence_view.h"
#include "font.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

void putpixel(SDL_Surface *surface, 
	      int x, int y, 
	      char r, char g, char b) {

    int offset = (x * 4) + (y * surface->pitch);
    char *buf = surface->pixels;
    buf[offset] = r;
    buf[offset+1] = g;
    buf[offset+2] = b;
}

void draw_digit(int num, SDL_Surface *surface, int x, int y, 
		int fgr, int fgg, int fgb,
		int bgr, int bgg, int bgb) {

    assert(num >= 0 && num <= 9);

    // get offset of number to draw
    int font_offset = font_width*font_height * num;

    for (int fx=0; fx<font_width; fx++) {

	for (int fy=0; fy<font_height; fy++) {

	    int font_index = font_offset + (fy * font_width) + fx;
	    char font_pixel = font_bits[font_index];
	    if (font_pixel)
		putpixel(surface, x+fx, y+fy, bgr, bgg, bgb);
	    else
		putpixel(surface, x+fx, y+fy, fgr, fgg, fgb);
	}
    }
}

void draw_step(sequence_view *sv, int step, SDL_Surface *surface, int x, int y) {

    char str[4];
    sprintf(str, "%03d", sv->seq->notes[step]);

    // get the current char
    int i=0;
    while (1) {
	char c = str[i];
	if (c == '\0')
	    break;
	int digit = c - '0';

	if (step == sv->current_step) {
	    if (i == sv->current_char) {
		// current char color
		draw_digit(digit, surface, x + i*font_width, y, 200,100,0, 0,100,200);
	    } else {
		// current step color
		draw_digit(digit, surface, x + i*font_width, y, 0,100,200, 200,100,0);
	    }
	} else {
	    // regular color
	    draw_digit(digit, surface, x + i*font_width, y, 0,0,0, 255,255,255);
	}
	i++;
    }
}

sequence_view* sequence_view_new(sequence *seq) {
    sequence_view *sv = malloc(sizeof(*sv));
    sv->seq = seq;
    sv->current_step = 0;
    sv->current_char = 0;
    return sv;
}

void sequence_view_draw(sequence_view *sv, SDL_Surface *screen, int x, int y) {

    // draw the sequence
    for (int step=0; step<sv->seq->length; step++) {
	draw_step(sv, step, screen, 0, step * font_height);
    }

    SDL_Flip(screen);
}

void sequence_view_free(sequence_view *sv) {
    free(sv);
}

void sequence_view_cursor_down(sequence_view *sv, SDL_Surface *screen) {
    sv->current_step++;
    if (sv->current_step >= sv->seq->length)
	sv->current_step = 0;
    sequence_view_draw(sv, screen, 0, 0);
}

void sequence_view_cursor_up(sequence_view *sv, SDL_Surface *screen) {
    sv->current_step--;
    if (sv->current_step < 0)
	sv->current_step = sv->seq->length - 1;
    sequence_view_draw(sv, screen, 0, 0);
}

void sequence_view_cursor_left(sequence_view *sv, SDL_Surface *screen) {
    sv->current_char--;
    if (sv->current_char < 0)
	sv->current_char = 2;
    sequence_view_draw(sv, screen, 0, 0);
}

void sequence_view_cursor_right(sequence_view *sv, SDL_Surface *screen) {
    sv->current_char++;
    if (sv->current_char > 2)
	sv->current_char = 0;
    sequence_view_draw(sv, screen, 0, 0);
}

void set_current_char(sequence_view *sv, int val, SDL_Surface *screen) {
    int note = sv->seq->notes[sv->current_step];
    int col[3];
    col[0] = note / 100;
    col[1] = note / 10 % 10;
    col[2] = note % 10;
    col[sv->current_char] = val;
    note = col[0] * 100 + col[1] * 10 + col[2];
    if (note > 127)
	note = 127;
    sv->seq->notes[sv->current_step] = note;
    sequence_view_draw(sv, screen, 0, 0);
}

void sequence_view_input(sequence_view *sv, int val, SDL_Surface *screen) {
    set_current_char(sv, val, screen);
}
