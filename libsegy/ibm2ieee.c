#ifndef C_IBM2IEEE_LIBSEGY
#define C_IBM2IEEE_LIBSEGY

#define IEEEMAX 0x7FFFFFFF
#define IEMAXIB 0x611FFFFF
#define IEMINIB 0x21200000

/*
static inline void ibm_native( void* buf ) {
    static int it[8] = { 0x21800000, 0x21400000, 0x21000000, 0x21000000,
                         0x20c00000, 0x20c00000, 0x20c00000, 0x20c00000 };
    static int mt[8] = { 8, 4, 2, 2, 1, 1, 1, 1 };
    unsigned int manthi, iexp, inabs;
    int ix;
    uint32_t u;

    memcpy( &u, buf, sizeof( u ) );

    manthi = u & 0x00ffffff;
    ix     = manthi >> 21;
    iexp   = ( ( u & 0x7f000000 ) - it[ix] ) << 1;
    manthi = manthi * mt[ix] + iexp;
    inabs  = u & 0x7fffffff;
    if ( inabs > IEMAXIB ) manthi = IEEEMAX;
    manthi = manthi | ( u & 0x80000000 );
    u = ( inabs < IEMINIB ) ? 0 : manthi;
    memcpy( buf, &u, sizeof( u ) );
}

static inline void native_ibm( void* buf ) {
    static int it[4] = { 0x21200000, 0x21400000, 0x21800000, 0x22100000 };
    static int mt[4] = { 2, 4, 8, 1 };
    unsigned int manthi, iexp, ix;
    uint32_t u;

    memcpy( &u, buf, sizeof( u ) );

    ix     = ( u & 0x01800000 ) >> 23;
    iexp   = ( ( u & 0x7e000000 ) >> 1 ) + it[ix];
    manthi = ( mt[ix] * ( u & 0x007fffff) ) >> 3;
    manthi = ( manthi + iexp ) | ( u & 0x80000000 );
    u      = ( u & 0x7fffffff ) ? manthi : 0;
    memcpy( buf, &u, sizeof( u ) );
}

void ibm2ieee( void* to, const void* from ) {
    uint32_t u;
    memcpy( &u, from, sizeof( u ) );
    u = ntohl( u );

    ibm_native( &u );
    memcpy( to, &u, sizeof( u ) );
}

void ieee2ibm( void* to, const void* from ) {
    uint32_t u;
    memcpy( &u, from, sizeof( u ) );

    native_ibm( &u );
    u = htonl( u );
    memcpy( to, &u, sizeof( u ) );
}
*/
#endif
