#include "player.h"
#include <stdlib.h>

player *P = NULL;

void calculate() {
    double samples_per_ms = P->sample_rate / 1000;
    P->samples_per_step = samples_per_ms * P->seq->speed;
}

int process(jack_nframes_t nframes, void *arg) {

    jack_default_audio_sample_t *out;

    out = jack_port_get_buffer(P->output_port, nframes);

    for (int i=0; i<nframes; i++) {

	if (P->seq) {
	    // update step
	    if (P->step_sample >= P->samples_per_step) {
		P->play_step++;
	    }
	    // loop
	    if (P->play_step >= P->seq->length) {
		P->play_step = 0;
	    }
	    
	} else {
	    out[i] = 0;
	}
    }

    return 0;
}

void jack_shutdown(void *arg) {
    exit(EXIT_SUCCESS);
}

void player_set_sequence(player *p, sequence *seq) {
    p->seq = seq;
    calculate();
}

player* player_new(void) {
    player *p = malloc(sizeof(*p));
    p->output_port = NULL;
    p->client = NULL;
    p->sample_rate = 0;
    p->seq = NULL;
    p->samples_per_step = 0;
    p->step_sample = 0;
}

void player_init(player *p) {

    const char *client_name = "synth";
    const char *server_name = NULL;
    jack_options_t options = JackNullOption;
    jack_status_t status;

    // set global player for process callback
    P = p;

    p->client = jack_client_open(client_name, options, &status, server_name);

    if (p->client == NULL) {
	fprintf(stderr, "jack_client_open failed, status = 0x%2.0x\n", status);
	if (status & JackServerFailed) {
	    fprintf(stderr, "unable to connect to jack server\n");
	}
	exit(EXIT_FAILURE);
    }
    if (status & JackServerStarted) {
	fprintf(stderr, "jack server started");
    }
    if (status & JackNameNotUnique) {
	client_name = jack_get_client_name(p->client);
	fprintf(stderr, "unique name %s assigned\n", client_name);
    }

    jack_set_process_callback(p->client, process, 0);

    jack_on_shutdown(p->client, jack_shutdown, 0);

    p->sample_rate = jack_get_sample_rate(p->client);
}

void player_free(player *p) {
    free(p);
}
