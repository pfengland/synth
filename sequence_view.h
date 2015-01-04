#ifndef H_SEQUENCE_VIEW
#define H_SEQUENCE_VIEW

#include "sequence.h"
#include "framebuffer.h"

typedef struct sequence_view_s sequence_view;
struct sequence_view_s {
    sequence *seq;
    framebuffer *fb;
    int current_step;
    int current_char;
};

sequence_view* sequence_view_new(sequence *seq);
void sequence_view_draw(sequence_view *sv, framebuffer *fb, int x, int y);
void sequence_view_free(sequence_view *sv);

#endif
