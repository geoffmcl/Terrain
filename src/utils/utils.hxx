// utils.hxx
#ifndef _UTILS_HXX_
#define _UTILS_HXX_
#include <string>
#include <vector>
#ifdef WIN32
#include <direct.h> // for _getcwd()
#endif
#include "slippystuff.hxx"

#ifndef PATH_SEP
#ifdef WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif
#endif // !PATH_SEP
#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifndef MY_EPSILON
#define MY_EPSILON 0.0000001
#endif
#ifndef BAD_LATLON
#define BAD_LATLON 400.0 // just arbitrary out of range value
#endif

#ifndef MIN_SPAN
#define MIN_SPAN 4
#endif

typedef std::vector<std::string> vSTG;
typedef std::vector<short> vSHRT;
typedef std::vector<double> vDBLE;

/* byte swap a 32-bit value */
#define SWAP32(x) { \
   unsigned char _c = ((unsigned char *) (x))[0];\
   ((unsigned char *) (x))[0] = ((unsigned char *) (x))[3];\
   ((unsigned char *) (x))[3] = _c;\
   _c = ((unsigned char *) (x))[1];\
   ((unsigned char *) (x))[1] = ((unsigned char *) (x))[2];\
   ((unsigned char *) (x))[2] = _c; }

/* byte swap a short */
#define SWAP16(x) { \
   unsigned char _c = ((unsigned char *) (x))[0];\
   ((unsigned char *) (x))[0] = ((unsigned char *) (x))[1];\
   ((unsigned char *) (x))[1] = _c; }

/* Inhibit C++ name-mangling for util functions */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define ISDIGIT(a) (( a >= '0' )&&( a <= '9'))
extern int is_an_integer( char *arg );
extern void HexDump(unsigned char *ptr, int length, bool addhdr = false, bool addascii = true, bool addspace = true);

extern int in_world_range(double lat, double lon);
extern unsigned char test_endian();

/////////////////////////////////////////////////
// endian stuff            SOURCE         DESTINATION
extern void get_ix_double(const void *xp, double *ip ); // copy or swap 8 bytes - double
extern void get_ix_int(const void *xp,    int *ip    ); // copy or swap 4 bytes - int
extern void get_ix_float(const void *xp,  float *ip  ); // copy or swap 4 bytes - float
extern void get_ix_short(const void *xp,  short *ip  ); // copy or swap 2 bytes - short
/////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// some timing services
extern double get_seconds();
extern char *get_seconds_stg( double dsecs );
extern char *get_elapsed_stg( double bgn ); // { return get_seconds_stg( bgn - get_seconds() ); }
extern char *Get_Current_UTC_Time_Stg();

//////////////////////////////////////////////////////////////////////
extern char *get_meters_stg( double m );

#ifdef _MSC_VER
extern void win_wsa_perror( char *msg );
//    #define SWRITE(a,b,c) send(a,b,c,0)
//    #define SREAD(a,b,c)  recv(a,b,c,0)
//    #define SCLOSE closesocket
    #define SERROR(a) (a == SOCKET_ERROR)
    #define PERROR(a) win_wsa_perror(a)
#else
//    #define SWRITE write
//    #define SREAD  read
//    #define SCLOSE close
    #define SERROR(a) (a < 0)
    #define PERROR(a) perror(a)
#endif

#ifdef _MSC_VER
#define W64 "I64"
#else
#define W64 "ll"
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

//////////////////////////////////////////////////////////////////////
// try to get lat, lon from file name
extern bool get_hgt_lat_lon( std::string in_file, double *plat, double *plon );
extern bool get_hgt_ilat_ilon( std::string in_file, int *pilat, int *pilon );
extern bool get_srtm_lat_lon( std::string in_file, double *plat, double *plon );
//////////////////////////////////////////////////////////////////////
extern std::string get_hex_str( unsigned char *ucp, int len );
//////////////////////////////////////////////////////////////////////

// added 20140401
#ifndef P64
#ifdef _MSC_VER
#define P64 "%I64u"
#else // !_MSC_VER
#define P64 "%llu"
#endif
#endif
#define nnf_NoClean 0x01

extern int InStr( char *lpb, char *lps );
extern int InStri( char *lpb, char *lps ); // uppercase compare

extern char *get_NiceNumber( char *lpn, int flag = 0 ); // get_nn()
extern char *get_NiceNumberStg( unsigned long num, int flag = 0 );   // get_nn()
extern char *get_NiceNumberStg64( unsigned long long num, int flag = 0 );   // get_nn()

extern char *get_trim_double( double d );
extern bool string_in_vec( vSTG &vs, std::string &s );


extern char *get_hgt_file_name( double dlat, double dlon, int *pilat, int *pilon, bool verb = false );
extern int stat_hgt1_file( double dlat, double dlon, std::string file_dir, std::string &hgt, 
    int *px_off, int *py_off, size_t *poff, bool verb = false );

//////////////////////////////////////////////////////////////
extern std::string get_current_dir();
//////////////////////////////////////////////////////////////
// hgt searching
enum srch_type {
    st_none,
    st_one,
    st_range,
    st_max
};

typedef struct tagLLE {
    double lat,lon;
    short elev;
    int x,y;
}LLE, *PLLE;

typedef std::vector<LLE> vLLE;

typedef struct tagSRCH {
    srch_type type;
    short min_elev;
    short max_elev;
    vLLE vlle;
}SRCH, *PSRCH;

///////////////////////////////////////////////////////////////////

extern vSTG string_split( const std::string &str, const char* sep = 0, int maxsplit = 0 );

extern char * get_k_bytes( long long i64, int type = 0, int dotrim = 1 );
extern char * get_k_num( long long i64, int type = 0, int dotrim = 1 );

extern bool get_hgt3_file_names( PBNDBOX pbb, vSTG &files, bool chkrng = true, bool verb = false );

extern double DistanceFromLine(double cx, double cy,    /* point c */
        double ax, double ay, double bx, double by );   /* line a-b */
///////////////////////////////////////////////////////////////////////
extern unsigned long PowerOfTwoFromString(char *input, int shift);
#define UlongFromBinaryString(str) PowerOfTwoFromString(str, 1)
#define UlongFromHexString(str) PowerOfTwoFromString(str, 4)
///////////////////////////////////////////////////////////////////////
extern bool is_hex_value( char *input ); // test string commences 0x<hex>

#endif // #ifndef _UTILS_HXX_
// eof

