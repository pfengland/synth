#include "player.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

player *P = NULL;
double note_freqs[128];

void calc_freqs() {
    for (int i=0; i<128; i++) {
	note_freqs[i] = 440 * pow(2.0, (i - 69)/12.0);
	printf("note %d freq %f\n", i, note_freqs[i]);
    }
}

void calculate() {
    double samples_per_ms = P->sample_rate / 1000;
    P->samples_per_step = samples_per_ms * P->seq->speed;
}

int process(jack_nframes_t nframes, void *arg) {

    jack_default_audio_sample_t *out;

    // get the output buffer pointer
    out = jack_port_get_buffer(P->output_port, nframes);

    // calculate each sample for the current buffer
    for (int i=0; i<nframes; i++) {

	// do we have a sequence and are we playing?
	if (P->seq && P->playing) {

	    // update step
	    if (P->step_sample >= P->samples_per_step) {
		P->step_sample = 0;
		P->play_step++;
		printf("play step %d\n", P->play_step);
	    }

	    // end of sequence - loop if set
	    if (P->play_step >= P->seq->length) {
		P->play_step = 0;
		if (!P->loop) {
		    P->playing = 0;
		    out[i] = 0;
		    continue;
		}
	    }

	    // got our current step
	    // get our frequency
	    int note;
	    if ((note = P->seq->notes[P->play_step]) == 0) {
		out[i] = 0;
	    } else {
		// calc samples per cycle
		double freq = note_freqs[note];
		int samples_per_cycle = (double)P->sample_rate/freq;
		int half_cycle = samples_per_cycle / 2;

		int cycle_sample = P->step_sample % samples_per_cycle;

		// square wave
		if (cycle_sample < half_cycle)
		    out[i] = -0.1;
		else
		    out[i] = 0.1;
	    }

	    P->step_sample++;

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
    p->loop = 1;
    p->playing = 0;
}

void player_init(player *p) {

    const char *client_name = "synth";
    const char *server_name = NULL;
    jack_options_t options = JackNullOption;
    jack_status_t status;

    // set global player for process callback
    P = p;

    calc_freqs();

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

    printf("engine sample rate: %" PRIu32 "\n", p->sample_rate);

    p->output_port = jack_port_register(p->client, "output", JACK_DEFAULT_AUDIO_TYPE,
					JackPortIsOutput, 0);

    if (p->output_port == NULL) {
	fprintf(stderr, "no more jack ports available\n");
	exit(EXIT_FAILURE);
    }

    if (jack_activate(p->client)) {
	fprintf(stderr, "can't activate client\n");
	exit(EXIT_FAILURE);
    }

    const char **ports = jack_get_ports (p->client, NULL, NULL,
					 JackPortIsPhysical|JackPortIsInput);
    if (ports == NULL) {
	fprintf(stderr, "no physical playback ports\n");
	exit (1);
    }

    if (jack_connect(p->client, jack_port_name(p->output_port), ports[0])) {
	fprintf (stderr, "cannot connect output ports\n");
    }

    free (ports);
}

void player_free(player *p) {
    free(p);
}
