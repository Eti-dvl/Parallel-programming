/**
 * @file image_file_io.c
 * @brief Basic image processing library: read/write NetBPM files
 * @author Saint-Cirgue Arnaud _ Correge Etienne
 * @version 0.1
 * @date october 2020
 */

/**
 * Read from / save to PNM format files.
 * 
 * Recall the following definitions:
 * 
 * File      Format name         Magic
 * extension                     ASCII   Binary
 * ---------------------------------------------
 * pbm       Portable Bitmap     P1      P4
 * pgm       Portable Graymap    P2      P5
 * ppm       Portable Pixmap     P3      P6
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <ctype.h>

#include "image_file_io.h"
#include "utils.h"


/**
 * @fn image_new_open(const char *fname)
 * @brief Image constructor; creates an image object from a PGM/PPM file.
 * @param fname path of image file
 * @return Handle of a new image object, NULL in case of failure.
 */
image_t *image_new_open(const char *fname) 
{
    FILE *fp;
    int width = 0, height = 0, depth = 0;
    image_t *self;
    image_type_t type;
    uint8_t pnm_format;
    uint8_t ascii_encoding;
    int c;

    fp = fopen(fname, "rb");
    if (!fp) 
    {
        fprintf(stderr, "Failed to open file `%s`", fname);
        return NULL;
    }
    
    if (getc(fp) != 'P') 
    {
        fprintf(stderr,"%s: not a Netpbm file", fname);
        return NULL;
    }

    pnm_format = getc(fp) - '0';
    if (pnm_format <= 0 || pnm_format > 6)
    {
        DIE("Not a Netpbm file");
    }

    /* read until end of line */
    while(getc(fp) != '\n') 
        continue;
    /* skip comments */
    while (getc(fp) == '#')
    {
        while (getc(fp) != '\n');
    }

    fseek(fp, -1, SEEK_CUR); // seems necessary in some cases...
    assert(1 == fscanf(fp,"%d", &width));
    assert(1 == fscanf(fp,"%d", &height));

    // basic plausibility checks
    assert(0 < width && width < 100000);
    assert(0 < height && height < 100000);

    ascii_encoding = 0;
    switch(pnm_format)
    {
    case 1: // P1 = black/white, 1 bit ; ascii encoding
        ascii_encoding = 1;
        do 
        {
            c = fgetc(fp);
        } while (c == '\r' || c == '\n');
        ungetc(c, fp);
        type = IMAGE_BITMAP;
        break;
    case 4: // P4 = black/white, 1 bit ; binary encoding
        do 
        {
            c = fgetc(fp);
        } while (c == '\r' || c == '\n');
        ungetc(c, fp);
        type = IMAGE_BITMAP;
        break;
    case 2: // P2 = 0-255 or 0-65535 grayascale ; ascii
        assert(1 == fscanf(fp,"%d", &depth));
        ascii_encoding = 1;
    case 5: // P5 = 0-255 or 0-65535 grayascale ; binary
        assert(1 == fscanf(fp,"%d", &depth));
        type = (depth < 256) ? IMAGE_GRAYSCALE_8 : IMAGE_GRAYSCALE_16;
        break;
    case 3: // P3 = 0-255 x 3 channels (R, G, B) color ; ascii
        assert(1 == fscanf(fp,"%d", &depth));
        ascii_encoding = 1;
    case 6: // P6 = 0-255 x 3 channels (R, G, B) color ; binary
        assert(1 == fscanf(fp,"%d", &depth));
        type = IMAGE_RGB_888;
        break;
    default:
        DIE("Format not supported");
    }

    self = image_new(width, height, type);
    if (!self) 
    {
        fprintf(stderr,"Image container failed");
        return NULL;
    }

    if (ascii_encoding) 
    {
        //DEBUG("Reading ASCII-encoded pixel data");
        FOR_YX(self, y, x)
        {
            uint32_t v, r, g, b;

            switch(self->type)
            {
            case IMAGE_BITMAP:
                assert(1 == fscanf(fp, "%d", &v));
                self->setpixel(self, x, y, (color_t){.bit = LIMIT(v, 0, 1)});
                break;
            case IMAGE_GRAYSCALE_8:
                assert(1 == fscanf(fp, "%d", &v));
                self->setpixel(self, x, y, (color_t){.gs8 = LIMIT(v, 0, 255)});
                break;
            case IMAGE_GRAYSCALE_16:
                assert(1 == fscanf(fp, "%d", &v));
                self->setpixel(self, x, y, (color_t){.gs16 = LIMIT(v, 0, 65535)});
                break;
            case IMAGE_RGB_888:
                assert(3 == fscanf(fp, "%d%d%d", &r, &g, &b));
                printf("%d %d %d\n", r, g, b);
                self->setpixel(self, x, y, (color_t){.rgb.r = LIMIT(r, 0, 255), .rgb.g = LIMIT(g, 0, 255), .rgb.b = LIMIT(b, 0, 255)});
                break;
            default:
                DIE("Not supported");
            }
        }
    }
    else 
    {
        //DEBUG_PRINT("Reading binary-encoded pixel data");
        long int nbytes;
        long int read;
        switch(self->type)
        {
        case IMAGE_BITMAP:
            // each pixel row is padded to an integer number of bytes
            nbytes = self->height * (int)((self->width + 7)/8);
            break;
        case IMAGE_GRAYSCALE_8:
            nbytes = self->width * self->height;
            break;
        case IMAGE_GRAYSCALE_16:
            nbytes = self->width * self->height * 2;
            break;
        case IMAGE_RGB_888:
            nbytes = self->width * self->height * 3;
            break;
        default:
            DIE("Unsupported");
        }

        read = fread(self->data, 1, nbytes, fp); //(self->width + 7)/8, self->height, fp);
        if (read < nbytes)
        {
            DIE("Expected %ld bytes, could read only %ld bytes.\n", nbytes, read);
        }
    }

    DEBUG_PRINT("Read file `%s` : ", fname);
    
    //image_print_details(self);
    return self;
}



