// from : http://stackoverflow.com/questions/25057471/drawing-3d-sphere-in-c-c
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define   BORDER  2
#define   XC(x,y,z) ((x)*16 + (y)*12)
#define   YC(x,y,z) ((x)*6 - (y)*8 - (z)*17)

static int xt = 0;
static int yt = 0;

static void fcube(FILE *out, const int x, const int y, const int z, const int fill)
{
    const int xc = xt + XC(x,y,z);
    const int yc = yt + YC(x,y,z);
    fprintf(out, "<path d=\"M%d,%dl16,6,12,-8,0,-17,-16,-6,-12,8z\" fill=\"#%06x\" stroke=\"#000\" />\n", xc, yc, fill & 0xFFFFFF);
    fprintf(out, "<path d=\"M%d,%dl16,6,12,-8m-12,8l0,17\" fill=\"none\" stroke=\"#000\" />\n", xc, yc-17);
}

static unsigned long rrmin = 0UL;
static unsigned long rrmax = 0UL;
static int           center = 0;

static int surface(const int x, const int y, const int z)
{
    /* Doubled coordinates: */
    const long dx = 2*x - center,
               dy = 2*y - center,
               dz = 2*z - center;
    const unsigned long rr = dx*dx + dy*dy + dz*dz;

    return (rrmin <= rr) && (rr <= rrmax);
}

int main(int argc, char *argv[])
{
    int  width, height;
    int  size, x, y, z;
    char dummy;

    if (argc != 4 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
        fprintf(stderr, "\n");
        fprintf(stderr, "Usage: %s SIZE RRMIN RRMAX\n", argv[0]);
        fprintf(stderr, "Where\n");
        fprintf(stderr, "       SIZE    is the size of the voxel cube\n");
        fprintf(stderr, "       RRMIN   is the minimum distance squared, and\n");
        fprintf(stderr, "       RRMAX   is the maximum distance squared,\n");
        fprintf(stderr, "               using doubled coordinates.\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "Examples:\n");
        fprintf(stderr, "       %s 3 1 4\n", argv[0]);
        fprintf(stderr, "       %s 4 11 11\n", argv[0]);
        fprintf(stderr, "       %s 5 8 16\n", argv[0]);
        fprintf(stderr, "       %s 5 12 20\n", argv[0]);
        fprintf(stderr, "       %s 5 16 24\n", argv[0]);
        fprintf(stderr, "\n");
        return EXIT_FAILURE;
    }

    if (sscanf(argv[1], " %d %c", &size, &dummy) != 1 || size < 3) {
        fprintf(stderr, "%s: Invalid size.\n", argv[1]);
        return EXIT_FAILURE;
    }

    if (sscanf(argv[2], " %lu %c", &rrmin, &dummy) != 1) {
        fprintf(stderr, "%s: Invalid rrmin.\n", argv[2]);
        return EXIT_FAILURE;
    }

    if (sscanf(argv[3], " %lu %c", &rrmax, &dummy) != 1 || rrmax < rrmin) {
        fprintf(stderr, "%s: Invalid rrmax.\n", argv[3]);
        return EXIT_FAILURE;
    }

    /* Calculate coordinate range. */
    {   int xmin = XC(0,0,0);
        int ymin = YC(0,0,0);
        int xmax = XC(0,0,0);
        int ymax = YC(0,0,0);

        for (z = 0; z <= size; z++)
            for (y = 0; y <= size; y++)
                for (x = 0; x <= size; x++) {
                    const int xc = XC(x,y,z);
                    const int yc = YC(x,y,z);
                    if (xc < xmin) xmin = xc;
                    if (xc > xmax) xmax = xc;
                    if (yc < ymin) ymin = yc;
                    if (yc > ymax) ymax = yc;
                } 

        xt = BORDER - xmin;
        width = xmax - xmin + 2*BORDER;

        yt = BORDER - ymin;
        height = ymax - ymin + 2*BORDER;
    }

    center = size - 1;

    /* SVG preamble. */
    printf("<?xml version=\"1.0\"?>\n");
    printf("<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 %d %d\">\n", width, height);
    printf("<path d=\"M%d,%dL%d,%d,%d,%d,%d,%d,%d,%d,%d,%dz\" fill=\"#f7f7f7\" stroke=\"#666666\"/>\n",
            xt+XC(   0,   0,   0), yt+YC(   0,   0,   0),
            xt+XC(size,   0,   0), yt+YC(size,   0,   0),
            xt+XC(size,size,   0), yt+YC(size,size,   0),
            xt+XC(size,size,size), yt+YC(size,size,size),
            xt+XC(0,   size,size), yt+YC(   0,size,size),
            xt+XC(0,      0,size), yt+YC(   0,   0,size));
    printf("<path d=\"M%d,%dL%d,%d,%d,%dM%d,%dL%d,%d\" fill=\"none\" stroke=\"#666666\"/>\n",
            xt+XC(   0,   0,   0), yt+YC(   0,   0,   0),
            xt+XC(   0,size,   0), yt+YC(   0,size,   0),
            xt+XC(size,size,   0), yt+YC(size,size,   0),
            xt+XC(   0,size,   0), yt+YC(   0,size,   0),
            xt+XC(   0,size,size), yt+YC(   0,size,size));
    for (z = 0; z < size; z++)
        for (y = size - 1; y >= 0; y--)
            for (x = 0; x < size; x++)
                if (surface(x,y,z))
                    fcube(stdout, x, y, z, 0x00CCFF);
    printf("</svg>\n");

    for (z=0; z < size; z++) {
        for (y = 0; y < size; y++) {
            for (x = 0; x < size; x++)
                fputc(surface(x,y,z) ? 'X' : '.', stderr);
            fputs("   ", stderr);
            for (x = 0; x < size; x++)
                fputc(surface(x,z,y) ? 'X' : '.', stderr);
            fputs("   ", stderr);
            for (x = 0; x < size; x++)
                fputc(surface(y,z,x) ? 'X' : '.', stderr);
            fputc('\n', stderr);
        }
        fputc('\n', stderr);
    }

    return EXIT_SUCCESS;
}

// eof
