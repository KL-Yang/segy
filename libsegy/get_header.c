int 
libsegy_get_textheader(libsegy_h handle, char *text, size_t length, int index)
{
    segy_struct_t *h = (segy_struct_t*)handle;
    if(!(h->flag & LIBSEGY_FLAG_IO_INITIATED))
        printf("ERROR %s: should be called after libsegy_init_io()!\n", __func__);

    length = MIN(length, 3200);
    if(index==0) {
        strncpy(text, h->text_header, length);
    } else {
        if(index>h->bhdr_ext_headers) {
            printf("%s: requested extend header does not exist!\n", __func__);
            return LIBSEGY_FAILED;
        }
        strncpy(text, h->etextheader[index-1], length);
    }
    return LIBSEGY_OK;
}

/**
 * @Brief Generic Binary header field extraction, all 2-4byte INT
 * @param field_id : Predefined field id constant
 * */
int
libsegy_get_bhdr_field(segy_struct_h handle, int field_id, int *value)
{
    segy_struct_t *h = (segy_struct_t*)handle;
    if(!(h->flag & LIBSEGY_FLAG_IO_INITIATED))
        printf("ERROR: %s should be called after libsegy_init_io()!\n", __func__);
    assert(field_id<SEGY_BHDR_TOTAL);
    segy_field_t *f = &segy_binfield[field_id];
    void *p = h->bin_header+f->pos-LIBSEGY_BASE_BINHEADER;

    if(f->type==LIBSEGY_TYPE_INT16) {
        int16_t val16;
        memcpy(&val16, p, sizeof(int16_t));
        *value = (int)ntohs(val16);
    } else {
        int32_t val32;
        memcpy(&val32, p, sizeof(int32_t));
        *value = (int)ntohl(val32);
    }
    return LIBSEGY_OK;
}

/**
 * If variable length samples segy file, this function returns 0!
 * TODO: set those as higher level API start with segy_
 * libsegy_ will not interpret the value, just change to convinent host format
 * segy_ function will try to interpret the value!
 * */
static int
get_bhdr_samples(segy_struct_h handle)
{
    int flag, samples=0;
    libsegy_get_bhdr_field(handle, SEGY_BHDR_TRACE_FLAG, &flag);
    if(flag!=0)
        libsegy_get_bhdr_field(handle, SEGY_BHDR_SAMPLES, &samples);
    return samples;
}

int
segy_get_bhdr_interval(segy_struct_h handle)
{   
    int si;
    libsegy_get_bhdr_field(handle, SEGY_BHDR_INTERVAL, &si); 
    return si;
}

int
segy_get_bhdr_traces(segy_struct_h handle)
{   
    int ninst;
    libsegy_get_bhdr_field(handle, SEGY_BHDR_TRACES, &ninst); 
    return ninst;
}

static int 
get_bhdr_format(segy_struct_h handle, int *size)
{
    int format;
    libsegy_get_bhdr_field(handle, SEGY_BHDR_FORMAT, &format);
    switch(format) {
        case 1: //IBM float
        case 2: //INT32
        case 4: //FIXED POINT
        case 5: //IEEE float
            *size = 4;
            break;
        case 3: //INT16
            *size = 2;
            break;
        case 8:
            *size = 1;
            break;
        default:    //unknow format code!
            *size = 0;
    }
    return format;
}

void libsegy_check_bhdr_field(segy_struct_h handle)
{
    int numb = sizeof(segy_binfield)/sizeof(segy_field_t);
    for(int i=0; i<numb; i++) {
        int val;
        libsegy_get_bhdr_field(handle, segy_binfield[i].index, &val);
        printf("[%2d] %24s  %4d | VAL=%d\n", i, segy_binfield[i].name,
                segy_binfield[i].pos, val);
    }
}
