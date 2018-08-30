#define LIBSEGY_OK      0
#define LIBSEGY_FAILED -1

#define LIBSEGY_TYPE_INT16      2
#define LIBSEGY_TYPE_INT32      3
#define LIBSEGY_TYPE_FLOAT      4
#define LIBSEGY_TYPE_DOUBLE     5


#define LIBSEGY_MAJOR   0
#define LIBSEGY_MINOR   1
#define LIBSEGY_READ    0
#define LIBSEGY_WRITE   1

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SEGY_BHDR_JOB_ID = 0,
    SEGY_BHDR_LINE_NUMBER,
    SEGY_BHDR_REEL_NUMBER,
    SEGY_BHDR_TRACES,
    SEGY_BHDR_AUX_TRACES,
    SEGY_BHDR_INTERVAL,
    SEGY_BHDR_INTERVAL_ORIG,
    SEGY_BHDR_SAMPLES,
    SEGY_BHDR_SAMPLES_ORIG,
    SEGY_BHDR_FORMAT,
    SEGY_BHDR_ENSEMBLE_FOLD,
    SEGY_BHDR_SORTING_CODE,
    SEGY_BHDR_VERTICAL_SUM,
    SEGY_BHDR_SWEEP_FREQ_START,
    SEGY_BHDR_SWEEP_FREQ_END,
    SEGY_BHDR_SWEEP_LENGTH,
    SEGY_BHDR_SWEEP,
    SEGY_BHDR_SWEEP_CHANNEL,
    SEGY_BHDR_SWEEP_TAPER_START,
    SEGY_BHDR_SWEEP_TAPER_END,
    SEGY_BHDR_TAPER,
    SEGY_BHDR_CORRELATED_TRACES,
    SEGY_BHDR_BIN_GAIN_RECOVERY,
    SEGY_BHDR_AMPLITUDE_RECOVERY,
    SEGY_BHDR_MEASUREMENT_SYSTEM,
    SEGY_BHDR_IMPULSE_POLARITY,
    SEGY_BHDR_VIBRATORY_POLARITY,
    SEGY_BHDR_UNASSIGNED1,
    SEGY_BHDR_SEGY_REVISION,
    SEGY_BHDR_TRACE_FLAG,
    SEGY_BHDR_EXT_HEADERS,
    SEGY_BHDR_UNASSIGNED2,
    SEGY_BHDR_TOTAL,
} SEGY_BHDR_FIELD;


/**
 * Trace header definition, named as attribute
 * */
