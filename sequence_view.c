#include "sequence_view.h"
#include "font.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

void draw_digit(int num, framebuffer *fb, int x, int y, 
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
		framebuffer_putpixel(fb, x+fx, y+fy, bgr, bgg, bgb);
	    else
		framebuffer_putpixel(fb, x+fx, y+fy, fgr, fgg, fgb);
	}
    }
}

void draw_step(sequence_view *sv, int step, framebuffer *fb, int x, int y) {

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
		draw_digit(digit, fb, x + i*font_width, y, 200,100,0, 0,100,200);
	    } else {
		// current step color
		draw_digit(digit, fb, x + i*font_width, y, 0,100,200, 200,100,0);
	    }
	} else {
	    // regular color
	    draw_digit(digit, fb, x + i*font_width, y, 0,0,0, 255,255,255);
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

void sequence_view_draw(sequence_view *sv, framebuffer *fb, int x, int y) {

    // draw the sequence
    for (int step=0; step<sv->seq->length; step++) {
	draw_step(sv, step, fb, 0, step * font_height);
    }
}

void sequence_view_free(sequence_view *sv) {
    free(sv);
}

