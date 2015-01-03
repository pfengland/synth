#include "sequence.h";
#include <stdlib.h>

sequence* sequence_new(length, speed) {
    sequence *seq = malloc(sizeof(*seq));
    seq->length = length;
    seq->speed = speed;
    seq->notes = malloc(length * sizeof(int));
    for (int i=0; i<length; i++)
	seq->notes[i] = 0;
    return seq;
}

void sequence_free(sequence *seq) {
    free(seq->notes);
    free(seq);
}
