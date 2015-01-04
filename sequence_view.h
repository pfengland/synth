#ifndef H_SEQUENCE_VIEW
#define H_SEQUENCE_VIEW

#include "sequence.h"
#include "SDL/SDL.h"

typedef struct sequence_view_s sequence_view;
struct sequence_view_s {
    sequence *seq;
    int current_step;
    int current_char;
};

sequence_view* sequence_view_new(sequence *seq);
void sequence_view_draw(sequence_view *sv, SDL_Surface *surface, int x, int y);
void sequence_view_free(sequence_view *sv);
void sequence_view_cursor_up(sequence_view *sv, SDL_Surface *screen);
void sequence_view_cursor_down(sequence_view *sv, SDL_Surface *screen);
void sequence_view_cursor_left(sequence_view *sv, SDL_Surface *screen);
void sequence_view_cursor_right(sequence_view *sv, SDL_Surface *screen);
void sequence_view_input(sequence_view *sv, int val, SDL_Surface *screen);

#endif
