/*\
 * slippystuff.cxx
 *
 *
    from : http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames
    the /zoom/x/y.png 
    X goes from 0 (left edge -180) to 2**zoom (right edge 180)
    Y goes from 0 (top edge 85.0511) to 2**zoom (bottom edge -85.0511) in mercator projection
    /////////////////////////////////////////////////////////////////////////////
    // slippy map value conversions
    // from : http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames#C.2FC.2B.2B
    ////////////////////////////////////////////////////////////////////////////
    extern int lon2tilex(double lon, int z);
    extern int lat2tiley(double lat, int z);
    extern double tilex2lon(int x, int z);
    extern double tiley2lat(int y, int z);
    zoom        tile count 
    0	 	             1         - 1 tile covers whole world
    1	 	             4         - 2 × 2 tiles
    2	 	            16         - 4 × 4 tiles
    n	 	          2^2n         - 2^n × 2^n tiles
    12	 	    16,777,216         - 4096 x 4096 tiles
    16		 4,294,967,296         - 2^32
    17		17,179,869,184
    18		68,719,476,736
    19	   274,877,906,944         - Maximum zoom for Mapnik layer
    ==================================================================
    LAT =   85.0511 -  -85.0511
    LON = -180      -  180
    To repesent that another way
    zoom ----- from -----------------    ------- to ---------------  tile count        tile span
    Z= 0 x,y 0,0 lon,lat -180,85.0511 --      1,     1 180,-85.0511            1 360.000000,170.102200
    Z= 1 x,y 0,0 lon,lat -180,85.0511 --      2,     2 180,-85.0511            4 180.000000, 85.051100
    Z= 2 x,y 0,0 lon,lat -180,85.0511 --      4,     4 180,-85.0511           16  90.000000, 42.525550
    Z= 3 x,y 0,0 lon,lat -180,85.0511 --      8,     8 180,-85.0511           64  45.000000, 21.262775
    Z= 4 x,y 0,0 lon,lat -180,85.0511 --     16,    16 180,-85.0511          256  22.500000, 10.631388
    Z= 5 x,y 0,0 lon,lat -180,85.0511 --     32,    32 180,-85.0511         1024  11.250000,  5.315694
    Z= 6 x,y 0,0 lon,lat -180,85.0511 --     64,    64 180,-85.0511         4096   5.625000,  2.657847
    Z= 7 x,y 0,0 lon,lat -180,85.0511 --    128,   128 180,-85.0511        16384   2.812500,  1.328923
    Z= 8 x,y 0,0 lon,lat -180,85.0511 --    256,   256 180,-85.0511        65536   1.406250,  0.664462
    Z= 9 x,y 0,0 lon,lat -180,85.0511 --    512,   512 180,-85.0511       262144   0.703125,  0.332231
    Z=10 x,y 0,0 lon,lat -180,85.0511 --   1024,  1024 180,-85.0511      1048576   0.351563,  0.166115
    Z=11 x,y 0,0 lon,lat -180,85.0511 --   2048,  2048 180,-85.0511      4194304   0.175781,  0.083058
    Z=12 x,y 0,0 lon,lat -180,85.0511 --   4096,  4096 180,-85.0511     16777216   0.087891,  0.041529
    Z=13 x,y 0,0 lon,lat -180,85.0511 --   8192,  8192 180,-85.0511     67108864   0.043945,  0.020764
    Z=14 x,y 0,0 lon,lat -180,85.0511 --  16384, 16384 180,-85.0511    268435456   0.021973,  0.010382
    Z=15 x,y 0,0 lon,lat -180,85.0511 --  32768, 32768 180,-85.0511   1073741824   0.010986,  0.005191
    Z=16 x,y 0,0 lon,lat -180,85.0511 --  65536, 65536 180,-85.0511   4294967296   0.005493,  0.002596
    Z=17 x,y 0,0 lon,lat -180,85.0511 -- 131072,131072 180,-85.0511  17179869184   0.002747,  0.001298
    Z=18 x,y 0,0 lon,lat -180,85.0511 -- 262144,262144 180,-85.0511  68719476736   0.001373,  0.000649
    Z=19 x,y 0,0 lon,lat -180,85.0511 -- 524288,524288 180,-85.0511 274877906944   0.000687,  0.000324
    /////////////////////////////////////////////////////////////////////////////

 *******************************************************************************************************
 from : http://wiki.openstreetmap.org/wiki/Tiles
 Slippy map tilenames calculate tilenames if you have coordinate
    the /zoom/x/y.png 
    X goes from 0 (left edge -180) to 2**zoom (right edge 180)
    Y goes from 0 (top edge 85.0511) to 2**zoom (bottom edge -85.0511) in mercator projection
 So that seems to say the 'tile' is named by the TOP LEFT corner
 http://svn.openstreetmap.org/

 Example : London tile - zoom 7
 http://tile.openstreetmap.org/7/63/42.png
 It stretches from Coventry in the North, to Portsmouth in the South
 ?hereford in the West to London in the East
 Approx TL 52.351,-3.524 BL 50.74,-3.374 TR 52.392,0.119 BR 50.752,0.0875
 Center approx - 51.562,-1.747 - about Oxford
 From my slippydirs.pl 51.562 -1.747 7 - Oxford (appr)

    For lat,lon,zoom 51.562,-1.747,7 = 7/63/42.png is y,x 42,63

    TL 52.4827802221,-2.8125000000 y+0,x+0 63,42
    BL 50.7364551370,-2.8125000000 y+1,x+0 63,43
    TR 52.4827802221,0.0000000000 y+0,x+1 64,42
    BR 50.7364551370,0.0000000000 y+1,x+1 64,43

    SPAN x = 2.8125 - y = 1.74632508506756 degrees

    bbox(southLatitude,westLongitude,northLatitude,eastLongitude)
    BBOX=50.7364551370107,-2.8125,52.4827802220782,0 (BLTR)

    osm bbox=westLongitude,southLatitude,eastLongitude,northLatitude)
    bbox=-2.8125,50.7364551370107,0,52.4827802220782 (LBRT)


\*/


