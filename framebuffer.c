#include "framebuffer.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>

framebuffer* framebuffer_new(void) {

    framebuffer *fb = malloc(sizeof(*fb));

    // open it
    fb->fd = open("/dev/fb0", O_RDWR);
    if (fb->fd == -1) {
	fprintf(stderr, "frameBuffer_init: error opening the device\n");
	exit(EXIT_FAILURE);
    }

    // get fixed screen info
    if (ioctl(fb->fd, FBIOGET_FSCREENINFO, &fb->finfo)) {
	fprintf(stderr, "frameBuffer_init: error reading fixed screen info\n");
	close(fb->fd);
	exit(EXIT_FAILURE);
    }

    // get variable screen info
    if (ioctl(fb->fd, FBIOGET_VSCREENINFO, &fb->vinfo)) {
	fprintf(stderr, "error reading variable screen info\n");
	close(fb->fd);
	exit(EXIT_FAILURE);
    }
    fprintf(stderr, "display info: %dx%d resolution, %d bits per pixel\n",
	    fb->vinfo.xres, fb->vinfo.yres, fb->vinfo.bits_per_pixel);

    // save it
    memcpy(&fb->orig_vinfo, &fb->vinfo, sizeof(fb->vinfo));

    fb->size = fb->finfo.smem_len;

    fb->buf = (char*)mmap(0, fb->size, 
			  PROT_READ|PROT_WRITE, MAP_SHARED, fb->fd, 0);
    if (fb->buf == (char*)-1) {
	fprintf(stderr, "error memory mapping frame buffer\n");
	close(fb->fd);
	exit(EXIT_FAILURE);
    }

    // turn off the cursor
    write(STDOUT_FILENO, "\033[?25l", 6);

    framebuffer_clear(fb);

    return fb;
}

void framebuffer_free(framebuffer *fb) {

    framebuffer_clear(fb);

    munmap(fb->buf, fb->size);

    // show the cursor again
    write(STDOUT_FILENO, "\033[?2c", 5);

    // enable original settings
    if (ioctl(fb->fd, FBIOPUT_VSCREENINFO, &fb->orig_vinfo)) {
	printf("error restoring screen settings\n");
    }

    close(fb->fd);

    free(fb);
}

void framebuffer_putpixel(framebuffer *fb, 
			  int x, int y, 
			  char r, char g, char b) {
    int offset = x * 4 + y * fb->finfo.line_length;
    fb->buf[offset] = r;
    fb->buf[offset+1] = g;
    fb->buf[offset+2] = b;
}

void framebuffer_fill(framebuffer *fb, char r, char g, char b) {
    int x, y;
    for (y=0; y<(fb->vinfo.yres); y++) {
	for (x=0; x<fb->vinfo.xres; x++) {
	    framebuffer_putpixel(fb, x, y, r, g, b);
	}
    }
}

void framebuffer_clear(framebuffer *fb) {

    framebuffer_fill(fb, 0, 0, 0);
}
