#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

// image width/height
#define X_SIZE 4096
#define Y_SIZE 4096
// maximum number of iterations
#define IT_MAX 255

//Thread structure parameters
typedef struct ThreadParams 
{
    long ystart, yend;
    double cx, cy;
    char* raster;
    int id;
}ThreadParams_t;

// Function prototypes
void *compute_set(void *arg);
long compute_point (double zx, double zy, double cx, double cy);
int read_seed (double *cx, double *cy);
void write_pgm( char *raster, char *name);

int main (int argc, char **argv) 
{
    char name [256];
    double cx, cy;
    int numThreads = atoi(argv[1]);  // Dedault value
    char *raster;
    pthread_t threads[numThreads];
    ThreadParams_t threadParams[numThreads];

    // Allocate memory for image
    raster = (char *) malloc (Y_SIZE * X_SIZE);

    // For each (x,y) pair read from input stream:
    while (read_seed(&cx, &cy)) 
    {
        for (int i = 0; i < numThreads; i++) 
        {
            // Répartir les sections de l'image entre les threads
            threadParams[i].ystart = (long)(i*(Y_SIZE / numThreads));
            threadParams[i].yend = (long)((i+1)*(Y_SIZE / numThreads));
            if(i == numThreads -1){threadParams[i].yend = Y_SIZE;}
            threadParams[i].cx = cx;
            threadParams[i].cy = cy;
            threadParams[i].raster = raster;
            threadParams[i].id = i;
            pthread_create(&threads[i], NULL, compute_set, (void *)&threadParams[i]);
        }

        for (int i = 0; i < numThreads; i++) 
        {
            pthread_join(threads[i], NULL);
        }

        // Write figure to PGM file
        sprintf (name, "julia_%f_%f.pgm", cx, cy);
        write_pgm(raster, name);
    }

    // Clean up
    free(raster);
    return 0;
}


// Compute a fractal image
void *compute_set(void *arg) 
{
    long x, y;
    double zx, zy;
    struct ThreadParams *params = (struct ThreadParams *)arg;
    
    //printf("id : %d , %ld -> %ld\n",params->id, params->ystart, params->yend);
    for (y = params->ystart; y < params->yend ; y++){
        zy = 4.0 * (double) y / (double) (Y_SIZE - 1) - 2.0;
        for (x = 0; x < X_SIZE; x++){
            zx = 4.0 * (double) x / (double) (X_SIZE - 1) - 2.0;
            params->raster[y * X_SIZE + x ] = compute_point ( zx, zy, params->cx, params->cy);
        }
    }

    pthread_exit(NULL);
}

// Compute 1 point
long compute_point (double zx, double zy, double cx, double cy) 
{
    double zx_temp, zy_temp;
    long it = 0;
    while ((it < IT_MAX) && ((zx * zx) + (zy * zy) < 4.0)) 
    {
        zx_temp = zx * zx - zy * zy + cx;
        zy_temp = 2 * zx * zy + cy;
        zx = zx_temp;
        zy = zy_temp;
        it++;
    }
    return it;
}

// Read a (cx, cy) pair, or return false
int read_seed (double *cx, double *cy) 
{
    return scanf ("%lf %lf\n", cx, cy) != EOF;
}

// Write raster to file
void write_pgm( char *raster, char *name) 
{
    FILE *fp;
    fp = fopen (name, "wb");
    fprintf (fp , "P5 %d %d %d\n", X_SIZE, Y_SIZE, IT_MAX);
    fwrite (raster , 1 , X_SIZE * Y_SIZE, fp);
    fclose (fp);
}