#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>
#include "libsegy.h"

#define LIBSEGY_SIZE_TEXTHEAD   3200
#define LIBSEGY_SIZE_BINHEAD    400
#define LIBSEGY_SIZE_TRACEHEAD  240

#define LIBSEGY_BASE_BINHEADER  3201
#define MIN(a,b)    (a)>(b)?(b):(a)

typedef struct {
    char        name[32];
    int         type;
    int         pos;
    int         index;  //QC
} segy_field_t;

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

int libsegy_getversion(int *major, int *minor)
{
    *major = LIBSEGY_MAJOR;
    *minor = LIBSEGY_MINOR;
    return LIBSEGY_OK;
}

/**
 * //read sequence--->
 * libsegy_init()
 * libsegy_get_textheader()
 * libsegy_get_binheader()
 * libsegy_init_trace()
 * libsegy_get_attr()
 * libsegy_get_trace()
 * libsegy_close()
 *
 * //write sequence--->
 * libsegy_init()               //create buffer to save all headers
 * libsegy_set_textheader()
 * libsegy_set_binheader()     //need helper function to simplify this!
 * libsegy_init_trace()         //flush header and create trace buffer
 * libsegy_set_attr()           //fill trace attribute
 * libsegy_set_trace()          //fill trace samples
 * libsegy_close()              //flush last batch and close
 * */
#include "libsegy_func.c"
#include "libsegy_utils.c"
#include "libsegy_header.c"
#include "libsegy_init.c"
#include "libsegy_init_trace.c"
#include "libsegy_attr.c"
#include "libsegy_get_trace.c"
#include "libsegy_set_trace.c"
