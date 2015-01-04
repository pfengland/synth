#include "sequence_view.h"
#include "font.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

void draw_digit(int num, framebuffer *fb, int x, int y) {

    assert(num >= 0 && num <= 9);

    // get offset of number to draw
    int font_offset = font_width*font_height * num;

    for (int fx=0; fx<font_width; fx++) {

	for (int fy=0; fy<font_height; fy++) {

	    int font_index = font_offset + (fy * font_width) + fx;
	    char font_pixel = font_bits[font_index];
	    int color = 255 * font_pixel;
	    framebuffer_putpixel(fb, x+fx, y+fy, color, color, color);
	}
    }
}

void draw_number_str(const char *str, framebuffer *fb, int x, int y) {
    // get the current char
    int i=0;
    while (1) {
	char c = str[i];
	if (c == '\0')
	    break;
	int digit = c - '0';
	draw_digit(digit, fb, x + i*font_width, y);
	i++;
    }
}

sequence_view* sequence_view_new(sequence *seq) {
    sequence_view *sv = malloc(sizeof(*sv));
    sv->seq = seq;
    return sv;
}

void sequence_view_draw(sequence_view *sv, framebuffer *fb, int x, int y) {
    // try drawing something with the font
    //    draw_number_str("1983", fb, x, y);

    // draw the sequence
    char str[4];
    for (int step=0; step<sv->seq->length; step++) {
	sprintf(str, "%03d", sv->seq->notes[step]);
	draw_number_str(str, fb, 0, step*font_height);
    }
}

void sequence_view_free(sequence_view *sv) {
    free(sv);
}

