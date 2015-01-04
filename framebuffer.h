#ifndef H_FRAMEBUFFER
#define H_FRAMEBUFFER

#include <linux/fb.h>

typedef struct framebuffer_s framebuffer;
struct framebuffer_s {
    int fd;
    struct fb_var_screeninfo orig_vinfo;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long size;
    char *buf;
};

framebuffer* framebuffer_new(void);
void framebuffer_free(framebuffer *fb);
void framebuffer_putpixel(framebuffer *fb, 
			  int x, int y, 
			  char r, char g, char b);
void framebuffer_fill(framebuffer *fb, char r, char g, char b);
void framebuffer_clear(framebuffer *fb);

#endif