#include <stdio.h>
#include <math.h>
#ifndef _MSC_VER
#include <stdlib.h> // for atoi(), ...
#include <string.h> // for strlen(), ...
#endif
#include "sprtf.hxx"
#include "utils.hxx"
#include "slippystuff.hxx"

static const char *module = "slippystuff";

std::string last_slippy_error;

bool in_slippy_range(double lat, double lon)
{
    if ((lat > MAX_SLIPPY_LAT) ||
        (lat < MIN_SLIPPY_LAT) ||
        (lon > MAX_SLIPPY_LON) ||
        (lon < MIN_SLIPPY_LON)) {
        return false;
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////
// slippy map value
// from : http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames#C.2FC.2B.2B
//
////////////////////////////////////////////////////////////////////////////
int lon2tilex(double lon, int z) 
{ 
	return (int)(floor((lon + 180.0) / 360.0 * pow(2.0, z))); 
}
 
int lat2tiley(double lat, int z)
{ 
	return (int)(floor((1.0 - log( tan(lat * M_PI/180.0) + 1.0 / cos(lat * M_PI/180.0)) / M_PI) / 2.0 * pow(2.0, z))); 
}
 
double tilex2lon(int x, int z) 
{
	return x / pow(2.0, z) * 360.0 - 180;
}
 
double tiley2lat(int y, int z) 
{
	double n = M_PI - 2.0 * M_PI * y / pow(2.0, z);
	return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}


bool get_slippy_path( double lat, double lon, int zoom, char **pcp )
{
    char *cp = GetNxtBuf();
    if ( !pcp || !in_slippy_range(lat,lon) || !SLIPPY_ZOOM(zoom) ) {
        sprintf(cp, "%s: PARAMETER ERROR: ", module);
        if (!pcp)
            strcat(cp, "<null> char pointer ");
        if (!in_slippy_range(lat,lon))
            sprintf(EndBuf(cp),"lat %s, and/or lon %s OUT of slippy range ", get_trim_double(lat), get_trim_double(lon));
        if ( !SLIPPY_ZOOM(zoom) )
            strcat(cp, "zoom not 0 <= z < 20");
        last_slippy_error = cp;
        return false;
    }
    sprintf(cp,"%d",zoom);
    strcat(cp,PATH_SEP);
    int i = lon2tilex(lon,zoom); // get tilex
    sprintf(EndBuf(cp),"%d",i);
    strcat(cp,PATH_SEP);
    i = lat2tiley(lat,zoom);    // get tiley
    sprintf(EndBuf(cp),"%d",i);
    strcat(cp,".png");
    *pcp = cp;
    return true;
}

/* ==================================================

sub LonLat_to_bbox {
	my ($lat, $lon, $zoom) = @_;
 
	my $width = 425; my $height = 350;	# note: must modify this to match your embed map width/height in pixels
	my $tile_size = 256;
 
	my ($xtile, $ytile) = getTileNumber ($lat, $lon, $zoom);
 
	my $xtile_s = ($xtile * $tile_size - $width/2) / $tile_size;
	my $ytile_s = ($ytile * $tile_size - $height/2) / $tile_size;
	my $xtile_e = ($xtile * $tile_size + $width/2) / $tile_size;
	my $ytile_e = ($ytile * $tile_size + $height/2) / $tile_size;
 
	my ($bb_min_lon, $bb_max_lat) = getLonLat($xtile_s, $ytile_s, $zoom);
	my ($bb_max_lon, $bb_min_lat) = getLonLat($xtile_e, $ytile_e, $zoom);
 
	my $bbox = "$bb_min_lon,$bb_max_lat,$bb_max_lon,$bb_min_lat";
	return $bbox;
}

   ====================================================== */


//# convert from permalink OSM format like:
//# http://www.openstreetmap.org/?lat=43.731049999999996&lon=15.79375&zoom=13&layers=M
//# to OSM "Export" iframe embedded bbox format like:   minlat  minlon  maxlat maxlon - ie BL to TR
//# http://www.openstreetmap.org/export/embed.html?bbox=15.7444,43.708,15.8431,43.7541&layer=mapnik
// ============================================================================================
// TODO: SEEMS THIS IS STILL FAULTY, SO DEPRECIATE - eventually REMOVE or FIX
bool get_bounding_box_depreciated( double lat, double lon, int zoom, PBNDBOX pbb, bool v )
{
    if (!pbb) {
        if (v) SPRTF("%s: Null BBOX pointer!\n", module);
        return false;
    }
    if (!in_slippy_range(lat,lon)) {
        if (v) SPRTF("%s: lat %s, and/or lon %s NOT in slippy range!\n", module,
            get_trim_double(lat),
            get_trim_double(lon) );
        return false;
    }
    int xtile = lon2tilex(lon,zoom);
    int ytile = lat2tiley(lat,zoom);
    int maxx = lon2tilex(MAX_SLIPPY_LON,zoom);
    //int maxy = lon2tilex(MAX_SLIPPY_LAT,zoom);
    int maxy = lon2tilex(MIN_SLIPPY_LAT,zoom); // TODO: CHECK ME! Is this correct???

    // # what is the operator pecedence here??????????
    //pbb->xtile_s = (int)(((double)xtile * 256.0 - 128.0) / 256.0);
    //pbb->ytile_s = (int)(((double)ytile * 256.0 - 128.0) / 256.0);
    //pbb->xtile_e = (int)(((double)xtile * 256.0 + 128.0) / 256.0);
    //pbb->ytile_e = (int)(((double)ytile * 256.0 + 128.0) / 256.0);

    // get min x,y ie -128, but care to NOT fall below ZERO
    if (xtile < 128)
        pbb->xtile_s = 0;
    else
        pbb->xtile_s = xtile - 128;

    if (ytile < 128)
        pbb->ytile_s = 0;
    else
        pbb->ytile_s = ytile - 128;

    // get max x,y ie min + 256; but care to NOT exceed MAX for this zoom

    pbb->xtile_e = pbb->xtile_s + 256;
    if (pbb->xtile_e > maxx) {
        pbb->xtile_e = maxx;
        pbb->xtile_s = maxx - 256;
        if (pbb->xtile_s < 0)
            pbb->xtile_s = 0;
    }

    pbb->ytile_e = pbb->ytile_s + 256;
    if (pbb->ytile_e > maxy) {
        pbb->ytile_e = maxy;
        pbb->ytile_s = maxy - 256;
        if (pbb->ytile_s < 0)
            pbb->ytile_s = 0;
    }

    // convert that spread back to lat,lon bounding box
    pbb->bb_min_lon = tilex2lon(pbb->xtile_s,zoom);
    pbb->bb_max_lat = tiley2lat(pbb->ytile_s,zoom);
    pbb->bb_max_lon = tilex2lon(pbb->xtile_e,zoom);
    pbb->bb_min_lat = tiley2lat(pbb->ytile_e,zoom);

    // keep the values used to generate box
    pbb->llz.zoom = zoom;
    pbb->llz.lat = lat;
    pbb->llz.lon = lon;

    if (v) SPRTF("%s: z=%d x,y s %d,%d e %d,%d s\n", module,
        zoom,
        pbb->xtile_s, pbb->ytile_s,
        pbb->xtile_e, pbb->ytile_e,
        get_bounding_box_stg(pbb,true) );

    return true;
}

// # to OSM "Export" iframe embedded bbox format like:   minlat  minlon  maxlat maxlon - ie BL to TR
char *get_bounding_box_stg( PBNDBOX pbb, bool add_span )
{
    char *cp = GetNxtBuf();
    sprintf(cp,"BBOX lat,lon min %s,%s max %s,%s",
        get_trim_double(pbb->bb_min_lat),
        get_trim_double(pbb->bb_min_lon),
        get_trim_double(pbb->bb_max_lat),
        get_trim_double(pbb->bb_max_lon) );
    if (add_span) {
        sprintf(EndBuf(cp), " span %s,%s",
            get_trim_double(pbb->bb_max_lat - pbb->bb_min_lat),
            get_trim_double(pbb->bb_max_lon - pbb->bb_min_lon));
    }
    return cp;
}

bool lat_lon_in_bounding_box( double lat, double lon, PBNDBOX pbb )
{
    if ((lat >= pbb->bb_min_lat) && // GTE min lat
        (lat <= pbb->bb_max_lat) && // LTE max lat
        (lon >= pbb->bb_min_lon) && // GTE min lon
        (lon <= pbb->bb_max_lon)) // LTE max lon
        return true;
    return false;
}

// assumes bounding box structure has be 'seeded' with a location
bool expand_bounding_box( double lat, double lon, PBNDBOX pbb, bool v )
{
    // check valid parameters
    if (!pbb) {
        if (v) SPRTF("%s: Null BBOX pointer!\n", module);
        return false;
    }
    if (!in_slippy_range(lat,lon)) {
        if (v) SPRTF("%s: Given lat %s, and/or lon %s NOT in slippy range!\n", module,
            get_trim_double(lat),
            get_trim_double(lon) );
        return false;
    }
    // including current values in BBOX
    if (!in_slippy_range(pbb->bb_max_lat,pbb->bb_max_lon)) {
        if (v) SPRTF("%s: BBOX max. lat %s, and/or lon %s NOT in slippy range!\n", module,
            get_trim_double(pbb->bb_max_lat),
            get_trim_double(pbb->bb_max_lon) );
        return false;
    }
    if (!in_slippy_range(pbb->bb_min_lat,pbb->bb_min_lon)) {
        if (v) SPRTF("%s: BBOX min. lat %s, and/or lon %s NOT in slippy range!\n", module,
            get_trim_double(pbb->bb_min_lat),
            get_trim_double(pbb->bb_min_lon) );
        return false;
    }

    // all ok to party
    if (lat < pbb->bb_min_lat) {
        if (v) SPRTF("%s: Reduced min. lat from %s to %s\n", module,
            get_trim_double( pbb->bb_min_lat ),
            get_trim_double( lat ));
        pbb->bb_min_lat = lat;
    }
    if (lat > pbb->bb_max_lat) {
        if (v) SPRTF("%s: Increased max. lat from %s to %s\n", module,
            get_trim_double( pbb->bb_max_lat ),
            get_trim_double( lat ));
        pbb->bb_max_lat = lat;
    }
    if (lon < pbb->bb_min_lon) {
        if (v) SPRTF("%s: Reduced min. lon from %s to %s\n", module,
            get_trim_double( pbb->bb_min_lon ),
            get_trim_double( lon ));
        pbb->bb_min_lon = lon;
    }

    if (lon > pbb->bb_max_lon) {
        if (v) SPRTF("%s: Increased max. lon from %s to %s\n", module,
            get_trim_double( pbb->bb_max_lon ),
            get_trim_double( lon ));
        pbb->bb_max_lon = lon;
    }
    return true;

}

bool path_to_llz( char *path, PLLZ pllz )
{
    int c = 0;
    char *cp = GetNxtBuf();
    strcpy(cp,path);
    size_t ii, len = strlen(cp);
    if (!len) {
        return false;
    }
    // 1: back up to 'dot'
    for (ii = (len - 1); ii > 0; ii--) {
        c = cp[ii];
        cp[ii] = 0;
        if (c == '.') {
            ii--;
            break;
        }
    }
    // no 'dot', or no digit now, out of here
    if ((c != '.') || !ISDIGIT(cp[ii]))
        return false;

    // get the tiley value
    for (; ii > 0; ii--) {
        c = cp[ii];
        if ((c == '\\') || (c == '/')) {
            ii++;   // back to last digit encountered
            break;
        } else if (!ISDIGIT(c)) {
            return false;
        }
    }
    pllz->tiley = atoi(&cp[ii]);
    ii--;
    cp[ii] = 0;
    if (ii < 2)
        return false;
    ii--;
    if (ii < 2)
        return false;
    if (!ISDIGIT(cp[ii]))
        return false;

    // get the tilex value
    for (; ii > 0; ii--) {
        c = cp[ii];
        if ((c == '\\') || (c == '/')) {
            ii++;   // back to last digit encountered
            break;
        } else if (!ISDIGIT(c)) {
            return false;
        }
    }
    pllz->tilex = atoi(&cp[ii]);
    ii--;
    cp[ii] = 0;
    if (!ii)
        return false;
    ii--;
    if (!ii)
        return false;
    if (!ISDIGIT(cp[ii]))
        return false;

    // get the zoom value
    int i = ii; // switch to signed for >= 0
    for (; i >= 0; i--) {
        c = cp[i];
        if ((c == '\\') || (c == '/')) {
            ii++;   // back to last digit encountered
            break;
        } else if (!ISDIGIT(c)) {
            return false;
        }
        if (ii > 0)
            ii--;
    }
    pllz->zoom = atoi(&cp[ii]);
    if (!SLIPPY_ZOOM(pllz->zoom))
        return false;

    pllz->lon = tilex2lon( pllz->tilex, pllz->zoom );
    pllz->lat = tiley2lat( pllz->tiley, pllz->zoom );

    if (!in_slippy_range(pllz->lat,pllz->lon))
        return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////
#ifdef ADD_SECOND_BBOX
void tile2boundingBox(int x, int y, int zoom, PBoundingBox pbb)
{
    pbb->north = tiley2lat(y, zoom);
    pbb->south = tiley2lat(y + 1, zoom);
    pbb->west = tilex2lon(x, zoom);
    pbb->east = tilex2lon(x + 1, zoom);
}

bool llz2bb( double lat, double lon, int zoom, PBoundingBox pbb )
{
    if (!in_slippy_range(lat,lon)) {
        //if (v) SPRTF("%s: lat %s, and/or lon %s NOT in slippy range!\n", module,
        //    get_trim_double(lat),
        //    get_trim_double(lon) );
        return false;
    }
    int tilex = lon2tilex(lon,zoom);
    int tiley = lat2tiley(lat,zoom);
    tile2boundingBox(tilex,tiley,zoom,pbb);
    return true;
}

char *get_BoundingBox_stg( PBoundingBox pbb, bool add_range )
{
    char *cp = GetNxtBuf();
    sprintf(cp,"BB: N %s, S %s, W %s, E %s",
        get_trim_double(pbb->north),
        get_trim_double(pbb->south),
        get_trim_double(pbb->west),
        get_trim_double(pbb->east));
    if (add_range) {
        sprintf(EndBuf(cp),", RNG lat %s, lon %s",
            get_trim_double(pbb->north - pbb->south),
            get_trim_double(pbb->east - pbb->west));
    }
    return cp;
}
#endif // #ifdef ADD_SECOND_BBOX
//////////////////////////////////////////////////////////////////////

// return the bounding box of the slippy map tile corresponding to this lat,lon,zoom
bool get_map_bounding_box( double lat, double lon, int zoom, PBNDBOX pbb, bool v )
{
    if (!in_slippy_range(lat,lon)) {
        if (v) SPRTF("%s: lat %s, and/or lon %s NOT in slippy range!\n", module,
            get_trim_double(lat),
            get_trim_double(lon) );
        return false;
    }
    if (!pbb)
        return false;

    // of course min is 0,0, and if max x,y needed, then
    //my ($maxx,$maxy) = getTileNumber($MIN_SLIPPY_LAT,$MAX_SLIPPY_LON,$z);
    //my ($minx,$miny) = getTileNumber($MAX_SLIPPY_LAT,$MIN_SLIPPY_LON,$z);
    //int maxx = lon2tilex(MAX_SLIPPY_LON,zoom);
    //int maxy = lat2tiley(MIN_SLIPPY_LAT,zoom);

    int xtile = lon2tilex(lon,zoom);
    int ytile = lat2tiley(lat,zoom);
/*
    TL 52.4827802221,-2.8125000000 y+0,x+0 63,42
    BL 50.7364551370,-2.8125000000 y+1,x+0 63,43
    TR 52.4827802221,0.0000000000 y+0,x+1 64,42
    BR 50.7364551370,0.0000000000 y+1,x+1 64,43
 */
    //double tl_lat = tiley2lat(ytile+0,zoom);
    double bl_lat = tiley2lat(ytile+1,zoom);
    double tr_lat = tiley2lat(ytile+0,zoom);
    //double br_lat = tiley2lat(ytile+1,zoom);

    //double tl_lon = tilex2lon(xtile+0,zoom);
    double bl_lon = tilex2lon(xtile+0,zoom);
    double tr_lon = tilex2lon(xtile+1,zoom);
    //double br_lon = tilex2lon(xtile+1,zoom);
/* 
    sprintf(cp,"BBOX lat,lon min %s,%s max %s,%s",
        get_trim_double(pbb->bb_min_lat),
        get_trim_double(pbb->bb_min_lon),
        get_trim_double(pbb->bb_max_lat),
        get_trim_double(pbb->bb_max_lon) );
    if (add_span) {
        sprintf(EndBuf(cp), " span %s,%s",
            get_trim_double(pbb->bb_max_lat - pbb->bb_min_lat),
            get_trim_double(pbb->bb_max_lon - pbb->bb_min_lon));
    }
 */
    pbb->bb_min_lat = bl_lat;
    pbb->bb_min_lon = bl_lon;
    pbb->bb_max_lat = tr_lat;
    pbb->bb_max_lon = tr_lon;

    pbb->xtile_e = xtile;
    pbb->xtile_s = xtile + 1;
    pbb->ytile_e = ytile;
    pbb->ytile_s = ytile + 1;

    // keep the values used to generate box
    pbb->llz.zoom = zoom;
    pbb->llz.lat = lat;
    pbb->llz.lon = lon;
    if (v) SPRTF("%s: %s\n", module, get_bounding_box_stg(pbb,true));

    return true;
}

bool in_slippy_zoom_range(int zoom)
{
    if ((zoom < MIN_SLIPPY_ZOOM) || (zoom > MAX_SLIPPY_ZOOM))
        return false;
    return true;
}

int get_max_tilex_for_zoom( int zoom )
{
    double lon = MAX_SLIPPY_LON - MY_EPSILON;
    int max_x = lon2tilex(lon,zoom); // get tilex
    return max_x;
}
int get_max_tiley_for_zoom( int zoom )
{
    double lat = MIN_SLIPPY_LAT;
    int max_y = lat2tiley(lat,zoom); // get tiley
    return max_y;
}

bool xy_in_range_for_zoom( int x, int y, int zoom, bool verb )
{
    if (!in_slippy_zoom_range(zoom)) {
        if (verb) SPRTF("%s: Z=%d: Out of slippy zoom range %d to %d\n", module,
            zoom, MIN_SLIPPY_ZOOM, MAX_SLIPPY_ZOOM );
        return false;
    }
    //double lat = MIN_SLIPPY_LAT;
    //double lon = MAX_SLIPPY_LON - MY_EPSILON;
    int max_x = get_max_tilex_for_zoom(zoom); // get tilex
    int max_y = get_max_tiley_for_zoom(zoom); // get tiley
    if ((x < 0) || (x > max_x) || (y < 0) || (y > max_y)) {
        if (verb) {
            SPRTF("%s: Z=%d ",module, zoom);
            if (x < 0)
                SPRTF("x LSS 0! ");
            else if (x > max_x)
                SPRTF("x=%d GTT maxx=%d! ",x,max_x);
            if (y < 0)
                SPRTF("y LSS 0! ");
            else if (y > max_y) 
                SPRTF("y=%d GTT maxy=%d! ",y,max_y);
            SPRTF("\n");
        }
        return false;
    }
    if (verb) {
        SPRTF("%s: Z=%d x=%d in range 0 to %d and y=%d in range 0 to %d\n", module,
            zoom, x, max_x, y, max_y );
    }
    return true;
}

char *get_slippy_range_stg( int zoom )
{
    char *cp = GetNxtBuf();
    if (!in_slippy_zoom_range(zoom)) {
        sprintf(cp, "Z=%d: Out of slippy zoom range %d to %d", zoom, MIN_SLIPPY_ZOOM, MAX_SLIPPY_ZOOM );
        return cp;
    }
    int max_x = get_max_tilex_for_zoom(zoom); // get tilex
    int max_y = get_max_tiley_for_zoom(zoom); // get tiley
    sprintf(cp,"Z=%d x range 0 to %d and y range 0 to %d", zoom, max_x, max_y );
    return cp;
}


bool get_next_map_bbox( PBNDBOX pbb, bb_dir dir, bool v )
{
    if ((dir <= nbb_none)||
        (dir >= nbb_max)) {
        if (v) SPRTF("%s: next bbox direction %d out of range %d to %d\n", module,
            dir,
            nbb_right,
            nbb_tr );
        return false;
    }
    if (!pbb)
        return false;
    int zoom = pbb->llz.zoom;
    if (!in_slippy_zoom_range(zoom)) {
        if (v) SPRTF("%s: next bbox zoom %d out of range %d to %d\n", module,
            zoom,
            MIN_SLIPPY_ZOOM,
            MAX_SLIPPY_ZOOM );
        return false;

    }

    double min_lat = pbb->bb_min_lat;
    double min_lon = pbb->bb_min_lon;
    double max_lat = pbb->bb_max_lat;
    if (!in_slippy_range(min_lat,min_lon)) {
        if (v) SPRTF("%s: lat %s, and/or lon %s NOT in slippy range!\n", module,
            get_trim_double(min_lat),
            get_trim_double(min_lon) );
        return false;
    }

    int maxx = get_max_tilex_for_zoom(zoom); // = lon2tilex(MAX_SLIPPY_LON,zoom);
    int maxy = get_max_tiley_for_zoom(zoom); // = lat2tiley(MIN_SLIPPY_LAT,zoom);

    int xtile = lon2tilex(min_lon,zoom);
    int ytile = lat2tiley(max_lat,zoom);
    switch (dir) {
    case nbb_right:
        // lon INCREASES
        xtile++;    // CHECKED: Z=10 -122.695313, -122.34375, -121.992188, etc
        break;
    case nbb_lr:
        // diagonal - lower right - ie lat DECREASE, lon INCREASE
        xtile++;    // CHECKED: Z=10
        ytile++;    // 
        break;
    case nbb_bottom:
        // down - lat DECREASES
        ytile++;    // lats 37.439974 37.160317 36.879621 36.597889 36.315125
        break;
    case nbb_ll:
        // diagonal - lower left - ie lat DECREASES, lon DECREASES
        xtile--;
        ytile++;
        break;
    case nbb_left:
        xtile--;    // CHECKED: Z=10 lons -122.695313 -123.046875 -123.398438 -123.75 -124.101563 -124.453125 -124.804688 
        break;
    case nbb_tl:
        xtile--;
        ytile--;
        break;
    case nbb_top:
        // up - lat INCREASES
        ytile--;    // CHECKED: Z=10 
        break;
    case nbb_tr:
        ytile--;
        xtile++;
        break;
    default:
        return false;
    }

    if (xtile < 0) {
        if (v) SPRTF("%s: xtile went negative!\n", module);
        return false;
    }
    if (xtile > maxx) {
        if (v) SPRTF("%s: xtile %d greater than max %d!\n", module, xtile, maxx);
        return false;
    }
    if (ytile < 0) {
        if (v) SPRTF("%s: ytile went negative!\n", module);
        return false;
    }
    if (ytile > maxy) {
        if (v) SPRTF("%s: ytile %d greater than max %d!\n", module, ytile, maxy);
        return false;
    }

    double lat = tiley2lat(ytile,zoom);
    double lon = tilex2lon(xtile,zoom);
    return get_map_bounding_box( lat, lon, zoom, pbb, v );

}

// assumes bounding box structure has be 'seeded' with a location
bool expand_bounding_box_by_ll( double lat, double lon, PBNDBOX pbb, bool v )
{
    // check valid parameters
    // check given
    if (!in_slippy_range(lat,lon)) {
        if (v) SPRTF("%s: Given lat %s, and/or lon %s NOT in slippy range!\n", module,
            get_trim_double(lat),
            get_trim_double(lon) );
        return false;
    }
    if (!pbb) {
        if (v) SPRTF("%s: Null BBOX pointer!\n", module);
        return false;
    }
    // check the current destination is valid
    if (!in_slippy_range(pbb->bb_max_lat,pbb->bb_max_lon)) {
        if (v) SPRTF("%s: DST MAX lat %s, and/or lon %s NOT in slippy range!\n", module,
            get_trim_double(pbb->bb_max_lat),
            get_trim_double(pbb->bb_max_lon) );
        return false;
    }
    if (!in_slippy_range(pbb->bb_min_lat,pbb->bb_min_lon)) {
        if (v) SPRTF("%s: DST MIN lat %s, and/or lon %s NOT in slippy range!\n", module,
            get_trim_double(pbb->bb_min_lat),
            get_trim_double(pbb->bb_min_lon) );
        return false;
    }
    // ok to party
    if (lat > pbb->bb_max_lat) {
        if (v) SPRTF("%s: Increased max lat from %s to %s\n", module,
            get_trim_double(pbb->bb_max_lat),
            get_trim_double(lat) );
        pbb->bb_max_lat = lat;
    }
    if (lat < pbb->bb_min_lat) {
        if (v) SPRTF("%s: Decreased min lat from %s to %s\n", module,
            get_trim_double(pbb->bb_min_lat),
            get_trim_double(lat) );
        pbb->bb_min_lat = lat;
    }
    if (lon > pbb->bb_max_lon) {
        if (v) SPRTF("%s: Increased max lon from %s to %s\n", module,
            get_trim_double(pbb->bb_max_lon),
            get_trim_double(lon) );
        pbb->bb_max_lon = lon;
    }
    if (lon < pbb->bb_min_lon) {
        if (v) SPRTF("%s: Decreased min lon from %s to %s\n", module,
            get_trim_double(pbb->bb_min_lon),
            get_trim_double(lon) );
        pbb->bb_min_lon = lon;
    }
    return true;
}

// assumes bounding box structure has be 'seeded' with a location
bool expand_bounding_box_by_box( PBNDBOX psrc, PBNDBOX pdst, bool v )
{
    // check valid parameters
    if (!psrc) {
        if (v) SPRTF("%s: Null source BBOX pointer!\n", module);
        return false;
    }
    if (!pdst) {
        if (v) SPRTF("%s: Null destination BBOX pointer!\n", module);
        return false;
    }

    if (!in_slippy_range(psrc->bb_max_lat,psrc->bb_max_lon)) {
        if (v) SPRTF("%s: SRC MAX lat %s, and/or lon %s NOT in slippy range!\n", module,
            get_trim_double(psrc->bb_max_lat),
            get_trim_double(psrc->bb_max_lon) );
        return false;
    }
    if (!in_slippy_range(psrc->bb_min_lat,psrc->bb_min_lon)) {
        if (v) SPRTF("%s: SRC MIN lat %s, and/or lon %s NOT in slippy range!\n", module,
            get_trim_double(psrc->bb_min_lat),
            get_trim_double(psrc->bb_min_lon) );
        return false;
    }

    // all ok to party
    if (!expand_bounding_box_by_ll( psrc->bb_max_lat, psrc->bb_max_lon, pdst, v ))
        return false;
    if (!expand_bounding_box_by_ll( psrc->bb_min_lat, psrc->bb_min_lon, pdst, v ))
        return false;

    return true;
}

bool seed_bounding_box( double lat, double lon, PBNDBOX pbb, bool v )
{
    // check valid parameters
    // check given
    if (!in_slippy_range(lat,lon)) {
        if (v) SPRTF("%s: Given lat %s, and/or lon %s NOT in slippy range!\n", module,
            get_trim_double(lat),
            get_trim_double(lon) );
        return false;
    }
    if (!pbb) {
        if (v) SPRTF("%s: Null BBOX pointer!\n", module);
        return false;
    }
    // seed bounding box
    pbb->bb_max_lat = lat;
    pbb->bb_max_lon = lon;
    pbb->bb_min_lat = lat;
    pbb->bb_min_lon = lon;
    return true;
}

// eof - slippystuff.cxx
