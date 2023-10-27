#include <stdio.h>
#include <stdlib.h>

// image width/height
#define X_SIZE 4096
#define Y_SIZE 4096
// maximum number of iterations
#define IT_MAX 255

// Function prototypes
void compute_set ( char *raster, double cx, double cy);
long compute_point (double zx, double zy, double cx, double cy);
int read_seed (double *cx, double *cy);
void write_pgm( char *raster, char *name);


int main (int argc, char **argv) {
    char *raster;
    char name [256];
    double cx, cy;

    // Allocate memory for image
    raster = (char *) malloc (Y_SIZE * X_SIZE);

    // For each (x,y) pair read from input stream:
    while (read_seed(&cx, &cy)) {
        // Compute the figure
        compute_set (raster, cx, cy);

        // Write figure to PGM file
        sprintf (name, "julia_%f_%f.pgm", cx, cy);
        write_pgm(raster, name);
    }

    // Don't forget to free memory
    free(raster);
    return 0;
}

// Compute a fractal image
void compute_set(char *raster, double cx, double cy) {
    long x, y;
    double zx, zy;
    for (y = 0; y < Y_SIZE; y++) {
        zy = 4.0 * (double) y / (double) (Y_SIZE - 1) - 2.0;
        for (x = 0; x < X_SIZE; x++) {
            zx = 4.0 * (double) x / (double) (X_SIZE - 1) - 2.0;
            raster[y * X_SIZE + x ] = compute_point ( zx, zy, cx, cy);
        }
    }
}

// Compute 1 point
long compute_point (double zx, double zy, double cx, double cy) {
    double zx_temp, zy_temp;
    long it = 0;
    while ((it < IT_MAX) && ((zx * zx) + (zy * zy) < 4.0)) {
        zx_temp = zx * zx - zy * zy + cx;
        zy_temp = 2 * zx * zy + cy;
        zx = zx_temp;
        zy = zy_temp;
        it++;
    }
    return it;
}

// Read a (cx, cy) pair, or return false
int read_seed (double *cx, double *cy) {
    return scanf ("%lf %lf\n", cx, cy) != EOF;
}

// Write raster to file
void write_pgm( char *raster, char *name) {
    FILE *fp;
    fp = fopen (name, "wb");
    fprintf (fp , "P5 %d %d %d\n", X_SIZE, Y_SIZE, IT_MAX);
    fwrite (raster , 1 , X_SIZE * Y_SIZE, fp);
    fclose (fp);
}