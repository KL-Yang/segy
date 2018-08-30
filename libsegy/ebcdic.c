
/**
 * @brief Try check how many none ASCII character might found!
 * */
static int
isEBCDIC_try_ascii(const char *text_header, size_t length, char *temp)
{
    int isEBCDIC=0, ascii_count=0, ebcdic_count=0;
    for(size_t i=0; i<length; i++)
        if(!isascii(text_header[i])) 
            ascii_count++;
    if(ascii_count) {
        encode(temp, text_header, e2a, length);
        for(size_t i=0; i<length; i++)
            if(!isascii(temp[i]))
                ebcdic_count++;
        if(ebcdic_count<ascii_count)
            isEBCDIC=1;
    }
    return isEBCDIC;
}

static int
isEBCDIC_try_print(const char *text_header, size_t length, char *temp)
{
    int isEBCDIC=0, ascii_count=0, ebcdic_count=0;
    for(size_t i=0; i<length; i++)
        if(!isprint(text_header[i])) 
            ascii_count++;
    if(ascii_count) {
        encode(temp, text_header, e2a, length);
        for(size_t i=0; i<length; i++)
            if(!isprint(temp[i]))
                ebcdic_count++;
        if(ebcdic_count<ascii_count)
            isEBCDIC=1;
    }
    return isEBCDIC;
}

/**
 * @brief Test Textual header if the first column is Cc in EBCDIC or ASCII
 * */
static int
isEBCDIC_text_header(const char *text_header, size_t length)
{
    int isEBCDIC = 0;
    if(((uint8_t*)text_header)[0]==0x83 || ((uint8_t*)text_header)[0]==0xc3)
        isEBCDIC = 1;

    char *local = malloc(length);
    if(!isEBCDIC) {
        /**
         * assume ascii, and test with isascii(3), if fail
         * try encode before test with isascii(3).
         * */
        isEBCDIC = isEBCDIC_try_ascii(text_header, length, local);

        /**
         * assume ascii, and test with isprint(3), if fail
         * try encode before test with isprint(3)
         * */
        if(!isEBCDIC)
            isEBCDIC = isEBCDIC_try_print(text_header, length, local);
    }
    free(local);
    return isEBCDIC;
}
