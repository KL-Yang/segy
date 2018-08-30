#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>
#include "libsegy.h"
#include "segy_bhdr.h"
#include "segy_thdr.h"

#define LIBSEGY_SIZE_TEXTHEAD   3200
#define LIBSEGY_SIZE_BINHEAD    400

#define LIBSEGY_BASE_BINHEADER  3201
#define MIN(a,b)    (a)>(b)?(b):(a)

/**
 * There need to be several stage:
 *  1. SEGD/ROPE -> SEGY disk file
 *  2. SEGY disk -> internal format or directly display
 * Allow seek!
 * */

segy_field_t segy_binfield[] = {
    {"JOB_ID",              LIBSEGY_TYPE_INT32,     3201,     SEGY_BHDR_JOB_ID}, 
    {"LINE_NUMBER",         LIBSEGY_TYPE_INT32,     3205,     SEGY_BHDR_LINE_NUMBER},
    {"REEL_NUMBER",         LIBSEGY_TYPE_INT32,     3209,     SEGY_BHDR_REEL_NUMBER},
    {"TRACES",              LIBSEGY_TYPE_INT16,     3213,     SEGY_BHDR_TRACES},
    {"AUX_TRACES",          LIBSEGY_TYPE_INT16,     3215,     SEGY_BHDR_AUX_TRACES},
    {"INTERVAL",            LIBSEGY_TYPE_INT16,     3217,     SEGY_BHDR_INTERVAL},
    {"INTERVAL_ORIG",       LIBSEGY_TYPE_INT16,     3219,     SEGY_BHDR_INTERVAL_ORIG},
    {"SAMPLES",             LIBSEGY_TYPE_INT16,     3221,     SEGY_BHDR_SAMPLES},
    {"SAMPLES_ORIG",        LIBSEGY_TYPE_INT16,     3223,     SEGY_BHDR_SAMPLES_ORIG},
    {"FORMAT",              LIBSEGY_TYPE_INT16,     3225,     SEGY_BHDR_FORMAT},
    {"ENSEMBLE_FOLD",       LIBSEGY_TYPE_INT16,     3227,     SEGY_BHDR_ENSEMBLE_FOLD},
    {"SORTING_CODE",        LIBSEGY_TYPE_INT16,     3229,     SEGY_BHDR_SORTING_CODE},
    {"VERTICAL_SUM",        LIBSEGY_TYPE_INT16,     3231,     SEGY_BHDR_VERTICAL_SUM},
    {"SWEEP_FREQ_START",    LIBSEGY_TYPE_INT16,     3233,     SEGY_BHDR_SWEEP_FREQ_START},
    {"SWEEP_FREQ_END",      LIBSEGY_TYPE_INT16,     3235,     SEGY_BHDR_SWEEP_FREQ_END},
    {"SWEEP_LENGTH",        LIBSEGY_TYPE_INT16,     3237,     SEGY_BHDR_SWEEP_LENGTH},
    {"SWEEP",               LIBSEGY_TYPE_INT16,     3239,     SEGY_BHDR_SWEEP},
    {"SWEEP_CHANNEL",       LIBSEGY_TYPE_INT16,     3241,     SEGY_BHDR_SWEEP_CHANNEL},
    {"SWEEP_TAPER_START",   LIBSEGY_TYPE_INT16,     3243,     SEGY_BHDR_SWEEP_TAPER_START},
    {"SWEEP_TAPER_END",     LIBSEGY_TYPE_INT16,     3245,     SEGY_BHDR_SWEEP_TAPER_END},
    {"TAPER",               LIBSEGY_TYPE_INT16,     3247,     SEGY_BHDR_TAPER},
    {"CORRLATED_TRACE",     LIBSEGY_TYPE_INT16,     3249,     SEGY_BHDR_CORRELATED_TRACES},
    {"BIN_GAIN_RECOVERY",   LIBSEGY_TYPE_INT16,     3251,     SEGY_BHDR_BIN_GAIN_RECOVERY},
    {"AMPLITUDE_RECOVERY",  LIBSEGY_TYPE_INT16,     3253,     SEGY_BHDR_AMPLITUDE_RECOVERY},
    {"MEASUREMENT_SYSTEM",  LIBSEGY_TYPE_INT16,     3255,     SEGY_BHDR_MEASUREMENT_SYSTEM},
    {"IMPULSE_POLARITY",    LIBSEGY_TYPE_INT16,     3257,     SEGY_BHDR_IMPULSE_POLARITY},
    {"VIBRATORY_POLARITY",  LIBSEGY_TYPE_INT16,     3259,     SEGY_BHDR_VIBRATORY_POLARITY},
    {"_UNASSIGNED1",        LIBSEGY_TYPE_INT16,     3261,     SEGY_BHDR_UNASSIGNED1},
    {"SEGY_REVISION",       LIBSEGY_TYPE_INT16,     3501,     SEGY_BHDR_SEGY_REVISION},
    {"TRACE_FLAG",          LIBSEGY_TYPE_INT16,     3503,     SEGY_BHDR_TRACE_FLAG},
    {"EXT_HEADERS",         LIBSEGY_TYPE_INT16,     3505,     SEGY_BHDR_EXT_HEADERS},
    {"_UNASSIGNED2",        LIBSEGY_TYPE_INT16,     3507,     SEGY_BHDR_UNASSIGNED2}
};

