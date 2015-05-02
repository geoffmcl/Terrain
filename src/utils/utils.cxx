// utils.cxx

#include <sys/types.h>
#include <sys/stat.h>
#ifdef _WIN32
#ifdef _MSC_VER
#include <WinSock2.h> // includes <Windows.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <direct.h>
#else
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <math.h> // for pow(), ...
#include <sys/time.h> // for gettimeofday(), ...
#define stricmp strcasecmp
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale>
//#include <iostream>
#include "sprtf.hxx"
#include "utils.hxx"
#include "slippystuff.hxx"
#include "dir_utils.hxx"

#ifdef _MSC_VER
#pragma comment (lib, "Ws2_32.lib")
//#pragma comment (lib, "Mswsock.lib")
//#pragma comment (lib, "AdvApi32.lib")
//#pragma comment (lib, "Winmm.lib") // __imp__timeGetTime@0
#endif

// avoid, if possible
// using namespace std;

static const char *module = "utils";
#ifndef HGT1_SIZE_1
#define HGT1_SIZE_1 3601
#endif
#ifndef HGT_1DEM_SIZ
#define HGT_1DEM_SIZE (HGT1_SIZE_1 * HGT1_SIZE_1 * 2)
#endif

///////////////////////////////////////////////////////////////////////////
// ENDIAN STUFF
// The Intel x86 and x86-64 series of processors use the little-endian format
// Big-endian is the most common convention in data networking (including IPv6), hence its pseudo-synonym network byte order
// The Motorola 6800 and 68k series of processors use the big-endian format
unsigned char test_endian()
{
    int test_var = 1;   // test whether stored as 00 00 00 01 or 01 00 00 00
    unsigned char *test_endian = (unsigned char*)&test_var;
    return (test_endian[0] == 0);
}

static void swap8b(void *dst, const void *src)
{
	char *op = (char *)dst;
	const char *ip = (const char *)src;
#  ifndef FLOAT_WORDS_BIGENDIAN
	op[0] = ip[7];
	op[1] = ip[6];
	op[2] = ip[5];
	op[3] = ip[4];
	op[4] = ip[3];
	op[5] = ip[2];
	op[6] = ip[1];
	op[7] = ip[0];
#  else
	op[0] = ip[3];
	op[1] = ip[2];
	op[2] = ip[1];
	op[3] = ip[0];
	op[4] = ip[7];
	op[5] = ip[6];
	op[6] = ip[5];
	op[7] = ip[4];
#  endif
}

static void swap4b(void *dst, const void *src)
{
	char *op = (char *)dst;
	const char *ip = (const char *)src;
	op[0] = ip[3];
	op[1] = ip[2];
	op[2] = ip[1];
	op[3] = ip[0];
}


void get_ix_double(const void *xp, double *ip) // copy or swap 8 bytes - double
{
#ifdef WORDS_BIGENDIAN
	(void) memcpy(ip, xp, sizeof(double));
#else
	swap8b(ip, xp);
#endif
}

void get_ix_int(const void *xp, int *ip) // copy or swap 4 bytes - int
{
#ifdef WORDS_BIGENDIAN
	(void) memcpy(ip, xp, sizeof(int));
#else
	swap4b(ip, xp);
#endif
}

void get_ix_float(const void *xp, float *ip) // copy or swap 4 bytes - float
{
#ifdef WORDS_BIGENDIAN
	(void) memcpy(ip, xp, sizeof(float));
#else
	swap4b(ip, xp);
#endif
}

void get_ix_short(const void *xp, short *ip) // copy or swap 2 bytes - short
{
	const unsigned char *cp = (const unsigned char *) xp;
#ifdef WORDS_BIGENDIAN
	(void) memcpy(ip, xp, sizeof(short));
#else
	*ip = *cp++ << 8;
	*ip |= *cp; 
#endif
}


// END ENDIAN STUFF
////////////////////////////////////////////////////////////////////////////////

int is_an_integer( char *arg )
{
    int len = (int)strlen(arg);
    if (!len) return 0;
    int i, c;
    for (i = 0; i < len; i++) {
        c = arg[i];
        if (!ISDIGIT(c))
            return 0;
    }
    return 1;   // is a positive integer
}

#ifndef _MSC_VER
/*
 * need a replacement for _itoa()
 * from : http://www.jb.man.ac.uk/~slowe/cpp/itoa.html
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 */
char * _itoa(int value, char* result, int base)
{
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }
	
    char *ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;
	
    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );
	
    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}
#endif // !_MSC_VER

