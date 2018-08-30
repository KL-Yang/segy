#ifndef C_HEADER_LIBSEGY
#define C_HEADER_LIBSEGYy

int 
libsegy_get_textheader(libsegy_h handle, char *text, size_t length, int index)
{
    segy_struct_t *h = (segy_struct_t*)handle;
    if(!(h->flag & LIBSEGY_FLAG_IO_INITIATED)) {
        printf("ERROR %s: should be called after libsegy_init_io()!\n", __func__);
        return LIBSEGY_FAILED;
    }

    length = MIN(length, LIBSEGY_SIZE_TEXTHEAD);
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
libsegy_get_binheader(segy_struct_h handle, int field_id, int *value)
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
    libsegy_get_binheader(handle, SEGY_BHDR_TRACE_FLAG, &flag);
    if(flag!=0)
        libsegy_get_binheader(handle, SEGY_BHDR_SAMPLES, &samples);
    return samples;
}

int
segy_get_bhdr_interval(segy_struct_h handle)
{   
    int si;
    libsegy_get_binheader(handle, SEGY_BHDR_INTERVAL, &si); 
    return si;
}

int
segy_get_bhdr_traces(segy_struct_h handle)
{   
    int ninst;
    libsegy_get_binheader(handle, SEGY_BHDR_TRACES, &ninst); 
    return ninst;
}

static int 
get_bhdr_format(segy_struct_h handle, int *size)
{
    int format;
    libsegy_get_binheader(handle, SEGY_BHDR_FORMAT, &format);
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

void libsegy_check_binheader(segy_struct_h handle)
{
    int numb = sizeof(segy_binfield)/sizeof(segy_field_t);
    for(int i=0; i<numb; i++) {
        int val;
        libsegy_get_binheader(handle, segy_binfield[i].index, &val);
        printf("[%2d] %24s  %4d | VAL=%d\n", i, segy_binfield[i].name,
                segy_binfield[i].pos, val);
    }
}


static char ** 
allocate_etextheader(libsegy_h handle, char **ppstr, int n_old, size_t size, int n_new)
{
    if(n_old>=n_new)
        return ppstr;
    char **ppnew = calloc(n_new, sizeof(char**));
    for(int i=0; i<n_old; i++)      //transfer old!
        ppnew[i] = ppstr[i];
    for(int i=n_old; i<n_new; i++)  //allocate new!
        ppnew[i] = calloc(size+1, sizeof(char));
    //mark new number of extend text header
    libsegy_set_bhdr_field(handle, SEGY_BHDR_EXT_HEADERS, n_new);  
    free(ppstr);
    return ppnew;
}

/**
 * @brief Set text header and extended text header
 * @param buff: 3200+1 byte Textual File Header
 * If set extended text header, the binary header will be changed accordingly.
 * */
int
libsegy_set_textheader(libsegy_h handle, const char *buff, int index)
{
    segy_struct_t *h = (segy_struct_t*)handle;
    if(h->flag & LIBSEGY_FLAG_IO_INITIATED)
        printf("ERROR: %s should be called before libsegy_init_io()!\n", __func__);
    assert(h->type==LIBSEGY_WRITE);
    if(index==0) {
        if(h->text_header==NULL)
            h->text_header = calloc(LIBSEGY_SIZE_TEXTHEAD+1, sizeof(char));
        strncpy(h->text_header, buff, LIBSEGY_SIZE_TEXTHEAD);
    } else {
        allocate_etextheader(handle, h->etextheader, h->n_etextheader, 
                LIBSEGY_SIZE_TEXTHEAD+1, index);
        strncpy(h->etextheader[index-1], buff, LIBSEGY_SIZE_TEXTHEAD);
    }
    return LIBSEGY_OK;
}

int
libsegy_set_bhdr_field(libsegy_h handle, int field_id, int value)
{
    segy_struct_t *h = (segy_struct_t*)handle;
    if(h->flag & LIBSEGY_FLAG_IO_INITIATED)
        printf("ERROR: %s should be called before libsegy_init_io()!\n", __func__);
    assert(field_id<SEGY_BHDR_TOTAL);
    segy_field_t *f = &segy_binfield[field_id];
    void *p = h->bin_header+f->pos-LIBSEGY_BASE_BINHEADER;

    if(f->type==LIBSEGY_TYPE_INT16) {
        uint16_t val16;
        val16 = htons(((int16_t)value));
        memcpy(p, &val16, sizeof(uint16_t));
    } else {
        uint32_t val32;
        val32 = htonl(((int32_t)value));
        memcpy(p, &val32, sizeof(uint32_t));
    }
    return LIBSEGY_OK;
}
#endif