segy_field_t segy_trace_attr[] = {
    {"SEQ_LINE",                LIBSEGY_TYPE_INT32,  1,     SEGY_ATTR_SEQ_LINE},
    {"SEQ_FILE",                LIBSEGY_TYPE_INT32,  5,     SEGY_ATTR_SEQ_FILE},
    {"FIELD_RECORD",            LIBSEGY_TYPE_INT32,  9,     SEGY_ATTR_FIELD_RECORD},
    {"NUMBER_ORIG_FIELD",       LIBSEGY_TYPE_INT32,  13,    SEGY_ATTR_NUMBER_ORIG_FIELD},
    {"ENERGY_SOURCE_POINT",     LIBSEGY_TYPE_INT32,  17,    SEGY_ATTR_ENERGY_SOURCE_POINT},
    {"ENSEMBLE",                LIBSEGY_TYPE_INT32,  21,    SEGY_ATTR_ENSEMBLE},
    {"NUM_IN_ENSEMBLE",         LIBSEGY_TYPE_INT32,  25,    SEGY_ATTR_NUM_IN_ENSEMBLE},
    {"TRACE_ID",                LIBSEGY_TYPE_INT16,  29,    SEGY_ATTR_TRACE_ID},
    {"SUMMED_TRACES",           LIBSEGY_TYPE_INT16,  31,    SEGY_ATTR_SUMMED_TRACES},
    {"STACKED_TRACES",          LIBSEGY_TYPE_INT16,  33,    SEGY_ATTR_STACKED_TRACES},
    {"DATA_USE",                LIBSEGY_TYPE_INT16,  35,    SEGY_ATTR_DATA_USE},
    {"OFFSET",                  LIBSEGY_TYPE_INT32,  37,    SEGY_ATTR_OFFSET},
    {"RECV_GROUP_ELEV",         LIBSEGY_TYPE_INT32,  41,    SEGY_ATTR_RECV_GROUP_ELEV},
    {"SOURCE_SURF_ELEV",        LIBSEGY_TYPE_INT32,  45,    SEGY_ATTR_SOURCE_SURF_ELEV},
    {"SOURCE_DEPTH",            LIBSEGY_TYPE_INT32,  49,    SEGY_ATTR_SOURCE_DEPTH},
    {"RECV_DATUM_ELEV",         LIBSEGY_TYPE_INT32,  53,    SEGY_ATTR_RECV_DATUM_ELEV},
    {"SOURCE_DATUM_ELEV",       LIBSEGY_TYPE_INT32,  57,    SEGY_ATTR_SOURCE_DATUM_ELEV},
    {"SOURCE_WATER_DEPTH",      LIBSEGY_TYPE_INT32,  61,    SEGY_ATTR_SOURCE_WATER_DEPTH},
    {"GROUP_WATER_DEPTH",       LIBSEGY_TYPE_INT32,  65,    SEGY_ATTR_GROUP_WATER_DEPTH},
    {"ELEV_SCALAR",             LIBSEGY_TYPE_INT16,  69,    SEGY_ATTR_ELEV_SCALAR},
    {"SOURCE_GROUP_SCALAR",     LIBSEGY_TYPE_INT16,  71,    SEGY_ATTR_SOURCE_GROUP_SCALAR},
    {"SOURCE_X",                LIBSEGY_TYPE_INT32,  73,    SEGY_ATTR_SOURCE_X},
    {"SOURCE_Y",                LIBSEGY_TYPE_INT32,  77,    SEGY_ATTR_SOURCE_Y},
    {"GROUP_X",                 LIBSEGY_TYPE_INT32,  81,    SEGY_ATTR_GROUP_X},
    {"GROUP_Y",                 LIBSEGY_TYPE_INT32,  85,    SEGY_ATTR_GROUP_Y},
    {"COORD_UNITS",             LIBSEGY_TYPE_INT16,  89,    SEGY_ATTR_COORD_UNITS},
    {"WEATHERING_VELO",         LIBSEGY_TYPE_INT16,  91,    SEGY_ATTR_WEATHERING_VELO},
    {"SUBWEATHERING_VELO",      LIBSEGY_TYPE_INT16,  93,    SEGY_ATTR_SUBWEATHERING_VELO},
    {"SOURCE_UPHOLE_TIME",      LIBSEGY_TYPE_INT16,  95,    SEGY_ATTR_SOURCE_UPHOLE_TIME},
    {"GROUP_UPHOLE_TIME",       LIBSEGY_TYPE_INT16,  97,    SEGY_ATTR_GROUP_UPHOLE_TIME},
    {"SOURCE_STATIC_CORR",      LIBSEGY_TYPE_INT16,  99,    SEGY_ATTR_SOURCE_STATIC_CORR},
    {"GROUP_STATIC_CORR",       LIBSEGY_TYPE_INT16,  101,   SEGY_ATTR_GROUP_STATIC_CORR},
    {"TOT_STATIC_APPLIED",      LIBSEGY_TYPE_INT16,  103,   SEGY_ATTR_TOT_STATIC_APPLIED},
    {"LAG_A",                   LIBSEGY_TYPE_INT16,  105,   SEGY_ATTR_LAG_A},
    {"LAG_B",                   LIBSEGY_TYPE_INT16,  107,   SEGY_ATTR_LAG_B},
    {"DELAY_REC_TIME",          LIBSEGY_TYPE_INT16,  109,   SEGY_ATTR_DELAY_REC_TIME},
    {"MUTE_TIME_START",         LIBSEGY_TYPE_INT16,  111,   SEGY_ATTR_MUTE_TIME_START},
    {"MUTE_TIME_END",           LIBSEGY_TYPE_INT16,  113,   SEGY_ATTR_MUTE_TIME_END},
    {"SAMPLE_COUNT",            LIBSEGY_TYPE_INT16,  115,   SEGY_ATTR_SAMPLE_COUNT},
    {"SAMPLE_INTER",            LIBSEGY_TYPE_INT16,  117,   SEGY_ATTR_SAMPLE_INTER},
    {"GAIN_TYPE",               LIBSEGY_TYPE_INT16,  119,   SEGY_ATTR_GAIN_TYPE},
    {"INSTR_GAIN_CONST",        LIBSEGY_TYPE_INT16,  121,   SEGY_ATTR_INSTR_GAIN_CONST},
    {"INSTR_INIT_GAIN",         LIBSEGY_TYPE_INT16,  123,   SEGY_ATTR_INSTR_INIT_GAIN},
    {"CORRELATED",              LIBSEGY_TYPE_INT16,  125,   SEGY_ATTR_CORRELATED},
    {"SWEEP_FREQ_START",        LIBSEGY_TYPE_INT16,  127,   SEGY_ATTR_SWEEP_FREQ_START},
    {"SWEEP_FREQ_END",          LIBSEGY_TYPE_INT16,  129,   SEGY_ATTR_SWEEP_FREQ_END},
    {"SWEEP_LENGTH",            LIBSEGY_TYPE_INT16,  131,   SEGY_ATTR_SWEEP_LENGTH},
    {"SWEEP_TYPE",              LIBSEGY_TYPE_INT16,  133,   SEGY_ATTR_SWEEP_TYPE},
    {"SWEEP_TAPERLEN_START",    LIBSEGY_TYPE_INT16,  135,   SEGY_ATTR_SWEEP_TAPERLEN_START},
    {"SWEEP_TAPERLEN_END",      LIBSEGY_TYPE_INT16,  137,   SEGY_ATTR_SWEEP_TAPERLEN_END},
    {"TAPER_TYPE",              LIBSEGY_TYPE_INT16,  139,   SEGY_ATTR_TAPER_TYPE},
    {"ALIAS_FILT_FREQ",         LIBSEGY_TYPE_INT16,  141,   SEGY_ATTR_ALIAS_FILT_FREQ},
    {"ALIAS_FILT_SLOPE",        LIBSEGY_TYPE_INT16,  143,   SEGY_ATTR_ALIAS_FILT_SLOPE},
    {"NOTCH_FILT_FREQ",         LIBSEGY_TYPE_INT16,  145,   SEGY_ATTR_NOTCH_FILT_FREQ},
    {"NOTCH_FILT_SLOPE",        LIBSEGY_TYPE_INT16,  147,   SEGY_ATTR_NOTCH_FILT_SLOPE},
    {"LOW_CUT_FREQ",            LIBSEGY_TYPE_INT16,  149,   SEGY_ATTR_LOW_CUT_FREQ},
    {"HIGH_CUT_FREQ",           LIBSEGY_TYPE_INT16,  151,   SEGY_ATTR_HIGH_CUT_FREQ},
    {"LOW_CUT_SLOPE",           LIBSEGY_TYPE_INT16,  153,   SEGY_ATTR_LOW_CUT_SLOPE},
    {"HIGH_CUT_SLOPE",          LIBSEGY_TYPE_INT16,  155,   SEGY_ATTR_HIGH_CUT_SLOPE},
    {"YEAR_DATA_REC",           LIBSEGY_TYPE_INT16,  157,   SEGY_ATTR_YEAR_DATA_REC},
    {"DAY_OF_YEAR",             LIBSEGY_TYPE_INT16,  159,   SEGY_ATTR_DAY_OF_YEAR},
    {"HOUR_OF_DAY",             LIBSEGY_TYPE_INT16,  161,   SEGY_ATTR_HOUR_OF_DAY},
    {"MIN_OF_HOUR",             LIBSEGY_TYPE_INT16,  163,   SEGY_ATTR_MIN_OF_HOUR},
    {"SEC_OF_MIN",              LIBSEGY_TYPE_INT16,  165,   SEGY_ATTR_SEC_OF_MIN},
    {"TIME_BASE_CODE",          LIBSEGY_TYPE_INT16,  167,   SEGY_ATTR_TIME_BASE_CODE},
    {"WEIGHTING_FAC",           LIBSEGY_TYPE_INT16,  169,   SEGY_ATTR_WEIGHTING_FAC},
    {"GEOPHONE_GROUP_ROLL1",    LIBSEGY_TYPE_INT16,  171,   SEGY_ATTR_GEOPHONE_GROUP_ROLL1},
    {"GEOPHONE_GROUP_FIRST",    LIBSEGY_TYPE_INT16,  173,   SEGY_ATTR_GEOPHONE_GROUP_FIRST},
    {"GEOPHONE_GROUP_LAST",     LIBSEGY_TYPE_INT16,  175,   SEGY_ATTR_GEOPHONE_GROUP_LAST},
    {"GAP_SIZE",                LIBSEGY_TYPE_INT16,  177,   SEGY_ATTR_GAP_SIZE},
    {"OVER_TRAVEL",             LIBSEGY_TYPE_INT16,  179,   SEGY_ATTR_OVER_TRAVEL},
    {"CDP_X",                   LIBSEGY_TYPE_INT32,  181,   SEGY_ATTR_CDP_X},
    {"CDP_Y",                   LIBSEGY_TYPE_INT32,  185,   SEGY_ATTR_CDP_Y},
    {"INLINE",                  LIBSEGY_TYPE_INT32,  189,   SEGY_ATTR_INLINE},
    {"CROSSLINE",               LIBSEGY_TYPE_INT32,  193,   SEGY_ATTR_CROSSLINE},
    {"SHOT_POINT",              LIBSEGY_TYPE_INT32,  197,   SEGY_ATTR_SHOT_POINT},
    {"SHOT_POINT_SCALAR",       LIBSEGY_TYPE_INT16,  201,   SEGY_ATTR_SHOT_POINT_SCALAR},
    {"MEASURE_UNIT",            LIBSEGY_TYPE_INT16,  203,   SEGY_ATTR_MEASURE_UNIT},
    {"TRANSDUCTION_MANT",       LIBSEGY_TYPE_INT32,  205,   SEGY_ATTR_TRANSDUCTION_MANT},
    {"TRANSDUCTION_EXP",        LIBSEGY_TYPE_INT16,  209,   SEGY_ATTR_TRANSDUCTION_EXP},
    {"TRANSDUCTION_UNIT",       LIBSEGY_TYPE_INT16,  211,   SEGY_ATTR_TRANSDUCTION_UNIT},
    {"DEVICE_ID",               LIBSEGY_TYPE_INT16,  213,   SEGY_ATTR_DEVICE_ID},
    {"SCALAR_TRACE_HEADER",     LIBSEGY_TYPE_INT16,  215,   SEGY_ATTR_SCALAR_TRACE_HEADER},
    {"SOURCE_TYPE",             LIBSEGY_TYPE_INT16,  217,   SEGY_ATTR_SOURCE_TYPE},
    {"SOURCE_ENERGY_DIR_MANT",  LIBSEGY_TYPE_INT32,  219,   SEGY_ATTR_SOURCE_ENERGY_DIR_MANT},//may be wrong
    {"SOURCE_ENERGY_DIR_EXP",   LIBSEGY_TYPE_INT16,  223,   SEGY_ATTR_SOURCE_ENERGY_DIR_EXP},//maybe wrong
    {"SOURCE_MEASURE_MANT",     LIBSEGY_TYPE_INT32,  225,   SEGY_ATTR_SOURCE_MEASURE_MANT},
    {"SOURCE_MEASURE_EXP",      LIBSEGY_TYPE_INT16,  229,   SEGY_ATTR_SOURCE_MEASURE_EXP},
    {"SOURCE_MEASURE_UNIT",     LIBSEGY_TYPE_INT16,  231,   SEGY_ATTR_SOURCE_MEASURE_UNIT},
    {"_UNASSIGNED1",            LIBSEGY_TYPE_INT32,  233,   SEGY_ATTR_UNASSIGNED1},
    {"_UNASSIGNED2",            LIBSEGY_TYPE_INT32,  237,   SEGY_ATTR_UNASSIGNED2}
};


