// to compile run:
// gcc -o synth -g -ljack -lSDL -std=gnu99 synth.c player.c sequence.c framebuffer.c sequence_view.c

#include "sequence.h"
#include "player.h"
#include "sequence_view.h"
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <SDL/SDL.h>

// globals for cleanup on signal
player *Player = NULL;
sequence *Seq = NULL;
sequence_view *SV = NULL;

void handle_signal(int signum) {

    printf("shutting down\n");

    sequence_view_free(SV);
    player_free(Player);
    sequence_free(Seq);

    SDL_Quit();

    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Surface* screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);

    // create the sequence
    sequence *seq = Seq = sequence_new(8, 100);
    seq->notes[0] = 50;
    seq->notes[1] = 0;
    seq->notes[2] = 51;
    seq->notes[3] = 0;
    seq->notes[4] = 52;
    seq->notes[5] = 0;
    seq->notes[6] = 53;
    seq->notes[7] = 0;

    // create the player
    player *p = Player = player_new();
    player_init(p);
    player_set_sequence(p, seq);

    // set up the signal handler
    struct sigaction action;
    sigaction(SIGTERM, NULL, &action);
    action.sa_handler = handle_signal;
    sigaction(SIGTERM, &action, NULL);

    //    framebuffer_fill(fb, 33,47,128);
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,100,100));

    sequence_view *sv = SV = sequence_view_new(seq);
    sequence_view_draw(sv, screen, 0, 0);

    SDL_Flip(screen);

    p->playing = 1;

    SDL_Event event;
    while (1) {
	if (SDL_PollEvent(&event)) {
	    if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
		case SDLK_UP:
		    sequence_view_cursor_up(sv, screen);
		    break;
		case SDLK_DOWN:
		    sequence_view_cursor_down(sv, screen);
		    break;
		case SDLK_LEFT:
		    sequence_view_cursor_left(sv, screen);
		    break;
		case SDLK_RIGHT:
		    sequence_view_cursor_right(sv, screen);
		    break;
		case SDLK_0:
		    sequence_view_input(sv, 0, screen);
		    break;
		case SDLK_1:
		    sequence_view_input(sv, 1, screen);
		    break;
		case SDLK_2:
		    sequence_view_input(sv, 2, screen);
		    break;
		case SDLK_3:
		    sequence_view_input(sv, 3, screen);
		    break;
		case SDLK_4:
		    sequence_view_input(sv, 4, screen);
		    break;
		case SDLK_5:
		    sequence_view_input(sv, 5, screen);
		    break;
		case SDLK_6:
		    sequence_view_input(sv, 6, screen);
		    break;
		case SDLK_7:
		    sequence_view_input(sv, 7, screen);
		    break;
		case SDLK_8:
		    sequence_view_input(sv, 8, screen);
		    break;
		case SDLK_9:
		    sequence_view_input(sv, 9, screen);
		    break;
		}
	    } else if (event.type == SDL_QUIT) {
		handle_signal(0);
	    }
	}
    }
}
