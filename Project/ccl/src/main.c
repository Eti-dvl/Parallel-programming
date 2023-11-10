/**
 * @file main.c
 * @brief Programme principal pour le Projet ING5SE Analyse des composantes connexes
 * @author Etienne HAMELIN
 * @version 0.1
 * @date octobre 2020
 */
#include <stdio.h>
#include <stdlib.h>
#include "image_lib.h"


void test_image_connected_components(const char *fname)
{
  /* Allocate image structure for input image (expect a bitmap, i.e; black/white image) */
  image_t *img = image_new_open(fname);
  assert(img);
  assert(img->type == IMAGE_BITMAP);

  /* Allocate a 2D table (image) structure for holding tags; as a 16bit grayscale so that it can hold up to 65536 temp tags */
  image_t *img_tag = image_new(img->width, img->height, IMAGE_GRAYSCALE_16);
  assert(img_tag);

  /* Allocate image structure for output color image, for visualization */
  image_t *img_colors = image_new(img->width, img->height, IMAGE_RGB_888);
  assert(img_colors);

  /* Actually call the connected components labelling procedure */
  (void)image_connected_components(img, img_tag, img_colors);

  image_delete(img_tag);
  image_delete(img_colors);
  image_delete(img);
  return;
}

int main(int argc, char **argv)
{
  printf("Started.\n");

  if (argc > 1)
  {
    test_image_connected_components(argv[1]);
  }
  else
  {
    DEBUG_PRINT("Using default parameters");
    test_image_connected_components("img/test1.pbm");
  }
    
  printf("Finished.\n");
  return 0;
}
