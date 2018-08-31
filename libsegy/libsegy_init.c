#ifndef C_INIT_LIBSEGY
#define C_INIT_LIBSEGY

static int init_io_read2(libsegy_h handle)
{
    segy_struct_t *h = (segy_struct_t*)handle;

    //1. read the first 3600 byte and parse control bits
    char localbuf[LIBSEGY_SIZE_TEXTHEAD+LIBSEGY_SIZE_BINHEAD];
    if(h->fn_seek!=NULL)
        h->fn_seek(h->fp, 0L, SEEK_SET);
    h->fn_read(localbuf, LIBSEGY_SIZE_TEXTHEAD+LIBSEGY_SIZE_BINHEAD,
            sizeof(char), h->fp);

    if(isEBCDIC_text_header(localbuf, LIBSEGY_SIZE_TEXTHEAD)) {
        h->text_ebdic = 1;
        ebdic2ascii(h->text_header, localbuf, LIBSEGY_SIZE_TEXTHEAD);
    } else 
        memcpy(h->text_header, localbuf, LIBSEGY_SIZE_TEXTHEAD);

    memcpy(h->bin_header, localbuf+LIBSEGY_SIZE_TEXTHEAD, LIBSEGY_SIZE_BINHEAD);

    h->flag |= LIBSEGY_FLAG_IO_INITIATED;

    libsegy_get_binheader(handle, SEGY_BHDR_EXT_HEADERS, &h->n_etextheader);
    h->trace_base = LIBSEGY_SIZE_TEXTHEAD+LIBSEGY_SIZE_BINHEAD
        +LIBSEGY_SIZE_TEXTHEAD*h->n_etextheader;

    h->bhdr_format = get_bhdr_format(handle, &h->trace_typesize);
    h->samples = get_bhdr_samples(handle);
    assert(h->bhdr_format==1 || h->bhdr_format==2 || h->bhdr_format==3 ||
            h->bhdr_format==4 || h->bhdr_format==5 || h->bhdr_format==8);
    /**
     * NOTE! if trace_flag != 1, file cannot be seeked!!!
     * */
    if(h->samples!=0)
        h->trace_size = LIBSEGY_SIZE_TRACEHEAD+h->trace_typesize*h->samples;

    //2. if necessary, read and parse the rest extended headers
    return LIBSEGY_OK;
}

/**
 * @brief write header of SEGY file, to be called at first libsegy_init_trace()
 * */
static int init_io_write_header(libsegy_h handle)
{
    segy_struct_t *h = (segy_struct_t*)handle;

    //1. write the text header!
    char local[LIBSEGY_SIZE_TEXTHEAD];
    if(h->fn_seek!=NULL)
        h->fn_seek(h->fp, 0L, SEEK_SET);
    ascii2ebdic(local, h->text_header, LIBSEGY_SIZE_TEXTHEAD);
    h->fn_write(local, sizeof(char), LIBSEGY_SIZE_TEXTHEAD, h->fp);
    
    //2. write bin header
    h->fn_write(h->bin_header, sizeof(char), LIBSEGY_SIZE_BINHEAD, h->fp);

    //3. write extended header
    for(int i=0; i<h->n_etextheader; i++) {
        ascii2ebdic(local, h->etextheader[i], LIBSEGY_SIZE_TEXTHEAD);
        h->fn_write(local, sizeof(char), LIBSEGY_SIZE_TEXTHEAD, h->fp);
    }
    h->flag |= LIBSEGY_FLAG_IO_INITIATED;
    return LIBSEGY_OK;
}

/**
 * @brief
 * @param handle :
 * @param fp     : FILE pointer of other io function parameters
 * @param op     : IO operation function pointer or NULL
 * */
int libsegy_init(libsegy_h *handle, int type, void *fp, libsegy_io_func_t *op)
{
    segy_struct_t *h = calloc(1, sizeof(segy_struct_t));

    h->fp = fp;
    h->type = type;
    if(op!=NULL) {
        h->fn_read = op->fn_read;
        h->fn_write = op->fn_write;
        h->fn_seek = op->fn_seek;
    } else {
        h->fn_read = &dummy_fread;
        h->fn_write = &dummy_fwrite;
        h->fn_seek = &dummy_fseek;
    }
    assert(type==LIBSEGY_READ || type==LIBSEGY_WRITE);

    h->bin_header = calloc(LIBSEGY_SIZE_BINHEAD, sizeof(char));
    h->text_header = calloc(LIBSEGY_SIZE_TEXTHEAD+1, sizeof(char));

    if(type==LIBSEGY_READ)
        init_io_read2((libsegy_h)h);

    *handle = (libsegy_h)h;
    return LIBSEGY_OK;
}

#endif
