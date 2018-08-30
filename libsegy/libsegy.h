#define LIBSEGY_OK      0
#define LIBSEGY_FAILED -1

#define LIBSEGY_MAJOR   0
#define LIBSEGY_MINOR   1
#define LIBSEGY_READ    0
#define LIBSEGY_WRITE   1

#ifdef __cplusplus
extern "C" {
#endif

typedef struct segy_struct_t * segy_struct_h;
typedef struct segy_struct_t * libsegy_h;

typedef struct {
    size_t   (* fn_read)(void *, size_t, size_t, void *);
    size_t   (* fn_write)(const void *, size_t, size_t, void *);
    int      (* fn_seek)(void *, long, int);
} libsegy_io_func_t;

/**
 * @brief return the version of libsegy
 * */
int libsegy_getversion(int *major, int *minor);

/**
 * @brief create libsegy handle for read or write
 * READ with or without seek?
 * */
int libsegy_create_struct(libsegy_h *handle, int type, libsegy_io_func_t *op);

/**
 * @brief Set text, binary and extend text headers
 * used only for type=LIBSEGY_WRITE, call before init_io
 * */
int libsegy_set_bhdr_field(libsegy_h handle, int field_id, int value);
int libsegy_set_textheader(libsegy_h handle, const char *buff, int index);

/**
 * @brief initiate io for libsegy
 * Note, for read, all the get information should go after init_io
 * for write segy, all the set header functions should go before init_io
 * */
int libsegy_init_io(libsegy_h handle, void *fp);

/**
 * @brief Get text, binary and extend text headers
 * */
int libsegy_get_textheader(libsegy_h handle, char *text, size_t length, int index);
int libsegy_get_bhdr_field(segy_struct_h handle, int field_id, int *value);

//debug function to print all bhdr field
void libsegy_check_bhdr_field(segy_struct_h handle);

/**
 * @param first : put as last parameters and later as variable argument
 *      as this parameter is not needed to handle unseekable stream.
 *
 * note, raw is still using local format, only support int16/int32 for attr
 * and int16/int32/float for trace!
 * Note: the init_trace is called first before set/get attr/trace for both read/write
 * when read, the data is prepared and ready for retrive,
 * when write, the data is flush at next call to init_trace.
 * The last flush happens at libsegy_destroy()
 * */
int libsegy_init_trace(segy_struct_h handle, int numb, int64_t first);
int libsegy_get_attr_raw(segy_struct_h handle, int field_id, int32_t *buff, int numb, int64_t first);
int libsegy_get_trace_raw(segy_struct_h handle, void *buff, int numb, int64_t first);
//libsegy_set_attr_raw
//libsegy_set_trace_raw

//mimic the libpng API, and python module named pysegy
//maybe later pysegd

/**
 * 1. segy_create_struct() create read or write struct
 *    1a. segy_set_read_fn();
 *    1b. segy_set_write_fn();
 * 2. segy_init_io() initiate io, use FILE* or 
 *    If read, initiate first, then get field, if write,
 *       set field first, then initiate header!
 *    2a. segy_init_header() if read
 *    2b. segy_set/get_textheader();
 *    2c. segy_set/get_binheader();
 *    2d. segy_set/get_bhdr_field();
 *    2e. segy_set/get_tape_label();
 * 3. segy_init_trace() if read, call before get, if write, 
 *       call after set!
 *    3a. segy_set/get_trace_data();
 *    3b. segy_set/get_trace_attr();
 * 6. segy_destroy()
 *
 * optionally adjust lable bytes, will cause redo decode of binheader!!!
 * segy_guess_label()   //use textheader's leading C! guess and read!
 * segy_set_label_bytes()
 * */

// optional handle of stream!
// segy_set_read_fn();
// segy_set_write_fn();
// segy_get_io_ptr();

#ifdef __cplusplus
}
#endif
