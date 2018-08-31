/**
 * when read irregular size traces
 * @param first : if first is -1, assume sequential read
 * */
static void init_trace_read(libsegy_h handle, int numb, int64_t first)
{
    segy_struct_t *h = (segy_struct_t*)handle;
    if(h->samples!=0) {
        if(first>=0 && first!=(h->trace_first+h->trace_numb)) {
            h->trace_first = first;
            int64_t offset = first*h->trace_size+h->trace_base;
            h->fn_seek(h->fp, offset, SEEK_SET);
        } else
            h->trace_first += h->trace_numb;
        h->trace_numb = numb;

        if(h->trace_buff!=NULL)
            free(h->trace_buff);
        h->trace_buff = malloc(numb*h->trace_size);
        h->fn_read(h->trace_buff, 1, h->trace_size*numb, h->fp);
    } else {
        printf("ERROR: %s does not support variable length trace yet!\n",
                __func__);
        //build a seeking table at first reading!
    }
}

static void init_trace_write(libsegy_h handle, int numb, int64_t first)
{
    segy_struct_t *h = (segy_struct_t*)handle;
    if(h->samples!=0) {
        //1. flush previous write!
        if(h->trace_numb!=0) {
            assert(h->fn_seek!=NULL && h->samples!=0);  //regular!
            int64_t offset = h->trace_first*h->trace_size+h->trace_base;
            h->fn_seek(h->fp, offset, SEEK_SET);
            h->fn_write(h->trace_buff, h->trace_size, h->trace_numb, h->fp);
        }
        //2. resize the buffer if necessary!
        h->trace_first = (first>=0)?(first):(h->trace_first+h->trace_numb);
        if(numb!=h->trace_numb) {
            if(h->trace_numb!=0)
                free(h->trace_buff);
            h->trace_buff = calloc(numb, h->trace_size);
            h->trace_numb = numb;
        } else 
            memset(h->trace_buff, 0, numb*h->trace_size);
    } else {
        // If not fixed length, cannot initiate the buffer
        // so the right way should be initiate attr and trace buffer
        // seperately! when trace length attribute is write,
        // trace buffer can be allocated immediately
        printf("ERROR: %s does not support variable length trace yet!\n",
                __func__);
    }
}

/**
 * only handle fixed length
 * @param first : -1 for sequential reading the next batch of numb
 * */
int 
libsegy_init_trace(libsegy_h handle, int numb, int64_t first)
{
    segy_struct_t *h = (segy_struct_t*)handle;
    if(h->type == LIBSEGY_READ)  {
        assert(h->samples!=0);  //fixed length!
        init_trace_read(handle, numb, first);
    } else {
        if(!(h->flag & LIBSEGY_FLAG_IO_INITIATED))
            init_io_write_header(handle);
        init_trace_write(handle, numb, first);
    }
    return LIBSEGY_OK;
}
