#ifndef H_BHDR_SEGY
#define H_BHDR_SEGY

typedef struct {
    char        name[32];
    int         type;
    int         pos;
    int         index;  //QC
} segy_field_t;

#define LIBSEGY_TYPE_INT16      2
#define LIBSEGY_TYPE_INT32      3
#define LIBSEGY_TYPE_FLOAT      4
#define LIBSEGY_TYPE_DOUBLE     5

typedef enum { //This enum must be consistent with next
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

//in python access as segy.bhdr.JOB_ID
//make it a class
//tap = segy.create_struct()
//segy.();
//

/*
int
segy_bhdr_get_field(segy_struct_h handle, int field_id, int *field)
{
} */
#endif