/**
 * ASCII and EBCDIC conversion matrix!
 * */
static const unsigned char a2e[256] = {
    0,  1,  2,  3,  55, 45, 46, 47, 22, 5,  37, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 60, 61, 50, 38, 24, 25, 63, 39, 28, 29, 30, 31,
    64, 79, 127,123,91, 108,80, 125,77, 93, 92, 78, 107,96, 75, 97,
    240,241,242,243,244,245,246,247,248,249,122,94, 76, 126,110,111,
    124,193,194,195,196,197,198,199,200,201,209,210,211,212,213,214,
    215,216,217,226,227,228,229,230,231,232,233,74, 224,90, 95, 109,
    121,129,130,131,132,133,134,135,136,137,145,146,147,148,149,150,
    151,152,153,162,163,164,165,166,167,168,169,192,106,208,161,7,
    32, 33, 34, 35, 36, 21, 6,  23, 40, 41, 42, 43, 44, 9,  10, 27,
    48, 49, 26, 51, 52, 53, 54, 8,  56, 57, 58, 59, 4,  20, 62, 225,
    65, 66, 67, 68, 69, 70, 71, 72, 73, 81, 82, 83, 84, 85, 86, 87,
    88, 89, 98, 99, 100,101,102,103,104,105,112,113,114,115,116,117,
    118,119,120,128,138,139,140,141,142,143,144,154,155,156,157,158,
    159,160,170,171,172,173,174,175,176,177,178,179,180,181,182,183,
    184,185,186,187,188,189,190,191,202,203,204,205,206,207,218,219,
    220,221,222,223,234,235,236,237,238,239,250,251,252,253,254,255
};

