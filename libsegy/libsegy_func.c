/**
 * Those are examples of creating new streaming function
 * seek is forbiden, everything is only read/write once
 * close and open is done outside of libsegy!
 * */
static size_t
dummy_fread(void *ptr, size_t size, size_t nmemb, void *stream)
{ 
    return fread(ptr, size, nmemb, stream); 
}

static size_t
dummy_fwrite(const void *ptr, size_t size, size_t nmemb, void *stream)
{ 
    return fwrite(ptr, size, nmemb, stream); 
}

static int 
dummy_fseek(void *stream, long offset, int whence)
{ 
    return fseek(stream, offset, whence); 
}
