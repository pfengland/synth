#include <jack/jack.h>
#include "sequence.h"

typedef struct player_s player;
struct player_s {
    jack_port_t *output_port;
    jack_client_t *client;
    int sample_rate;
    sequence *seq;
    int play_step;
    int samples_per_step;
    int step_sample;
};

player* player_new(void);
void player_set_sequence(player *p, sequence *seq);
void player_init(player *p);
void player_free(player *p);
