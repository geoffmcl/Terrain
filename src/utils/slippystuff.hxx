// slippystuff.hxx
#ifndef _SLIPPYSTUFF_HXX_
#define _SLIPPYSTUFF_HXX_

#define MAX_SLIPPY_LAT 85.0511
#define MIN_SLIPPY_LAT -85.0511
#define MAX_SLIPPY_LON 180.0
#define MIN_SLIPPY_LON -180.0

#define MAX_SLIPPY_ZOOM 19
#define MIN_SLIPPY_ZOOM 0

typedef struct tagLLZ {
    double lat,lon;
    int tilex, tiley, zoom;
}LLZ, *PLLZ;

typedef struct tagBNDBOX {
    LLZ llz;
    int xtile_e, ytile_e, xtile_s, ytile_s;
    double bb_min_lon,bb_max_lat,bb_max_lon,bb_min_lat;
}BNDBOX, *PBNDBOX;

// DEPRECIATE this faulty service
extern bool get_bounding_box_depreciated( double lat, double lon, int zoom, PBNDBOX pbb, bool verb = false );
extern char *get_bounding_box_stg( PBNDBOX pbb, bool add_span = false );
extern bool lat_lon_in_bounding_box( double lat, double lon, PBNDBOX pbb );
// return the bounding box of the slippy map tile corresponding to this lat,lon,zoom
extern bool get_map_bounding_box( double lat, double lon, int zoom, PBNDBOX pbb, bool v = false );
// assumes bounding box structure has be 'seeded' with a location
extern bool seed_bounding_box( double lat, double lon, PBNDBOX pbb, bool v = false );
extern bool expand_bounding_box_by_ll( double lat, double lon, PBNDBOX pbb, bool v = false );
extern bool expand_bounding_box_by_box( PBNDBOX psrc, PBNDBOX pdst, bool v = false );

/////////////////////////////////////////////////////////////////////////////
// some z,x,y range checking
extern int get_max_tiley_for_zoom( int zoom );
extern int get_max_tilex_for_zoom( int zoom );
extern bool xy_in_range_for_zoom( int x, int y, int zoom, bool verb = false );
extern bool in_slippy_zoom_range(int zoom);
extern char *get_slippy_range_stg( int zoom );
/////////////////////////////////////////////////////////////////////////////

// slippy map value
// from : http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames#C.2FC.2B.2B
////////////////////////////////////////////////////////////////////////////
extern int lon2tilex(double lon, int z);
extern int lat2tiley(double lat, int z);
extern double tilex2lon(int x, int z);
extern double tiley2lat(int y, int z);
/////////////////////////////////////////////////////////////////////////////

extern std::string last_slippy_error;

#define SLIPPY_ZOOM(z)  ((z >= 0) && (z < 20))

extern bool get_slippy_path( double lat, double lon, int zoom, char **pcp );
extern bool path_to_llz( char *path, PLLZ pllz );

#ifdef ADD_SECOND_BBOX
//////////////////////////////////////////////////////////////////////////
// Another try at getting a bounding box
typedef struct tagBoundingBox {
    double north;
    double south;
    double east;
    double west;   
}BoundingBox, *PBoundingBox;
extern void tile2boundingBox(int x, int y, int zoom, PBoundingBox pbb);
extern bool llz2bb( double lat, double lon, int zoom, PBoundingBox pbb );
extern char *get_BoundingBox_stg( PBoundingBox pbb, bool add_range = false );
//////////////////////////////////////////////////////////////////////////
#endif // ADD_SECOND_BBOX


enum bb_dir {
    nbb_none,
    nbb_right,
    nbb_lr,
    nbb_bottom,
    nbb_ll,
    nbb_left,
    nbb_tl,
    nbb_top,
    nbb_tr,
    nbb_max
};

extern bool get_next_map_bbox( PBNDBOX pbb, bb_dir dir, bool v = false );


#endif // #ifndef _SLIPPYSTUFF_HXX_
// eof - slippystuff.hxx

