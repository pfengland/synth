// to compile run:
// gcc -o synth -ljack -std=gnu99 synth.c

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <jack/jack.h>

jack_port_t *input_port;
jack_port_t *output_port;
jack_client_t *client;

int sample_rate;
int cur_sample = 0;
int freq = 200; // cycles per second
int samples_per_cycle;
int half_cycle;

int calc() {
    samples_per_cycle = sample_rate/freq;
    half_cycle = samples_per_cycle / 2;
}

int process(jack_nframes_t nframes, void *arg) {

    jack_default_audio_sample_t *in, *out;

    //    in = jack_port_get_buffer(input_port, nframes);
    out = jack_port_get_buffer(output_port, nframes);

    for (int i=0; i<nframes; i++) {

	 if (cur_sample > samples_per_cycle) {
	     cur_sample = 0;
	 }

	if (cur_sample < half_cycle) {
	    out[i] = -1;
	} else {
	    out[i] = 1;
	    
	}
	    
	cur_sample++;
    }

    return 0;
}

void jack_shutdown(void *arg) {
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {

    const char **ports;
    const char *client_name = "synth";
    const char *server_name = NULL;
    jack_options_t options = JackNullOption;
    jack_status_t status;

    client = jack_client_open(client_name, options, &status, server_name);
    if (client == NULL) {
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
	client_name = jack_get_client_name(client);
	fprintf(stderr, "unique name %s assigned\n", client_name);
    }

    jack_set_process_callback(client, process, 0);

    jack_on_shutdown(client, jack_shutdown, 0);

    sample_rate = jack_get_sample_rate(client);

    calc();

    printf("engine sample rate: %" PRIu32 "\n", sample_rate);

    input_port = jack_port_register(client, "input", JACK_DEFAULT_AUDIO_TYPE,
				    JackPortIsInput, 0);
    output_port = jack_port_register(client, "output", JACK_DEFAULT_AUDIO_TYPE,
				    JackPortIsOutput, 0);

    if ((input_port == NULL) || (output_port == NULL)) {
	fprintf(stderr, "no more jack ports available\n");
	exit(EXIT_FAILURE);
    }

    if (jack_activate(client)) {
	fprintf(stderr, "can't activate client\n");
	exit(EXIT_FAILURE);
    }

    ports = jack_get_ports (client, NULL, NULL,
			    JackPortIsPhysical|JackPortIsOutput);
    if (ports == NULL) {
	fprintf(stderr, "no physical capture ports\n");
	exit (1);
    }

    if (jack_connect (client, ports[0], jack_port_name (input_port))) {
	fprintf (stderr, "cannot connect input ports\n");
    }

    free (ports);
    
    ports = jack_get_ports (client, NULL, NULL,
			    JackPortIsPhysical|JackPortIsInput);
    if (ports == NULL) {
	fprintf(stderr, "no physical playback ports\n");
	exit (1);
    }

    if (jack_connect (client, jack_port_name (output_port), ports[0])) {
	fprintf (stderr, "cannot connect output ports\n");
    }

    free (ports);

    /* keep running until stopped by the user */

    sleep (-1);

    /* this is never reached but if the program
          had some other way to exit besides being killed,
	     they would be important to call.
    */

    jack_client_close (client);
    exit (0);
}
