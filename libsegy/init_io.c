#ifndef C_IO_INIT_LIBSEGY
#define C_IO_INIT_LIBSEGY

static int init_io_read(libsegy_h handle)
{
    segy_struct_t *h = (segy_struct_t*)handle;
    h->text_header = malloc((3200+1)*sizeof(char));
    h->bin_header = malloc(400*sizeof(char));
    h->text_header[3200] = '\0';

    //1. read the first 3600 byte and parse control bits
    h->fn_seek(h->fp, 0L, SEEK_SET);
    char localbuf[3600];
    h->fn_read(localbuf, 3600, sizeof(char), h->fp);
    if(isEBCDIC_text_header(localbuf, 3200)) {
        h->text_ebdic = 1;
        encode(h->text_header, localbuf, e2a, 3200);
    } else 
        memcpy(h->text_header, localbuf, 3200);
    memcpy(h->bin_header, localbuf+3200, 400);

    h->flag |= LIBSEGY_FLAG_IO_INITIATED;

    int ext_header;
    libsegy_get_bhdr_field(handle, SEGY_BHDR_EXT_HEADERS, &ext_header);
    h->trace_base = 3600+3200*ext_header;

    h->bhdr_format = get_bhdr_format(handle, &h->trace_typesize);
    h->samples = get_bhdr_samples(handle);
    assert(h->bhdr_format==1 || h->bhdr_format==2 || h->bhdr_format==3 ||
            h->bhdr_format==4 || h->bhdr_format==5 || h->bhdr_format==8);
    /**
     * NOTE! if trace_flag != 1, file cannot be seeked!!!
     * */
    if(h->samples!=0)
        h->trace_size = 240+h->trace_typesize*h->samples;
    printf("trace_size=%d\n", h->trace_size);

    //2. if necessary, read and parse the rest extended headers
    return LIBSEGY_OK;
}

/**
 * @brief Initiate IO stream, call after set_read_fn and set_write_fn
 * @param fp      : FILE normally, or other stream pointer
 * If reading, should call init_io first, then get header information.
 * If writing, should set header information, then call init_io.
 * */
int 
libsegy_init_io(segy_struct_h handle, void *fp)
{
    segy_struct_t *h = (segy_struct_t*)handle;

    h->fp = fp;
    if(h->type==LIBSEGY_READ) 
    {
        init_io_read(handle);
    } 
    else {
        //write all the header information onto the disk
        //if need setup headers, setup before calling the init_io
        //after init_io, user can start to access traces now!
    }
    return LIBSEGY_OK;
}

#endif
