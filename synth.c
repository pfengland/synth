// to compile run:
// gcc -o synth -ljack -std=gnu99 synth.c player.c sequence.c

#include "player.h"
#include <unistd.h>

int main(int argc, char *argv[]) {

    sequence *seq = sequence_new(8, 100);
    seq->notes[0] = 50;
    seq->notes[1] = 0;
    seq->notes[2] = 51;
    seq->notes[3] = 0;
    seq->notes[4] = 52;
    seq->notes[5] = 0;
    seq->notes[6] = 53;
    seq->notes[7] = 0;

    player *p = player_new();
    player_init(p);
    player_set_sequence(p, seq);

    sleep(-1);

}