typedef enum {
    SEGY_ATTR_SEQ_LINE = 0,
    SEGY_ATTR_SEQ_FILE,
    SEGY_ATTR_FIELD_RECORD,
    SEGY_ATTR_NUMBER_ORIG_FIELD,
    SEGY_ATTR_ENERGY_SOURCE_POINT,
    SEGY_ATTR_ENSEMBLE,
    SEGY_ATTR_NUM_IN_ENSEMBLE,
    SEGY_ATTR_TRACE_ID,
    SEGY_ATTR_SUMMED_TRACES,
    SEGY_ATTR_STACKED_TRACES,
    SEGY_ATTR_DATA_USE,
    SEGY_ATTR_OFFSET,
    SEGY_ATTR_RECV_GROUP_ELEV,
    SEGY_ATTR_SOURCE_SURF_ELEV,
    SEGY_ATTR_SOURCE_DEPTH,
    SEGY_ATTR_RECV_DATUM_ELEV,
    SEGY_ATTR_SOURCE_DATUM_ELEV,
    SEGY_ATTR_SOURCE_WATER_DEPTH,
    SEGY_ATTR_GROUP_WATER_DEPTH,
    SEGY_ATTR_ELEV_SCALAR,
    SEGY_ATTR_SOURCE_GROUP_SCALAR,
    SEGY_ATTR_SOURCE_X,
    SEGY_ATTR_SOURCE_Y,
    SEGY_ATTR_GROUP_X,
    SEGY_ATTR_GROUP_Y,
    SEGY_ATTR_COORD_UNITS,
    SEGY_ATTR_WEATHERING_VELO,
    SEGY_ATTR_SUBWEATHERING_VELO,
    SEGY_ATTR_SOURCE_UPHOLE_TIME,
    SEGY_ATTR_GROUP_UPHOLE_TIME,
    SEGY_ATTR_SOURCE_STATIC_CORR,
    SEGY_ATTR_GROUP_STATIC_CORR,
    SEGY_ATTR_TOT_STATIC_APPLIED,
    SEGY_ATTR_LAG_A,
    SEGY_ATTR_LAG_B,
    SEGY_ATTR_DELAY_REC_TIME,
    SEGY_ATTR_MUTE_TIME_START,
    SEGY_ATTR_MUTE_TIME_END,
    SEGY_ATTR_SAMPLE_COUNT,
    SEGY_ATTR_SAMPLE_INTER,
    SEGY_ATTR_GAIN_TYPE,
    SEGY_ATTR_INSTR_GAIN_CONST,
    SEGY_ATTR_INSTR_INIT_GAIN,
    SEGY_ATTR_CORRELATED,
    SEGY_ATTR_SWEEP_FREQ_START,
    SEGY_ATTR_SWEEP_FREQ_END,
    SEGY_ATTR_SWEEP_LENGTH,
    SEGY_ATTR_SWEEP_TYPE,
    SEGY_ATTR_SWEEP_TAPERLEN_START,
    SEGY_ATTR_SWEEP_TAPERLEN_END,
    SEGY_ATTR_TAPER_TYPE,
    SEGY_ATTR_ALIAS_FILT_FREQ,
    SEGY_ATTR_ALIAS_FILT_SLOPE,
    SEGY_ATTR_NOTCH_FILT_FREQ,
    SEGY_ATTR_NOTCH_FILT_SLOPE,
    SEGY_ATTR_LOW_CUT_FREQ,
    SEGY_ATTR_HIGH_CUT_FREQ,
    SEGY_ATTR_LOW_CUT_SLOPE,
    SEGY_ATTR_HIGH_CUT_SLOPE,
    SEGY_ATTR_YEAR_DATA_REC,
    SEGY_ATTR_DAY_OF_YEAR,
    SEGY_ATTR_HOUR_OF_DAY,
    SEGY_ATTR_MIN_OF_HOUR,
    SEGY_ATTR_SEC_OF_MIN,
    SEGY_ATTR_TIME_BASE_CODE,
    SEGY_ATTR_WEIGHTING_FAC,
    SEGY_ATTR_GEOPHONE_GROUP_ROLL1,
    SEGY_ATTR_GEOPHONE_GROUP_FIRST,
    SEGY_ATTR_GEOPHONE_GROUP_LAST,
    SEGY_ATTR_GAP_SIZE,
    SEGY_ATTR_OVER_TRAVEL,
    SEGY_ATTR_CDP_X,
    SEGY_ATTR_CDP_Y,
    SEGY_ATTR_INLINE,
    SEGY_ATTR_CROSSLINE,
    SEGY_ATTR_SHOT_POINT,
    SEGY_ATTR_SHOT_POINT_SCALAR,
    SEGY_ATTR_MEASURE_UNIT,
    SEGY_ATTR_TRANSDUCTION_MANT,
    SEGY_ATTR_TRANSDUCTION_EXP,
    SEGY_ATTR_TRANSDUCTION_UNIT,
    SEGY_ATTR_DEVICE_ID,
    SEGY_ATTR_SCALAR_TRACE_HEADER,
    SEGY_ATTR_SOURCE_TYPE,
    SEGY_ATTR_SOURCE_ENERGY_DIR_MANT,
    SEGY_ATTR_SOURCE_ENERGY_DIR_EXP,
    SEGY_ATTR_SOURCE_MEASURE_MANT,
    SEGY_ATTR_SOURCE_MEASURE_EXP,
    SEGY_ATTR_SOURCE_MEASURE_UNIT,
    SEGY_ATTR_UNASSIGNED1,
    SEGY_ATTR_UNASSIGNED2,
    SEGY_ATTR_TOTAL,
} SEGY_ATTR_FIELD;

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
int libsegy_init(libsegy_h *handle, int type, void *fp, libsegy_io_func_t *op);

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
