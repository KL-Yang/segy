/**
 * @file
 * provide libsegy_get_attr() and libsegy_get_attr_raw()
 * SEGY raw attributes are all integer, 16bit or 32bit
 * can assume always expect native int
 * */

static void 
assert_init_range(segy_struct_h handle, int numb, int64_t first)
{
    segy_struct_t *h = (segy_struct_t*)handle;
    if((first>=0 && first<h->trace_first) ||
            (first+numb>h->trace_first+h->trace_numb)) {
        printf("ERROR: Access uninitiated trace range!\n");
        printf("  current initiated range (%ld, %ld)\n"
                "  trying to access range (%ld, %ld)\n", 
                h->trace_first, h->trace_first+h->trace_numb,
                first, first+numb);
        abort();
    }
}

/**
 * @param buff : the 240 Bytes Trace header
 * */
static int get_attr_raw(void *buff, int field_id)
{
    segy_field_t *field = &segy_trace_attr[field_id];
    size_t offset = field->pos-segy_trace_attr[0].pos;
    void *p = buff+offset;
    if(field->type==LIBSEGY_TYPE_INT16) {
        uint16_t val16;
        memcpy(&val16, p, sizeof(uint16_t));
        return (int16_t)ntohs(val16);
    } else {
        uint32_t val32;
        memcpy(&val32, p, sizeof(uint32_t));
        return (int32_t)ntohl(val32);
    }
}

/**
 * @brief Debug function to print all attribute 
 * */
int libsegy_check_attr(libsegy_h handle, int64_t first) 
{
    assert_init_range(handle, 1, first);
    segy_struct_t *h = (segy_struct_t*)handle;
    size_t offset = (first-h->trace_first)*h->trace_size;
    void *buff = h->trace_buff+offset;
    for(int i=0; i<SEGY_ATTR_TOTAL; i++) {
        int value = get_attr_raw(buff, i);
        printf("%24s -> %d\n", segy_trace_attr[i].name, value);
    }
    return LIBSEGY_OK;
}

int
libsegy_get_attr_raw(segy_struct_h handle, int field_id, int32_t *buff, int numb, int64_t first)
{
    assert_init_range(handle, numb, first);
    segy_struct_t *h = (segy_struct_t*)handle;
    assert(field_id<SEGY_ATTR_TOTAL);
    segy_field_t *field = &segy_trace_attr[field_id];

    int tr_shift = first-h->trace_first;
    int at_shift = field->pos-segy_trace_attr[0].pos;

    if(first<h->trace_first || first+numb>h->trace_first+h->trace_numb) {
        printf("ERROR: Access uninitiated trace range!");
        abort();
    } 
    if(h->samples!=0) {
        void *p = h->trace_buff+tr_shift*h->trace_size+at_shift;
        if(field->type==LIBSEGY_TYPE_INT16) {
            for(int i=0; i<numb; i++, p+=h->trace_size) {
                uint16_t val = *((uint16_t*)p);
                buff[i] = (int16_t)ntohs(val);
            }
        } else {
            printf("%s:int32 at_shift=%d\n", __func__, at_shift);
            for(int i=0; i<numb; i++, p+=h->trace_size) {
                uint32_t val = *((uint32_t*)p);
                buff[i] = (int32_t)ntohl(val);
            }
        }
    } else {
        //irregular samples length
    }
    return LIBSEGY_OK;
}

void libsegy_check_attr_field()
{
    int numb = sizeof(segy_trace_attr)/sizeof(segy_field_t);
    for(int i=0, j=1; i<numb; i++) {
        printf("[%2d] %24s  %4d <-> %4d\n", i, segy_trace_attr[i].name,
                segy_trace_attr[i].pos, j);
        assert(segy_trace_attr[i].index==i);
        int x = (segy_trace_attr[i].type==LIBSEGY_TYPE_INT16)?(2):(4);
        j += x;
    }
}
