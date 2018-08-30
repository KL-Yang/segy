#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <png.h>
#include <math.h>
#include "segy_bhdr.h"
#include "segy_thdr.h"
#include "libsegy.h"

#define MIN(a,b)    (a)>(b)?(b):(a)
#define MAX(a,b)    (a)<(b)?(b):(a)

void float2uint8(const float *f, int n, uint8_t *u, float ratio);
int unit_data2png(FILE *fp, const uint8_t *data, int width, int height, int transpose);
