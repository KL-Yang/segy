#include "unit.h"
//TODO: use cario to output as PNG plot!

int main(int argc, char *argv[])
{
    if(argc!=2) {
        printf("Usage: %s name.segy\n", argv[0]);
        abort();
    }

    char *name = argv[1];
    int error, nsamp, text_len = 3200;

    segy_struct_h segy;
    error = libsegy_create_struct(&segy, LIBSEGY_READ, NULL);
    assert(error==LIBSEGY_OK);

    FILE *fp2 = fopen(name, "r");
    error = libsegy_init_io(segy, fp2);

    char text_header[3201]; text_header[3200]='\0';
    libsegy_get_textheader(segy, text_header, 3201, 0);

    char p[81]; p[80]='\0';
    for(int i=0; i<text_len; i+=80) {
        strncpy(p, text_header+i, 80);
        printf("%s", p);
        printf("|\n");
    }
    
    libsegy_check_bhdr_field(segy);

    int ext_headers, revision, format, ninst;
    libsegy_get_bhdr_field(segy, SEGY_BHDR_EXT_HEADERS, &ext_headers);
    libsegy_get_bhdr_field(segy, SEGY_BHDR_SEGY_REVISION, &revision);
    libsegy_get_bhdr_field(segy, SEGY_BHDR_SAMPLES, &nsamp);
    libsegy_get_bhdr_field(segy, SEGY_BHDR_FORMAT, &format);
    libsegy_get_bhdr_field(segy, SEGY_BHDR_TRACES, &ninst);
    printf("ext_headers=%d revision=%d nsamp=%d format=%d ninst=%d\n", 
            ext_headers, revision, nsamp, format, ninst);

    ninst = MIN(1201, ninst);

    libsegy_init_trace(segy, ninst, -1);
    int32_t *xline = malloc(ninst*sizeof(int32_t));
    libsegy_get_attr_raw(segy, SEGY_ATTR_ENSEMBLE, xline, ninst, 0);
    //libsegy_get_attr_raw(segy, SEGY_ATTR_ELEV_SCALAR, xline, 100, 0);

    float *trace = calloc(ninst, nsamp*sizeof(float));
    libsegy_get_trace_raw(segy, trace, ninst, 0);

    FILE *pngfp = fopen("unit01.png", "w");
    uint8_t *pngdata = calloc(ninst, nsamp*sizeof(uint8_t));
    float2uint8(trace, ninst*nsamp, pngdata, 0.01f);
    unit_data2png(pngfp, pngdata, nsamp, ninst, 1);
    free(pngdata);
    fclose(pngfp);

    free(trace);
    free(xline);
    fclose(fp2);

    return 1;
}