/////////////////////////////////////////////////////////////////////////
// Number of hex values displayed per line
#define HEX_DUMP_WIDTH 16
//
// Dump a region of memory in a hexadecimal format
//
void HexDump(unsigned char *ptr, int length, bool addhdr, bool addascii, bool addspace)
{
    char buffer[256];
    char *buffPtr, *buffPtr2;
    unsigned cOutput, i;
    int bytesToGo = length;

    while ( bytesToGo  )
    {
        memset(buffer,0,256);
        cOutput = bytesToGo >= HEX_DUMP_WIDTH ? HEX_DUMP_WIDTH : bytesToGo;

#if 0 // chop this
        if( Try_HD_Width( ptr, cOutput ) )
        {
           sprtf( "WARNING: Abandoning HEX DUMP, bad ptr %p, length %d!\n",
               ptr, cOutput );
            return;
        }
        if (add_2_ptrlist(ptr)) {
            bytesToGo -= cOutput;
            ptr += HEX_DUMP_WIDTH;
            continue;
        }
#endif // 0 - chopped code

        buffPtr = buffer;
        if (addhdr) {
            buffPtr += sprintf(buffPtr, "%08X:  ", length-bytesToGo );
        }
        buffPtr2 = buffPtr + (HEX_DUMP_WIDTH * 3) + 1;
        *buffPtr2 = 0;

        for ( i=0; i < HEX_DUMP_WIDTH; i++ )
        {
            unsigned char value = *(ptr+i);

            if ( i >= cOutput )
            {
                // On last line.  Pad with spaces
                *buffPtr++ = ' ';
                *buffPtr++ = ' ';
                *buffPtr++ = ' ';
            }
            else
            {
                if ( value < 0x10 )
                {
                    *buffPtr++ = '0';
                    _itoa( value, buffPtr++, 16);
                }
                else
                {
                    _itoa( value, buffPtr, 16);
                    buffPtr+=2;
                }
 
                *buffPtr++ = ' ';
                if (addascii) {
                    *buffPtr2++ = isprint(value) ? value : '.';
                    if ( value == '%' )
                       *buffPtr2++ = '%';  // insert another
                }
            }
            
            // Put an extra space between the 1st and 2nd half of the bytes
            // on each line.
            if (addspace) {
                if ( i == (HEX_DUMP_WIDTH/2)-1 )
                    *buffPtr++ = ' ';
            }
        }

        *buffPtr2 = 0;  // Null terminate it.
        //puts(buffer);   // Can't use sprtf(), since there may be a '%'
                        // in the string.
        SPRTF("%s\n", buffer);   // Have add 2nd % if one found in ASCII

        bytesToGo -= cOutput;
        ptr += HEX_DUMP_WIDTH;
    }
}

////////////////////////////////////////////////////////////////////////////

int in_world_range(double lat, double lon)
{
    if ((lat >= -90.0) && (lat <= 90.0) &&
        (lon >= -180.0) && (lon <= 180.0)) {
            return 1;
    }
    return 0;
}


// get HGT lat, lon from file name
// file MUST be of form [N|S][12][E|W][123] - case ignored
bool get_hgt_ilat_ilon( std::string in_file, int *pilat, int *pilon )
{
    int ns, lat, ew, lon;
    double dlat, dlon;
    std::string file = get_file_name_only(in_file);
    std::string tmp;
    if ( !(file.size() >= (1+2+1+3)) ) {
        SPRTF("Invalid file name len %d! Should be 7 plus\n", (int)file.size());
        return false;
    }

    tmp = file.substr(0,1); // get first letter
    if (stricmp(tmp.c_str(),"N") == 0) {
        ns = 0;
    } else if (stricmp(tmp.c_str(),"S") == 0) {
        ns = 1;
    } else {
        SPRTF("Name does not begin with N|S!\n");
        return false;
    }
    tmp = file.substr(1,2); // get LAT
    lat = atoi(tmp.c_str());

    tmp = file.substr(3,1); // get E/W
    if (stricmp(tmp.c_str(),"E") == 0) {
        ew = 0;
    } else if (stricmp(tmp.c_str(),"W") == 0) {
        ew = 1;
    } else {
        SPRTF("Name does not have with E|W!\n");
        return false;
    }
    tmp = file.substr(4,3); // get LON

    lon = atoi(tmp.c_str());
    if (ns) lat *= -1;
    if (ew) lon *= -1;
    dlat = (double)lat;
    dlon = (double)lon;
    if (!in_world_range(dlat,dlon)) {
        SPRTF("lat %lf, lon %lf not in world range\n", dlat, dlon );
        return false;
    }
    *pilat = lat;
    *pilon = lon;
    return true;
}

bool get_hgt_lat_lon( std::string in_file, double *plat, double *plon )
{
    bool bret = false;
    int ilat, ilon;
    double dlat, dlon;
    if ( get_hgt_ilat_ilon( in_file, &ilat, &ilon ) ) {
        dlat = (double)ilat;
        dlon = (double)ilon;
        if (!in_world_range(dlat,dlon)) {
            SPRTF("lat %lf, lon %lf not in world range\n", dlat, dlon );
        } else {
            *plat = dlat;
            *plon = dlon;
            bret = true;
        }
    }
    return bret;
}


