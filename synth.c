// to compile run:
// gcc -o synth -g -ljack -std=gnu99 synth.c player.c sequence.c framebuffer.c sequence_view.c

#include "sequence.h"
#include "player.h"
#include "framebuffer.h"
#include "sequence_view.h"
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

// globals for cleanup on signal
framebuffer *FB = NULL;
player *Player = NULL;
sequence *Seq = NULL;
sequence_view *SV = NULL;

void handle_signal(int signum) {
    sequence_view_free(SV);
    player_free(Player);
    sequence_free(Seq);
    framebuffer_free(FB);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {

    // create the framebuffer
    framebuffer *fb = FB = framebuffer_new();

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

    framebuffer_fill(fb, 33,47,128);

    sequence_view *sv = SV = sequence_view_new(seq);
    sequence_view_draw(sv, fb, 0, 0);

    p->playing = 1;

    // run forever
    sleep(-1);
}
