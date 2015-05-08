/*\
 * png-dump2.cxx
 *
 * Copyright (c) 2015 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#include <string.h> // for strdup(), ...
#ifdef HAVE_EASYBMP
#include <EasyBMP.h>
#endif
#include "utils/sprtf.hxx"
#include "lodepng/lodepng.h"

static const char *module = "png-dump2";

static const char *usr_input = 0;
#ifdef NDEBUG
static bool add_test = false;
#else
static bool add_test = true;
#endif
static bool show_raw_data = false;

static const char *test_file = "Z:\\dem3\\build\\hmn37w123.png";
static const char *def_log = "temppngd2.txt";

void give_help( char *name )
{
    printf("%s: usage: [options] usr_input\n", module);
    printf("Options:\n");
    printf(" --help  (-h or -?) = This help and exit(2)\n");
    // TODO: More help
}

int parse_args( int argc, char **argv )
{
    int i,i2,c;
    char *arg, *sarg;
    for (i = 1; i < argc; i++) {
        arg = argv[i];
        i2 = i + 1;
        if (*arg == '-') {
            sarg = &arg[1];
            while (*sarg == '-')
                sarg++;
            c = *sarg;
            switch (c) {
            case 'h':
            case '?':
                give_help(argv[0]);
                return 2;
                break;
            // TODO: Other arguments
            default:
                printf("%s: Unknown argument '%s'. Tyr -? for help...\n", module, arg);
                return 1;
            }
        } else {
            // bear argument
            if (usr_input) {
                printf("%s: Already have input '%s'! What is this '%s'?\n", module, usr_input, arg );
                return 1;
            }
            usr_input = strdup(arg);
        }
    }
    if (add_test && !usr_input) {
        usr_input = strdup(test_file);
    }
    if (!usr_input) {
        printf("%s: No user input found in command!\n", module);
        return 1;
    }
    return 0;
}

#ifdef HAVE_EASYBMP
static const char *def_bmp = "tempbmp.bmp";
static bool convert_zero_2_blue = true;
/////////////////////////////////////////////////////////////////////////////
// 
bool write_bmp_file( int width, int height, unsigned char *pixels )
{
    RGBApixel p;
    BMP *pbmp = new BMP;
    pbmp->SetSize(width,height);
    pbmp->SetBitDepth(24);
    int x,y,off;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            // Where to fetch the PIXEL from
            off = ((y * width) + x)  * 4;
            // need to INVERT pixels - NO
            // off = (((height - y - 1) * width) + x) * 4;
            // and seems invert columns - NO
            // off = (((height - y - 1) * width) + (width - 1 - x)) * 4;
            // another try - just invert columns - NO
            // off = ((y * width) + (width - 1 - x))  * 4;
            p.Red   = pixels[off + 0];
            p.Green = pixels[off + 1];
            p.Blue  = pixels[off + 2];
            p.Alpha = pixels[off + 3];
            if (convert_zero_2_blue) {
                if ((p.Red == 0) &&(p.Green == 0)) {
                    if (p.Blue == 0) {
                        p.Blue = 255;
                    } else if (p.Blue == 255) {
                        p.Blue = 128;
                    }
                }
            }
            // BAD! pbmp->SetPixel(y,x,p); // Oops, seem that should be (col,row,pixel)
            pbmp->SetPixel(x,y,p);
        }
    }
    bool bret = pbmp->WriteToFile(def_bmp);
    if (bret) {
        SPRTF("%s: Written BMP file '%s'\n", module, def_bmp);
    } else {
        SPRTF("%s: Write of BMP file '%s' FAILED!\n", module, def_bmp);
    }
    delete pbmp;
    return bret;
}
#endif // HAVE_EASYBMP


static bool add_alpha = false;
int dump_png_input()
{
    int iret = 0;
    unsigned res = 0;
    unsigned char *out = 0;
    unsigned char *pixels = 0;
    size_t outsize;
    unsigned int width, height, wid, hgt;
    const char *file = usr_input;
    //LodePNGColorType colortype;
    LodePNGState state;
    out = 0;
    outsize = 0;
    lodepng_state_init(&state);
    res = lodepng_load_file(&out, &outsize, file);
    if (res) {
        SPRTF("%s: Failed to load '%s'\n%s\n", module, file, lodepng_error_text(res));
        iret = 1;
        goto exit;
    }
    res = lodepng_inspect(&width, &height, &state, out, outsize);
    if (res) {
        SPRTF("%s: Failed to inspect() '%s'\n%s\n", module, file, lodepng_error_text(res));
        iret = 1;
        goto exit;
    }
    //res = lodepng_decode_file( &out, &width, &height, file, LCT_RGBA, 8);
    //unsigned res = lodepng::decode( out, width, height, file );
    //unsigned res = decode(

    SPRTF("%s: loaded '%s' in buffer %d bytes\n", module, file, (int)outsize);
    SPRTF("%s: Width %d, Height %d. State bits %d, color %d\n", module, width, height, state.info_raw.bitdepth, state.info_raw.colortype);
    res = lodepng_decode_memory(&pixels, &wid, &hgt,
                               out, outsize,
                               state.info_raw.colortype, state.info_raw.bitdepth);
    if (res) {
        SPRTF("%s: Failed to decode() '%s'\n%s\n", module, file, lodepng_error_text(res));
        iret = 1;
        goto exit;
    }
    if ((width != wid) || (hgt != height)) {
        SPRTF("%s: decode() return different width %d/%d, or height %d,%d\n", module, width, wid, height, hgt);
        iret = 1;
        goto exit;

    }
    SPRTF("%s: Processing Width %d, Height %d pixels...\n", module, wid, hgt);
    if (show_raw_data) {
        int off;
        unsigned char r,g,b,a;
        int val = add_std_out(0);
        for (hgt = 0; hgt < height; hgt++) {
            SPRTF("Row %4d: ", (hgt + 1));
            for (wid = 0; wid < width; wid += 4) {
                off = (hgt * width * 4) + (wid * 4);
                r = pixels[off + 0];
                g = pixels[off + 1];
                b = pixels[off + 2];
                a = pixels[off + 3];
                if (add_alpha) {
                    SPRTF("(%d,%d,%d,%d) ", r, g, b, a );
                } else {
                    SPRTF("(%d,%d,%d) ", r, g, b, a );
                }
            }
            SPRTF("\n");
        }
        add_std_out(val);
    }
#ifdef HAVE_EASYBMP
    write_bmp_file( width, height, pixels );
#endif // HAVE_EASYBMP
exit:
    if (out)
        lodepng_free(out);
    if (pixels)
        lodepng_free(pixels);
    lodepng_state_cleanup(&state);
    return iret;
}

// main() OS entry
int main( int argc, char **argv )
{
    int iret = 0;
    iret = parse_args(argc,argv);
    if (iret)
        return iret;
    set_log_file((char *)def_log,0);

    iret = dump_png_input(); // action of app

    return iret;
}


// eof = png-dump2.cxx