// get SRTM lon, lat from file name
// file MUST be of form [E|W][123][N|S][12] - case ignored
bool get_srtm_lat_lon( std::string in_file, double *plat, double *plon )
{
    int ns, lat, ew, lon;
    double dlat, dlon;
    std::string file = get_file_name_only(in_file);
    std::string tmp;
    if ( !(file.size() >= (1+2+1+3)) ) {
        SPRTF("Invalid file name len %d! Should be 7 plus\n", (int)file.size());
        return false;
    }

    tmp = file.substr(0,1); // get first letter
    if (stricmp(tmp.c_str(),"E") == 0) {
        ew = 0;
    } else if (stricmp(tmp.c_str(),"W") == 0) {
        ew = 1;
    } else {
        SPRTF("Name does not begin with E|W!\n");
        return false;
    }
    tmp = file.substr(1,3);
    lon = atoi(tmp.c_str());

    tmp = file.substr(4,1);
    if (stricmp(tmp.c_str(),"N") == 0) {
        ns = 0;
    } else if (stricmp(tmp.c_str(),"S") == 0) {
        ns = 1;
    } else {
        SPRTF("Name does not have with N|S!\n");
        return false;
    }
    tmp = file.substr(5,2);
    lat = atoi(tmp.c_str());

    if (ns) lat *= -1;
    if (ew) lon *= -1;

    dlat = (double)lat;
    dlon = (double)lon;

    if (!in_world_range(dlat,dlon)) {
        SPRTF("lat %lf, lon %lf not in world range\n", dlat, dlon );
        return false;
    }

    *plat = dlat;
    *plon = dlon;

    return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
std::string get_hex_str( unsigned char *ucp, int len )
{
    char buf[16*4];
    char *cp = buf;
    *cp = 0;
    int i, max = 0, off = 0;
    unsigned char val;
    std::string str;
    while (len) {
        if (len > 16)
            max = 16;
        else
            max = len;
        len -= max;
        *cp = 0;
        if (str.size() > 0)
            str += "\n";
        for (i = 0; i < max; i++) {
            if (*cp)
                strcat(cp," ");
            val = (ucp[i+off] & 0x0FF);
            sprintf(EndBuf(cp),"%02X", val);
        }
        off += max;
        str += cp;
    }
    return str;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// added 20140401

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : InStr
// Return type: INT 
// Arguments  : LPTSTR lpb
//            : LPTSTR lps
// Description: Return the position of the FIRST instance of the string in lps
//              Emulates the Visual Basic function.
///////////////////////////////////////////////////////////////////////////////

int InStr( char *lpb, char *lps )
{
   int   iRet = 0;
   int   i, j, k, l, m;
   char  c;
   i = (int)strlen(lpb);
   j = (int)strlen(lps);
   if( i && j && ( i >= j ) )
   {
      c = *lps;   // get the first we are looking for
      l = i - ( j - 1 );   // get the maximum length to search
      for( k = 0; k < l; k++ )
      {
         if( lpb[k] == c )
         {
            // found the FIRST char so check until end of compare string
            for( m = 1; m < j; m++ )
            {
               if( lpb[k+m] != lps[m] )   // on first NOT equal
                  break;   // out of here
            }
            if( m == j )   // if we reached the end of the search string
            {
               iRet = k + 1;  // return NUMERIC position (that is LOGICAL + 1)
               break;   // and out of the outer search loop
            }
         }
      }  // for the search length
   }
   return iRet;
}
int InStri( char *lpb, char *lps )
{
   int   iRet = InStr(lpb,lps);
   if (iRet)
       return iRet;
   int   i, j, k, l, m;
   char  c, d;
   i = (int)strlen(lpb);
   j = (int)strlen(lps);
   if( i && j && ( i >= j ) )
   {
      c = toupper(*lps);   // get the first we are looking for
      l = i - ( j - 1 );   // get the maximum length to search
      for( k = 0; k < l; k++ )
      {
         d = toupper(lpb[k]);
         if( d == c )
         {
            // found the FIRST char so check until end of compare string
            for( m = 1; m < j; m++ )
            {
               if( toupper(lpb[k+m]) != toupper(lps[m]) )   // on first NOT equal
                  break;   // out of here
            }
            if( m == j )   // if we reached the end of the search string
            {
               iRet = k + 1;  // return NUMERIC position (that is LOGICAL + 1)
               break;   // and out of the outer search loop
            }
         }
      }  // for the search length
   }
   return iRet;
}


// return nice number - with comas - get_nn()
char *get_NiceNumber( char *lpn, int flag ) // get_nn()
{
    size_t i, j, k;
    char *lpr = GetNxtBuf();
    *lpr = 0;
    i = strlen( lpn );
	k = 0;
	if( i ) {
        if( i > 3 ) {
			for( j = 0; j < i; j++ ) {
            // FIX20070923 - avoid adding FIRST comma
				if( k && ( ( (i - j) % 3 ) == 0 ) )
					lpr[k++] = ',';
				lpr[k++] = lpn[j];
			}
			lpr[k] = 0;
		} else {
			strcpy( lpr, lpn );
		}
    }
    if (k && !(flag & nnf_NoClean)) {
        i = InStr(lpr,(char *)".");
        if (i) {
            while (k--) {
                if (lpr[k] == '.') {
                    lpr[k] = 0;
                    break;
                } else if (lpr[k] == '0') {
                    if (k > 0)
                        lpr[k] = 0;
                } else
                    break;
            }
        }
    }

    return lpr;
}

// given a DWORD number, get nice number in buffer - get_nn()
char *get_NiceNumberStg( unsigned long num, int flag )   // get_nn()
{
   char * lpr = GetNxtBuf();
   sprintf(lpr, "%lu", num);
   return (get_NiceNumber(lpr, flag));
}

char *get_NiceNumberStg64( unsigned long long num, int flag )   // get_nn()
{
   char * lpr = GetNxtBuf();
   sprintf(lpr, P64, num);
   return (get_NiceNumber(lpr,flag));
}

///////////////////////////////////////////////////////////////////////////////////////////////////


char *get_trim_double( double d )
{
   char *cp = GetNxtBuf();
   sprintf(cp,"%lf",d);
   if (!InStr(cp,(char *)"."))
       return cp;
   int c;
   size_t ii, max = strlen(cp);
   for (ii = max - 1; ii > 0; ii--) {
       c = cp[ii];
       if (c == '.') {
           cp[ii] = 0;
           break;
       } else if (c == '0') {
           cp[ii] = 0;
       } else
           break;
   }
   return cp;
}

bool string_in_vec( vSTG &vs, std::string &s )
{
    size_t ii, max = vs.size();
    for (ii = 0; ii < max; ii++) {
        if (s == vs[ii])
            return true;
    }
    return false;
}


//////////////////////////////////////////////////////////////////////
// some timing services
#define USE_PERF_COUNTER
#if (defined(WIN32) && defined(USE_PERF_COUNTER))
// QueryPerformanceFrequency( &frequency ) ;
// QueryPerformanceCounter(&timer->start) ;
double get_seconds()
{
    static double dfreq;
    static bool done_freq = false;
    static bool got_perf_cnt = false;
    if (!done_freq) {
        LARGE_INTEGER frequency;
        if (QueryPerformanceFrequency( &frequency )) {
            got_perf_cnt = true;
            dfreq = (double)frequency.QuadPart;
        }
        done_freq = true;
    }
    double d;
    if (got_perf_cnt) {
        LARGE_INTEGER counter;
        QueryPerformanceCounter (&counter);
        d = (double)counter.QuadPart / dfreq;
    }  else {
        DWORD dwd = GetTickCount(); // milliseconds that have elapsed since the system was started
        d = (double)dwd / 1000.0;
    }
    return d;
}

#else // !WIN32
double get_seconds()
{
    struct timeval tv;
    gettimeofday(&tv,0);
    double t1 = (double)(tv.tv_sec+((double)tv.tv_usec/1000000.0));
    return t1;
}
#endif // WIN32 y/n

// see : http://geoffair.org/misc/powers.htm
char *get_seconds_stg( double dsecs )
{
    //static char _s_secs_buf[256];
    //char *cp = _s_secs_buf;
    char *cp = GetNxtBuf();
    sprintf(cp,"%g",dsecs);
    if (dsecs < 0.0) {
        strcpy(cp,"?NEG? secs");
    } else if (dsecs < 0.0000000000001) {
        strcpy(cp,"~0 secs");
    } else if (dsecs < 0.000000005) {
        // nano- n 10 -9 * 
        dsecs *= 1000000000.0;
        sprintf(cp,"%.3f nano-secs",dsecs);
    } else if (dsecs < 0.000005) {
        // micro- m 10 -6 * 
        dsecs *= 1000000.0;
        sprintf(cp,"%.3f micro-secs",dsecs);
    } else if (dsecs <  0.005) {
        // milli- m 10 -3 * 
        dsecs *= 1000.0;
        sprintf(cp,"%.3f milli-secs",dsecs);
    } else if (dsecs < 60.0) {
        sprintf(cp,"%.3f secs",dsecs);
    } else {
        int mins = (int)(dsecs / 60.0);
        dsecs -= (double)mins * 60.0;
        if (mins < 60) {
            sprintf(cp,"%d:%.3f min:secs", mins, dsecs);
        } else {
            int hrs = mins / 60;
            mins -= hrs * 60;
            sprintf(cp,"%d:%02d:%.3f hrs:min:secs", hrs, mins, dsecs);
        }
    }
    return cp;
}

char *get_elapsed_stg( double bgn ) { return get_seconds_stg( get_seconds() - bgn ); }

char *get_hgt_file_name( double lat, double lon, int *pilat, int *pilon, bool verb )
{
    if (!in_world_range(lat,lon)) {
        if (verb) SPRTF("%s: Given lat %s, and/or lon %s NOT in world range!\n", module,
            get_trim_double(lat),
            get_trim_double(lon) );
        return 0;
    }
    double dlat = lat;
    double dlon = lon;
    char ns = 'N';
    char ew = 'E';
    int ilat = (int)dlat;
    int ilon = (int)dlon;
    if (dlat < 0.0) {
        ns = 'S';
        dlat *= -1.0;
        if ((dlat - (double)((int)dlat)) > 0.0)
            dlat += 1.0;
        ilat = (int)dlat;
    }
    if (dlon < 0.0) {
        ew = 'W';
        dlon *= -1.0;
        if ((dlon - (double)((int)dlon)) > 0.0)
            dlon += 1.0;
        ilon = (int)dlon;
    }

    char *cp = GetNxtBuf();
    sprintf(cp,"%c%02d%c%03d.hgt", ns, ilat, ew, ilon);
    if (pilat)
        *pilat = ilat;
    if (pilon)
        *pilon = ilon;
    if (verb) SPRTF("%s: lat,lon %s,%s is in file %s.\n", module,
        get_trim_double(lat),
        get_trim_double(lon),
        cp );
    return cp;
}

// get ALL the HGT 3" files required to satisfy this bounding box
bool get_hgt3_file_names( PBNDBOX pbb, vSTG &files, bool chkrng, bool verb )
{
    bool bret = true;
    char *cp;
    int ilat,ilon;
    std::string f;
    double lat,lon;
    int count = 0;
    if (chkrng) {
        if (!in_world_range(pbb->bb_min_lat,pbb->bb_min_lon)) {
            if (verb) {
                SPRTF("%s: BBOX min lat,lon %s,%s out of WORLD range\n", module,
                    get_trim_double(pbb->bb_min_lat),
                    get_trim_double(pbb->bb_min_lon));
            }
            bret = false;
        }
        if (!in_world_range(pbb->bb_max_lat,pbb->bb_max_lon)) {
            if (verb) {
                SPRTF("%s: BBOX max lat,lon %s,%s out of WORLD range\n", module,
                    get_trim_double(pbb->bb_max_lat),
                    get_trim_double(pbb->bb_max_lon));
            }
            bret = false;
        }
        if ( pbb->bb_min_lat >= pbb->bb_max_lat ) {
            if (verb) {
                SPRTF("%s: min_lat %s GTE max_lat %s\n", module,
                    get_trim_double(pbb->bb_min_lat),
                    get_trim_double(pbb->bb_max_lat));
            }
            bret = false;
        }
        if ( pbb->bb_min_lon >= pbb->bb_max_lon ) {
            if (verb) {
                SPRTF("%s: min_lon %s GTE max_lon %s\n", module,
                    get_trim_double(pbb->bb_min_lon),
                    get_trim_double(pbb->bb_max_lon));
            }
            bret = false;
        }
        if (!bret)
            return bret;
    }   // if (chkrng)

    // 20140520: get the file name SILENTLY
    // BL - min_lat, min_lon
    // cp = get_hgt_file_name( pbb->bb_min_lat, pbb->bb_min_lon, &ilat, &ilon, verb );
    lat = pbb->bb_min_lat;
    lon = pbb->bb_min_lon;
    count++;
    cp = get_hgt_file_name( lat, lon, &ilat, &ilon, false );
    if (!cp) {
        if (verb) {
            SPRTF("%s: Failed to get file name for lat,lon %s,%s\n", module,
                get_trim_double(lat),
                get_trim_double(lon));
        }
        return false;
    }
    f = cp;
    if (!string_in_vec( files, f )) {
        if (verb) {
            SPRTF("%s: %d: BL min_lat,min_lon %s,%s is in file %s\n", module,
                count,
                get_trim_double(lat),
                get_trim_double(lon),
                cp);
        }
        files.push_back(f);
    }
    // TR - max_lat, max_lon
    // cp = get_hgt_file_name( pbb->bb_max_lat, pbb->bb_max_lon, &ilat, &ilon, verb );
    lat = pbb->bb_max_lat;
    lon = pbb->bb_max_lon;
    count++;
    cp = get_hgt_file_name( lat, lon, &ilat, &ilon, false );
    if (!cp) {
        if (verb) {
            SPRTF("%s: Failed to get file name for lat,lon %s,%s\n", module,
                get_trim_double(lat),
                get_trim_double(lon));
        }
        return false;
    }
    if (strcmp(cp,f.c_str())) {
        // different files, so must check more
        f = cp;
        if (!string_in_vec( files, f )) {
            if (verb) {
                SPRTF("%s: %d: TR max_lat,max_lon %s,%s, is in file %s\n", module,
                    count,
                    get_trim_double(lat),
                    get_trim_double(lon),
                    cp);
            }
            files.push_back(f);
        }
        // need to check for MORE files
        double lon,lat,min_lat,min_lon,max_lat,max_lon;
        if (pbb->bb_min_lat < pbb->bb_max_lat) {
            min_lat = pbb->bb_min_lat;
            max_lat = pbb->bb_max_lat;
        } else {
            min_lat = pbb->bb_max_lat;
            max_lat = pbb->bb_min_lat;
        }
        if (pbb->bb_min_lon < pbb->bb_max_lon) {
            min_lon = pbb->bb_min_lon;
            max_lon = pbb->bb_max_lon;
        } else {
            min_lon = pbb->bb_max_lon;
            max_lon = pbb->bb_min_lon;
        }
        for (lat = min_lat; lat <= max_lat; lat += 0.5) {
            for (lon = min_lon; lon <= max_lon; lon += 0.5) {
                count++;
                //cp = get_hgt_file_name( lat, lon, &ilat, &ilon, verb );
                cp = get_hgt_file_name( lat, lon, &ilat, &ilon, false );
                if (!cp) {
                    if (verb) {
                        SPRTF("%s: Failed to get file name for lat,lon %s,%s\n", module,
                            get_trim_double(pbb->bb_max_lat),
                            get_trim_double(pbb->bb_max_lon));
                    }
                    return false;
                }
                f = cp;
                if (!string_in_vec( files, f )) {
                    if (verb) {
                        SPRTF("%s: %d: lat,lon %s,%s, is in file %s\n", module,
                            count,
                            get_trim_double(lat),
                            get_trim_double(lon),
                            cp);
                    }
                    files.push_back(f);
                }
            }
        }

        // seem to sometimes MISS a file or 2??? So try adding
        // BR - min_lat, max_lon
        count++;
        lat = pbb->bb_min_lat;
        lon = pbb->bb_max_lon;
        cp = get_hgt_file_name( lat, lon, &ilat, &ilon, false );
        if (!cp) {
            if (verb) {
                SPRTF("%s: Failed to get file name for lat,lon %s,%s\n", module,
                    get_trim_double(pbb->bb_min_lat),
                    get_trim_double(pbb->bb_max_lon));
            }
            return false;
        }
        f = cp;
        if (!string_in_vec( files, f )) {
            if (verb) {
                SPRTF("%s: %d: BR min_lat,max_lon %s,%s, is in file %s\n", module,
                    count,
                    get_trim_double(lat),
                    get_trim_double(lon),
                    cp);
            }
            files.push_back(f);
        }

        // TL - max_lat, min_lon
        count++;
        lat = pbb->bb_max_lat;
        lon = pbb->bb_min_lon;
        cp = get_hgt_file_name( lat, lon, &ilat, &ilon, false );
        if (!cp) {
            if (verb) {
                SPRTF("%s: Failed to get file name for lat,lon %s,%s\n", module,
                    get_trim_double(lat),
                    get_trim_double(lon));
            }
            return false;
        }
        f = cp;
        if (!string_in_vec( files, f )) {
            if (verb) {
                SPRTF("%s: %d: TL max_lat,min_lon %s,%s, is in file %s\n", module,
                    count,
                    get_trim_double(lat),
                    get_trim_double(lon),
                    cp);
            }
            files.push_back(f);
        }

    }
    return bret;
}


int stat_hgt1_file( double dlat, double dlon, std::string file_dir, std::string &hgt, 
    int *px_off, int *py_off, size_t *poff, bool verb )
{
    int ilat,ilon;
    char *cp = get_hgt_file_name( dlat, dlon, &ilat, &ilon, verb );
    if (!cp) {
        if (verb) SPRTF("%s: Failed to get HGT file name of lat,lon %s,%s\n", module,
            get_trim_double(dlat),
            get_trim_double(dlon) );
        return 1;   // FAILED: No file name
    }

    hgt = file_dir;
    if (hgt.size() > 0)
        hgt += PATH_SEP;
    hgt += cp;

    if (is_file_or_directory((char *)hgt.c_str()) != 1) {
        if (verb) SPRTF("%s: Unable to 'stat' file %s!\n", module, hgt.c_str());
        if (verb && (file_dir.size() == 0)) {
            SPRTF("%s: Since no file directory given can only search cwd [%s]\n", module,
                get_current_dir().c_str() );
            SPRTF("%s: Use hgt2File::set_hgt_dir(dir) if they ae in a different source.\n", module);
        }
        return 2;   // FAILED: Could NOT find file
    }
    size_t file_size = get_last_file_size();
    if (file_size != HGT_1DEM_SIZE) {
        if (verb) SPRTF("%s: Incorrect file size! got %d, should be %d\n", module,
            (int)file_size, HGT_1DEM_SIZE);
        return 3;   // FAILED: Not correct size
    }

    double dx,dy;
    //double dx = lon - (double)ilon; // index
    //double dy = lat - (double)ilat;
    if (dlon < 0.0) {
        dx = dlon + (double)ilon;
    } else {
        dx = dlon - (double)ilon;
    }
    if (dlat < 0.0) {
        dy = 1.0 - dlat - (double)ilat;
    } else {
        dy = 1.0 - (dlat - (double)ilat);
    }
    int x_off = (int)(dx * (double)HGT1_SIZE_1);
    int y_off = (int)(dy * (double)HGT1_SIZE_1);
    if (x_off >= HGT1_SIZE_1)
        x_off = HGT1_SIZE_1 - 1;
    if (y_off >= HGT1_SIZE_1)
        y_off = HGT1_SIZE_1 - 1;
    if (px_off)
        *px_off = x_off;
    if (py_off)
        *py_off = y_off;
    size_t offset = ((y_off * HGT1_SIZE_1) + x_off) * 2;
    if (poff)
        *poff = offset;

    return 0;   // SUCCESS: And have returned FILE NAME, and offsets

}


char *get_meters_stg( double m )
{
    int i;
    char *cp = GetNxtBuf();
    if ( m < 0.0 ) {
        strcpy(cp,"?NEG? m");
    } else if (m == 0.0) {
        strcpy(cp,"0 m");
    } else if (m <= MY_EPSILON) {
        strcpy(cp,"~0 m");
    } else if (m < 0.01) {
        m *= 1000.0;
        i = (int)(m * 10.0);
        sprintf(cp,"%s mm", get_trim_double((double)i / 10.0));
    } else if (m < 0.1) {
        m *= 100.0;
        i = (int)(m * 10.0);
        sprintf(cp,"%s cm", get_trim_double((double)i / 10.0));
    } else if (m < 1000.0) {
        i = (int)(m * 10.0);
        sprintf(cp,"%s m", get_trim_double((double)i / 10.0));
    } else {
        m /= 1000.0;
        i = (int)(m * 10.0);
        sprintf(cp,"%s Km", get_trim_double((double)i / 10.0));
    }
    return cp;
}

//////////////////////////////////////////////////////
#ifdef _MSC_VER

// get a message from the system for this error value
char *get_errmsg_text( int err )
{
    LPSTR ptr = 0;
    DWORD fm = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&ptr, 0, NULL );
    if (ptr) {
        size_t len = strlen(ptr);
        while(len--) {
            if (ptr[len] > ' ') break;
            ptr[len] = 0;
        }
        if (len) return ptr;
        LocalFree(ptr);
    }
    return 0;
}

void win_wsa_perror( char *msg )
{
    int err = WSAGetLastError();
    LPSTR ptr = get_errmsg_text(err);
    if (ptr) {
        SPRTF("%s = %s (%d)\n", msg, ptr, err);
        LocalFree(ptr);
    } else {
        SPRTF("%s %d\n", msg, err);
    }
}

#endif // _MSC_VER
//////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
const char *ts_form = "%04d-%02d-%02d %02d:%02d:%02d";
// Creates the UTC time string
char *Get_UTC_Time_Stg(time_t Timestamp)
{
    char *ps = GetNxtBuf();
    tm  *ptm;
    ptm = gmtime (& Timestamp);
    sprintf (
        ps,
        ts_form,
        ptm->tm_year+1900,
        ptm->tm_mon+1,
        ptm->tm_mday,
        ptm->tm_hour,
        ptm->tm_min,
        ptm->tm_sec );
    return ps;
}

char *Get_Current_UTC_Time_Stg()
{
    time_t Timestamp = time(0);
    return Get_UTC_Time_Stg(Timestamp);
}
////////////////////////////////////////////////////////////////
	/**
	 * 
	 */
static vSTG split_whitespace( const std::string& str, int maxsplit )
{
    vSTG result;
	std::string::size_type len = str.length();
	std::string::size_type i = 0;
	std::string::size_type j;
	int countsplit = 0;

	while (i < len) {
    	while ((i < len) && isspace((unsigned char)str[i])) {
    		++i;
	    }

    	j = i;

    	while ((i < len) && !isspace((unsigned char)str[i])) {
            ++i;
	    }

    	if (j < i) {
            result.push_back( str.substr(j, i-j) );
		    ++countsplit;
		    while ((i < len) && isspace((unsigned char)str[i])) {
		        ++i;
		    }

		    if (maxsplit && (countsplit >= maxsplit) && (i < len)) {
        		result.push_back( str.substr( i, (len - i) ) );
		        i = len;
		    }
	    }
	}
	return result;
}

/**
  * split a string per a separator - if no sep, use space - split_whitespace 
  */
vSTG string_split( const std::string& str, const char* sep, int maxsplit )
{
    if (sep == 0)
        return split_whitespace( str, maxsplit );

	vSTG result;
	int n = (int)strlen( sep );
	if (n == 0) {
	    // Error: empty separator string
	    return result;
	}
	const char* s = str.c_str();
	std::string::size_type len = str.length();
	std::string::size_type i = 0;
	std::string::size_type j = 0;
	int splitcount = 0;

	while ((i + n) <= len) {
    	if ((s[i] == sep[0]) && (n == 1 || memcmp(s+i, sep, n) == 0)) {
            result.push_back( str.substr(j,i-j) );
		    i = j = i + n;
		    ++splitcount;
		    if (maxsplit && (splitcount >= maxsplit))
                break;
	    } else {
		    ++i;
	    }
	}
	result.push_back( str.substr(j,len-j) );
	return result;
}

///////////////////////////////////////////////////////////////
#ifndef SPRINTF
#define SPRINTF sprintf
#endif
#ifndef STRLEN
#define STRLEN strlen
#endif
#ifndef STRCAT
#define STRCAT strcat
#endif
void trim_float_buf( char *pb )
{
   size_t len = STRLEN(pb);
   size_t i, dot, zcnt;
   for( i = 0; i < len; i++ )
   {
      if( pb[i] == '.' )
         break;
   }
   dot = i + 1; // keep position of decimal point (a DOT)
   zcnt = 0;
   for( i = dot; i < len; i++ )
   {
      if( pb[i] != '0' )
      {
         i++;  // skip past first
         if( i < len )  // if there are more chars
         {
            i++;  // skip past second char
            if( i < len )
            {
               size_t i2 = i + 1;
               if( i2 < len )
               {
                  if ( pb[i2] >= '5' )
                  {
                     if( pb[i-1] < '9' )
                     {
                        pb[i-1]++;
                     }
                  }
               }
            }
         }
         pb[i] = 0;
         break;
      }
      zcnt++;     // count ZEROS after DOT
   }
   if( zcnt == (len - dot) )
   {
      // it was ALL zeros
      pb[dot - 1] = 0;
   }
}

char * get_k_bytes( long long i64, int type, int dotrim )
{
   char *pb = GetNxtBuf();
   const char *form = " bytes";
   unsigned long long byts = i64;
   double res;
   const char* ffm = "%0.20f";  // get 20 digits
   if( byts < 1024 ) {
      SPRINTF(pb, P64 , byts);
      dotrim = 0;
   } else if( byts < 1024*1024 ) {
      res = ((double)byts / 1024.0);
      form = (type ? " KiloBypes" : " KB");
      SPRINTF(pb, ffm, res);
   } else if( byts < 1024*1024*1024 ) {
      res = ((double)byts / (1024.0*1024.0));
      form = (type ? " MegaBypes" : " MB");
      SPRINTF(pb, ffm, res);
   } else { // if( byts <  (1024*1024*1024*1024)){
      double val = (double)byts;
      double db = (1024.0*1024.0*1024.0);   // x3
      double db2 = (1024.0*1024.0*1024.0*1024.0);   // x4
      if( val < db2 )
      {
         res = val / db;
         form = (type ? " GigaBypes" : " GB");
         SPRINTF(pb, ffm, res);
      }
      else
      {
         db *= 1024.0;  // x4
         db2 *= 1024.0; // x5
         if( val < db2 )
         {
            res = val / db;
            form = (type ? " TeraBypes" : " TB");
            SPRINTF(pb, ffm, res);
         }
         else
         {
            db *= 1024.0;  // x5
            db2 *= 1024.0; // x6
            if( val < db2 )
            {
               res = val / db;
               form = (type ? " PetaBypes" : " PB");
               SPRINTF(pb, ffm, res);
            }
            else
            {
               db *= 1024.0;  // x6
               db2 *= 1024.0; // x7
               if( val < db2 )
               {
                  res = val / db;
                  form = (type ? " ExaBypes" : " EB");
                  SPRINTF(pb, ffm, res);
               }
               else
               {
                  db *= 1024.0;  // x7
                  res = val / db;
                  form = (type ? " ZettaBypes" : " ZB");
                  SPRINTF(pb, ffm, res);
               }
            }
         }
      }
   }
   if( dotrim > 0 )
      trim_float_buf(pb);

   STRCAT(pb, form);

   return pb;
}

char * get_k_num( long long i64, int type, int dotrim )
{
   char *pb = GetNxtBuf();
   const char *form = "";
   unsigned long long byts = i64;
   double res;
   const char* ffm = "%0.20f";  // get 20 digits
   if( byts < 1024 ) {
      SPRINTF(pb, P64 , byts);
      dotrim = 0;
   } else if( byts < 1024*1024 ) {
      res = ((double)byts / 1024.0);
      form = (type ? " Kilo" : " K");
      SPRINTF(pb, ffm, res);
   } else if( byts < 1024*1024*1024 ) {
      res = ((double)byts / (1024.0*1024.0));
      form = (type ? " Mega" : " M");
      SPRINTF(pb, ffm, res);
   } else { // if( byts <  (1024*1024*1024*1024)){
      double val = (double)byts;
      double db = (1024.0*1024.0*1024.0);   // x3
      double db2 = (1024.0*1024.0*1024.0*1024.0);   // x4
      if( val < db2 )
      {
         res = val / db;
         form = (type ? " Giga" : " G");
         SPRINTF(pb, ffm, res);
      }
      else
      {
         db *= 1024.0;  // x4
         db2 *= 1024.0; // x5
         if( val < db2 )
         {
            res = val / db;
            form = (type ? " Tera" : " T");
            SPRINTF(pb, ffm, res);
         }
         else
         {
            db *= 1024.0;  // x5
            db2 *= 1024.0; // x6
            if( val < db2 )
            {
               res = val / db;
               form = (type ? " Peta" : " P");
               SPRINTF(pb, ffm, res);
            }
            else
            {
               db *= 1024.0;  // x6
               db2 *= 1024.0; // x7
               if( val < db2 )
               {
                  res = val / db;
                  form = (type ? " Exa" : " E");
                  SPRINTF(pb, ffm, res);
               }
               else
               {
                  db *= 1024.0;  // x7
                  res = val / db;
                  form = (type ? " Zetta" : " Z");
                  SPRINTF(pb, ffm, res);
               }
            }
         }
      }
   }
   if( dotrim > 0 )
      trim_float_buf(pb);

   STRCAT(pb, form);

   return pb;
}


///////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
// from : http://forums.codeguru.com/showthread.php?194400-Distance-between-point-and-line-segment
//
// find the distance from the point (cx,cy) to the line
// determined by the points (ax,ay) and (bx,by)
//
// distanceSegment = distance from the point to the line segment
// distanceLine = distance from the point to the line (assuming
//					infinite extent in both directions
//
/*

Subject 1.02: How do I find the distance from a point to a line?


    Let the point be C (Cx,Cy) and the line be AB (Ax,Ay) to (Bx,By).
    Let P be the point of perpendicular projection of C on AB.  The parameter
    r, which indicates P's position along AB, is computed by the dot product 
    of AC and AB divided by the square of the length of AB:
    
    (1)     AC dot AB
        r = ---------  
            ||AB||^2
    
    r has the following meaning:
    
        r=0      P = A
        r=1      P = B
        r<0      P is on the backward extension of AB
        r>1      P is on the forward extension of AB
        0<r<1    P is interior to AB
    
    The length of a line segment in d dimensions, AB is computed by:
    
        L = sqrt( (Bx-Ax)^2 + (By-Ay)^2 + ... + (Bd-Ad)^2)

    so in 2D:   
    
        L = sqrt( (Bx-Ax)^2 + (By-Ay)^2 )
    
    and the dot product of two vectors in d dimensions, U dot V is computed:
    
        D = (Ux * Vx) + (Uy * Vy) + ... + (Ud * Vd)
    
    so in 2D:   
    
        D = (Ux * Vx) + (Uy * Vy) 
    
    So (1) expands to:
    
            (Cx-Ax)(Bx-Ax) + (Cy-Ay)(By-Ay)
        r = -------------------------------
                          L^2

    The point P can then be found:

        Px = Ax + r(Bx-Ax)
        Py = Ay + r(By-Ay)

    And the distance from A to P = r*L.

    Use another parameter s to indicate the location along PC, with the 
    following meaning:
           s<0      C is left of AB
           s>0      C is right of AB
           s=0      C is on AB

    Compute s as follows:

            (Ay-Cy)(Bx-Ax)-(Ax-Cx)(By-Ay)
        s = -----------------------------
                        L^2


    Then the distance from C to P = |s|*L.

*/

double DistanceFromLine(double cx, double cy,
        double ax, double ay, double bx, double by )
{
	double r_numerator = (cx-ax)*(bx-ax) + (cy-ay)*(by-ay);
	double r_denomenator = (bx-ax)*(bx-ax) + (by-ay)*(by-ay);
	double r = r_numerator / r_denomenator;
//
    double px = ax + r*(bx-ax);
    double py = ay + r*(by-ay);
//     
    double s =  ((ay-cy)*(bx-ax)-(ax-cx)*(by-ay) ) / r_denomenator;

	double distanceLine = fabs(s)*sqrt(r_denomenator);
    double distanceSegment;
//
// (xx,yy) is the point on the lineSegment closest to (cx,cy)
//
	double xx = px;
	double yy = py;
	if ( (r >= 0) && (r <= 1) ) {
		return distanceLine;
	} else {
    	double dist1 = (cx-ax)*(cx-ax) + (cy-ay)*(cy-ay);
		double dist2 = (cx-bx)*(cx-bx) + (cy-by)*(cy-by);
		if (dist1 < dist2) {
			xx = ax;
			yy = ay;
			distanceSegment = sqrt(dist1);
		} else {
			xx = bx;
			yy = by;
			distanceSegment = sqrt(dist2);
		}
	}
	return distanceSegment;
}
//////////////////////////////////////////////////////////////////////////
// from : http://stackoverflow.com/questions/819487/efficiently-convert-between-hex-binary-and-decimal-in-c-c

unsigned long PowerOfTwoFromString(char *input, int shift)
{
    unsigned long val = 0;
    char upperLimit = 'a' + (1 << shift);
    while (*input) {
        char c = tolower(*input++);
        unsigned long digit = (c > 'a' && c < upperLimit) ? c - 'a' + 10 : c - '0';
        val = (val << shift) | digit;
    }
    return val;
}

#ifndef ISHEX
#define ISHEX(a) ISDIGIT(a) || (( a >= 'A' ) && ( a <= 'F' )) || (( a >= 'a' ) && ( a <= 'f' ))
#endif

bool is_hex_value( char *input )
{

    if (input && (strlen(input) >= 3)) {
        if ((input[0] == '0') && (tolower(input[1]) == 'x')) {
            size_t ii, len = strlen(input);
            for (ii = 2; ii < len; ii++) {
                if (!ISHEX(input[ii]))
                    return false;
            }
            return true;
        }
    }
    return false;
}

// eof