static const unsigned char e2a[256] = {
    0,  1,  2,  3,  156,9,  134,127,151,141,142, 11,12, 13, 14, 15,
    16, 17, 18, 19, 157,133,8,  135,24, 25, 146,143,28, 29, 30, 31,
    128,129,130,131,132,10, 23, 27, 136,137,138,139,140,5,  6,  7,
    144,145,22, 147,148,149,150,4,  152,153,154,155,20, 21, 158,26,
    32, 160,161,162,163,164,165,166,167,168,91, 46, 60, 40, 43, 33,
    38, 169,170,171,172,173,174,175,176,177,93, 36, 42, 41, 59, 94,
    45, 47, 178,179,180,181,182,183,184,185,124,44, 37, 95, 62, 63,
    186,187,188,189,190,191,192,193,194,96, 58, 35, 64, 39, 61, 34,
    195,97, 98, 99, 100,101,102,103,104,105,196,197,198,199,200,201,
    202,106,107,108,109,110,111,112,113,114,203,204,205,206,207,208,
    209,126,115,116,117,118,119,120,121,122,210,211,212,213,214,215,
    216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,
    123,65, 66, 67, 68, 69, 70, 71, 72, 73, 232,233,234,235,236,237,
    125,74, 75, 76, 77, 78, 79, 80, 81, 82, 238,239,240,241,242,243,
    92, 159,83, 84, 85, 86, 87, 88, 89, 90, 244,245,246,247,248,249,
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 250,251,252,253,254,255
};

