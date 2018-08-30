#include "unit.h"

/**
 * @brief Convert 8bit data array to PNG picture for display
 * @param data   : unsigned 8bit data array
 * @param width  : horizontal length
 * @param height : veritical length
 * */
int unit_data2png(FILE *fp, const uint8_t *data, int width, int height, int transpose)
{
	int code = 0;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	
	// Initialize write structure
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fprintf(stderr, "Could not allocate write struct\n");
		code = 1;
		goto finalise;
	}

	// Initialize info structure
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fprintf(stderr, "Could not allocate info struct\n");
		code = 1;
		goto finalise;
	}

	// Setup Exception handling
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "Error during png creation\n");
		code = 1;
		goto finalise;
	}

	png_init_io(png_ptr, fp);
    if(transpose) { int temp=height; height=width; width=temp; }
	png_set_IHDR(png_ptr, info_ptr, width, height,
			8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr, info_ptr);
    if(transpose) {
        png_bytep row = malloc(width*sizeof(png_byte));
        for(int y=0; y<height ; y++) {
            for(int x=0; x<width; x++)
                row[x] = data[x*height+y];
            png_write_row(png_ptr, row);
        }
        free(row);
    } else {
        for(int y=0 ; y<height ; y++) {
            png_bytep row = (png_bytep)(data+y*width);
            png_write_row(png_ptr, row);
        }
    }
    png_write_end(png_ptr, NULL);

finalise:
    if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    return code;
}


void float2uint8(const float *f, int n, uint8_t *u, float ratio)
{
    float min=FLT_MAX, max=-FLT_MAX;
    for(int i=0; i<n; i++) {
        min = MIN(f[i], min);
        max = MAX(f[i], max);
    }
    min *= ratio; max *= ratio;
    float len=max-min;
    printf("%s: min=%f max=%f\n", __func__, min, max);
    for(int i=0; i<n; i++)
        if(f[i]<min) 
            u[i] = 0;
        else if(f[i]>max)
            u[i] = 255;
        else
            u[i] = 255-255*(f[i]-min)/len;
}

/*
void gendat(uint8_t *data, int nx, int ny)
{
    int ix=nx*0.4, iy=ny*0.4;
    float period = 0.1*sqrtf(nx*nx+ny*ny);
    for(int i=0; i<ny; i++)
        for(int j=0; j<nx; j++) {
            float dy = (iy-i);
            float dx = (ix-j);
            float dist = sqrtf(dx*dx+dy*dy);
            data[i*nx+j] = 255*(1+cosf(M_PI*dist/period))/2.0f;
        }
}

#define WIDTH   800
#define HEIGHT  600

int main(int argc, const char argv[])
{
    uint8_t *data;
    int nx=WIDTH, ny=HEIGHT;

    data = calloc(ny, nx*sizeof(uint8_t));
    gendat(data, nx, ny);

    FILE *fp = fopen("test.png", "w");
    unit_data2png(fp, data, nx, ny);
    fclose(fp);

    free(data);
    return 0;
}
*/
