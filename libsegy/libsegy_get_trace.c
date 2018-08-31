/**
 * @file
 * provide libsegy_get_trace() and libsegy_get_trace_raw()
 * can assume always expect to read float samples.
 * */

static void ibm2ieee2(float *ibm, int len) 
{
    for(int i=0; i<len; i++) {
        ibm[i] = (((( (*(int*)&ibm[i]) >> 31) & 0x01) * (-2)) + 1) *
            ((float)((*(int*)&ibm[i]) & 0x00ffffff) / 0x1000000) *
            ((float)pow(16, (( (*(int*)&ibm[i]) >> 24) & 0x7f) - 64));
    }
}

static void
copy_trace_bad(int ninst, int nsamp, void *pdes, size_t dskip,
        const void *psrc, size_t sskip)
{
    printf("%s: unknown or invalid format of trace data!\n", __func__);
    printf("%s(%d, %d, %p, %lu, %p, %lu)\n", __func__, ninst, nsamp,
            pdes, dskip, psrc, sskip);
    abort();
}

/**
 * @brief Copy and convert to host format value!
 * @param pdes  : destination
 * @param ninst : number of instance to copy
 * @param nsamp : number of sample per instance
 * @param ssize : sample size in number of bytes
 * @param psrc  : source location
 * @param skip  : bytes to skip for next instance
 * */
static void 
copy_trace_i08(int ninst, int nsamp, void *pdes, size_t dskip, 
        const void *psrc, size_t sskip)
{
    for(int i=0; i<ninst; i++) {
        uint8_t *des8 = (uint8_t*)(pdes+i*dskip);
        uint8_t *src8 = (uint8_t*)(psrc+i*sskip);
        for(int j=0; j<nsamp; j++)
            des8[j] = src8[j];
    }
}

static void
copy_trace_i08f(int ninst, int nsamp, void *pdes, size_t dskip,
        const void *psrc, size_t sskip)
{
    for(int i=0; i<ninst; i++) {
        float *fdes = (float*)(pdes+i*dskip);
        uint8_t *src8 = (uint8_t*)(psrc+i*sskip);
        for(int j=0; j<nsamp; j++)
            fdes[j] = src8[j];
    }

}

static void 
copy_trace_i16(int ninst, int nsamp, void *pdes, size_t dskip, 
        const void *psrc, size_t sskip)
{
    for(int i=0; i<ninst; i++) {
        uint16_t *des16 = (uint16_t*)(pdes+i*dskip);
        uint16_t *src16 = (uint16_t*)(psrc+i*sskip);
        for(int j=0; j<nsamp; j++)
            des16[j] = ntohs(src16[j]);
    }
}
static void 
copy_trace_i16f(int ninst, int nsamp, void *pdes, size_t dskip, 
        const void *psrc, size_t sskip)
{
    for(int i=0; i<ninst; i++) {
        float *fdes = (float*)(pdes+i*dskip);
        uint16_t *src16 = (uint16_t*)(psrc+i*sskip);
        for(int j=0; j<nsamp; j++)
            fdes[j] = (float)((int)ntohs(src16[j]));
    }
}

static void  //may loose precision
copy_trace_i32(int ninst, int nsamp, void *pdes, size_t dskip, 
        const void *psrc, size_t sskip)
{
    for(int i=0; i<ninst; i++) {
        uint32_t *des32 = (uint32_t*)(pdes+i*dskip);
        uint32_t *src32 = (uint32_t*)(psrc+i*sskip);
        for(int j=0; j<nsamp; j++)
            des32[j] = ntohl(src32[j]);
    }
}
static void  //may loose precision
copy_trace_i32f(int ninst, int nsamp, void *pdes, size_t dskip, 
        const void *psrc, size_t sskip)
{
    for(int i=0; i<ninst; i++) {
        float *fdes = (float*)(pdes+i*dskip);
        uint32_t *src32 = (uint32_t*)(psrc+i*sskip);
        for(int j=0; j<nsamp; j++)
            fdes[j] = (float)((int)ntohl(src32[j]));
    }
}

static void     //format=4 fix point
copy_trace_fix(int ninst, int nsamp, void *pdes, size_t dskip, 
        const void *psrc, size_t sskip)
{
    copy_trace_bad(ninst, nsamp, pdes, dskip, psrc, sskip);
}

static void     //format=1 ibm float
copy_trace_ibm(int ninst, int nsamp, void *pdes, size_t dskip, 
        const void *psrc, size_t sskip)
{
    copy_trace_i32(ninst, nsamp, pdes, dskip, psrc, sskip);
    for(int i=0; i<ninst; i++) {
        float *fdes = (float*)(pdes+i*dskip);
        ibm2ieee2(fdes, nsamp);
    }
}

static void     //format=5 ieee flt
copy_trace_flt(int ninst, int nsamp, void *pdes, size_t dskip, 
        const void *psrc, size_t sskip)
{ copy_trace_i32(ninst, nsamp, pdes, dskip, psrc, sskip); }

enum {
    SEGY_FORMAT_IBM_FLOAT=1,
    SEGY_FORMAT_INT32,
    SEGY_FORMAT_INT16,
    SEGY_FORMAT_FIX32,
    SEGY_FORMAT_FLOAT,
    SEGY_FORMAT_6,
    SEGY_FORMAT_7,
    SEGY_FORMAT_INT8,
    SEGY_FORMAT_MAX,
} SEGY_FORMAT_VALUE; 

static void 
(*copy_trace[SEGY_FORMAT_MAX])(int, int, void*, size_t, const void*, size_t) = 
{   copy_trace_bad, copy_trace_ibm, copy_trace_i32,
    copy_trace_i16, copy_trace_fix, copy_trace_flt,
    copy_trace_bad, copy_trace_bad, copy_trace_i08
};

static void
(*copy_trflt[SEGY_FORMAT_MAX])(int, int, void*, size_t, const void*, size_t) =
{   copy_trace_bad,  copy_trace_ibm, copy_trace_i32f,
    copy_trace_i16f, copy_trace_fix, copy_trace_flt,
    copy_trace_bad,  copy_trace_bad, copy_trace_i08f
};

/**
 * @brief Read and convert trace to float value!
 * */
static int 
get_trace_format(segy_struct_h handle, void *buff, int numb, int64_t first, int raw)
{
    segy_struct_t *h = (segy_struct_t*)handle;

    int tr_shift = first-h->trace_first;
    if(first<h->trace_first || first+numb>h->trace_first+h->trace_numb) {
        printf("ERROR: Access uninitiated trace range!");
        abort();
    }

    const void *psrc = h->trace_buff+240+tr_shift*h->trace_size;
    if(h->samples!=0) {
        if(raw) 
            copy_trace[h->bhdr_format](numb, h->samples, buff, 
                    h->samples*h->trace_typesize, 
                    psrc, h->trace_size);
        else
            copy_trflt[h->bhdr_format](numb, h->samples, buff, 
                    h->samples*h->trace_typesize, 
                    psrc, h->trace_size);
    } else {
        //irregular samples length, can only read trace by trace,
        //and need built up a trace map!!!
    }
    return LIBSEGY_OK;
}

/**
 * might be 8/16/32 bit integer or IEEE float, converted to host format!
 * */
int 
libsegy_get_trace_raw(segy_struct_h handle, void *buff, int numb, int64_t first)
{ return get_trace_format(handle, buff, numb, first, 1); }

int 
libsegy_get_trace(segy_struct_h handle, void *buff, int numb, int64_t first)
{ return get_trace_format(handle, buff, numb, first, 0); }
