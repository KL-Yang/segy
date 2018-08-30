
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