static int 
encode(char* dst, const char* src, const unsigned char* conv, size_t n)
{
    for(size_t i = 0; i < n; ++i )
        dst[ i ] = (char)conv[ (unsigned char) src[ i ] ];
    return LIBSEGY_OK;
}

#include "ebcdic.c"
#include "ibm2ieee.c"

typedef struct {
    int         flag;
    int         type;
    void      * fp;
    char      * text_header;
    void      * bin_header;
    char     ** etextheader;
    int         n_etextheader;  //number of extend text header
    int         history;
    char      * tape_label;
    int         samples;        //samples per trace, 0 for variable length!

    void      * trace_buff;
    int64_t     trace_first;
    int         trace_numb;
    int         trace_size;     //
    int         trace_typesize; //type (format) size
    int64_t     trace_count;
    int64_t     trace_base;      // Where Trace start

    size_t   (* fn_read)(void *, size_t, size_t, void *);
    size_t   (* fn_write)(const void *, size_t, size_t, void *);
    int      (* fn_seek)(void *, long, int);

    int64_t     count;          // Stream read

    ////////////////////////////////
    int         bhdr_samples;
    int         bhdr_format;
    int         bhdr_segy_revision;
    int         bhdr_ext_headers;
    ////////////////////////////////
    int         text_ebdic;

} segy_struct_t;