/** 
 * @fn int image_save_ascii(const image_t *self, const char *fname)
 * @brief saves image data to NetBPM file, with ASCII encoding
 * @param self image object
 * @param fname path to file
 * @return 0 if success, -1 if failure
 */
int image_save_ascii(const image_t *self, const char *fname)
{
    return image_save(self, fname, 0);
}

/** 
 * @fn int image_save_binary(const image_t *self, const char *fname)
 * @brief saves image data to NetBPM file, with binary encoding
 * @param self image object
 * @param fname path to file
 * @return 0 if success, -1 if failure
 */
int image_save_binary(const image_t *self, const char *fname)
{
    return image_save(self, fname, 1);
}

/** 
 * @fn int image_save(const image_t *self, const char *fname, int binary_encoding)
 * @brief saves image data to file
 * @param self image object
 * @param fname path to file
 * @param binary_encoding set to 1 to use binary encoding (better performance), 0 for ASCII encoding (better readability)
 * @return 0 if success, -1 if failure
 */
int image_save(const image_t *self, const char *fname, int binary_encoding)
{
    FILE *fp;
    int depth;
    int format;
    
    fp = fopen(fname, "wb");
    if (!fp)
    {
        DIE("Could not open file %s", fname);
    }
    assert(fp);
    assert(self);
    assert(self->data);

    DEBUG_PRINT("Saving image @%p to file %s",
        (void *)self, fname);
    image_print_details(self);

    switch(self->type)
    {
    case IMAGE_BITMAP:
        format = 1;
        depth = 1;
        break;
    case IMAGE_GRAYSCALE_8:
        format = 2;
        depth = 255;
        break;
    case IMAGE_GRAYSCALE_16:
        format = 2;
        depth = 65535;
        break;
    case IMAGE_GRAYSCALE_FL:
        format = 2; 
        depth = 255;
        DEBUG_PRINT("Warning: floating-point grayscale values will be converted to 0...%d range", depth);
        break;
    case IMAGE_RGB_888:
        format = 3;
        depth = 255;
        break;
    default:
        DIE("Unsupported");
    }

    if (binary_encoding)
    {
        format += 3;
    }

    fprintf(fp, "P%d\n%d %d\n%d\n",
            format,
            self->width, self->height,
            depth);

    if (binary_encoding)
    {
        long int nbytes;
        switch(self->type)
        {
        case IMAGE_BITMAP:
            nbytes = (self->height * self->width + 7) / 8;
            break;
        case IMAGE_GRAYSCALE_8:
            nbytes = (self->height * self->width);
            break;
        case IMAGE_GRAYSCALE_16:
            nbytes = (self->height * self->width) * 2;
            break;
        case IMAGE_RGB_888:
            nbytes = (self->height * self->width) * 3;
            break;

        case IMAGE_GRAYSCALE_FL:
        default:
            DIE("Unsupported");
            break;
        }
        fwrite(self->data, nbytes, 1, fp);
    }
    else
    {
        FOR_YX(self, y, x)
        {
            color_t c;
            c = self->getpixel(self, x, y);

            switch(self->type)
            {
            case IMAGE_BITMAP:
                fprintf(fp, "%d\t", c.bit);
                break;
            case IMAGE_GRAYSCALE_8:
                fprintf(fp, "%d\t", c.gs8);
                break;
            case IMAGE_GRAYSCALE_16:
                fprintf(fp, "%d\t", c.gs16);
                break;
            case IMAGE_GRAYSCALE_FL:
                fprintf(fp, "%d\t", (uint16_t)(depth * LIMIT(c.fl, 0.0, 1.0)));
                break;
            case IMAGE_RGB_888:
                fprintf(fp, "%d\t%d\t%d\t", c.rgb.r, c.rgb.g, c.rgb.b);
                break;
            }

            if (x == self->width - 1)
            {
                fprintf(fp, "\n");
            }
        }
    }
    DEBUG_PRINT("File saved");
    fclose(fp);
    return 0;
}

