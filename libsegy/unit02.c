#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cairo/cairo.h>

void gendat(uint8_t *argb, int nx, int ny)
{
    int ix=nx*0.4, iy=ny*0.4;
    float period = 0.1*sqrtf(nx*nx+ny*ny);
    for(int i=0; i<ny; i++)
        for(int j=0; j<nx; j++) {
            float dy = (iy-i);
            float dx = (ix-j);
            float dist = sqrtf(dx*dx+dy*dy);
            float data = (1+cosf(M_PI*dist/period))/2.0f;
            argb[4*(i*nx+j)+0] = data*255;
            argb[4*(i*nx+j)+1] = data*255;
            argb[4*(i*nx+j)+2] = data*255;
            argb[4*(i*nx+j)+3] = 255;
        }
}

int main(void)
{
    cairo_surface_t *surface;
    cairo_t *cr;

    unsigned char *data;
    int stride, height, width;
    height = 800;
    width  = 600;
    stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, width);
    printf("stride=%d\n", stride);

    data = calloc(800, stride*sizeof(unsigned char));
    gendat(data, width, height);
    surface = cairo_image_surface_create_for_data(data, CAIRO_FORMAT_ARGB32, width, height, stride);
    cr = cairo_create(surface);
/*
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
            CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 40.0);

    cairo_move_to(cr, 10.0, 50.0);
    cairo_show_text(cr, "Disziplin ist Macht.");
*/
    cairo_surface_write_to_png(surface, "image.png");
    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    return 0;
}