#define LIBSEGY_FLAG_IO_INITIATED   (1u<<0)

/**
 * Those are examples of creating new streaming function
 * seek is forbiden, everything is only read/write once
 * close and open is done outside of libsegy!
 * */
static size_t
dummy_fread(void *ptr, size_t size, size_t nmemb, void *stream)
{ return fread(ptr, size, nmemb, stream); }

static size_t
dummy_fwrite(const void *ptr, size_t size, size_t nmemb, void *stream)
{ return fwrite(ptr, size, nmemb, stream); }

static int 
dummy_fseek(void *stream, long offset, int whence)
{ return fseek(stream, offset, whence); }

typedef struct {
    size_t (*fn_read)(void*, size_t, size_t, void*);
    size_t (*fn_write)(const void*, size_t, size_t, void*);
    int (*fn_seek)(void *, long, int);
} segy_op_t;

/**
 * @brief Create a READ/WRITE SEGY handle 
 * @param type          : LIBSEGY_READ or LIBSEGY_WRITE
 *
 * TODO: libsegy_init(void *fp, int flag, struct segy_op_t *op)
 * use this function alone to replace both create and init_io.
 *
 * //read sequence--->
 * libsegy_init()
 * libsegy_get_textheader()
 * libsegy_get_bhdr_field()
 * libsegy_init_trace()
 * libsegy_get_attr()
 * libsegy_get_trace()
 * libsegy_close()
 *
 * //write sequence--->
 * libsegy_init()               //create buffer to save all headers
 * libsegy_set_textheader()
 * libsegy_set_bhdr_field()     //need helper function to simplify this!
 * libsegy_init_trace()         //flush header and create trace buffer
 * libsegy_set_attr()           //fill trace attribute
 * libsegy_set_trace()          //fill trace samples
 * libsegy_close()              //flush last batch and close
 * */
int libsegy_getversion(int *major, int *minor)
{
    *major = LIBSEGY_MAJOR;
    *minor = LIBSEGY_MINOR;
    return LIBSEGY_OK;
}

int libsegy_create_struct(libsegy_h *handle, int type, libsegy_io_func_t *op)
{
    segy_struct_t *h;
    h = calloc(1, sizeof(segy_struct_t));
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
    if(type==LIBSEGY_WRITE) {   //allocate text and binary header!
        h->bin_header = calloc(LIBSEGY_SIZE_BINHEAD, sizeof(char));
        h->text_header = calloc(LIBSEGY_SIZE_TEXTHEAD+1, sizeof(char));
    }
    *handle = (libsegy_h)h;
    return LIBSEGY_OK;
}

#include "set_header.c"
#include "get_header.c"
#include "init_io.c"
#include "init_trace.c"

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

#include "get_attr.c"
#include "get_trace.c"
