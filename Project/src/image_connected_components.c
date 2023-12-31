/**
 * @file image_connected_components.c
 * @brief Image processing library: connected components labeling
 * @author Saint-Cirgue Arnaud _ Correge Etienne
 * @version 0.1
 * @date octobre 2020
 */
#include "image_connected_components.h"
#include <omp.h>

/**
 * @brief Assign a different color to each tag
 * @param tag a class tag
 * @return a color_t RGB color
 */
color_t class_color(int tag)
{
  /* multiply class id by golden angle: move along the color wheel */
  float hue = 137.507 * tag;
  /* also change saturation & value by a cycle of 9 */
  float sat = 0.33 * (3 -  tag % 3);
  float val = 0.33 * (3 - (tag / 3) % 3); 
  return (color_t){.rgb = rgb_from_hsv((hsv_t){.h = hue, .s = sat, .v = val})};
}

/**
 * @brief Find the root ancestor of a given tag
 * @param table Table of class ancestors
 * @param tag the original tag (expected > 0)
 * @return The root of the tree containing given tag
 * 
 * This recursion is guaranteed to finish since:
 * we always progress down to 0, since by construction we always have table[c] <= c
 */
int find_root(int *table, int tag)
{
  while ((tag > 0) && (table[tag] > 0) && (table[tag] < tag))
  {
    tag = table[tag];
  }
  return tag;
}

/**
 * @brief Join two classes
 * @param table equivalence table
 * @param tag1 a tag of the first class
 * @param tag2 a tag of the second class
 * @return the root tag of the now common class
 */
int join(int *table, int tag1, int tag2)
{
  /* first look for each class' root ancestor */
  tag1 = find_root(table, tag1);
  tag2 = find_root(table, tag2);
  /* now join both roots */
  int t_min = (tag1 < tag2) ? tag1 : tag2;
  int t_max = (tag1 < tag2) ? tag2 : tag1;
  table[t_max] = t_min;
  return t_min;
}


/**
 * @brief Return the minimum strictly positive of two int values
 * @param a a int
 * @param b another int
 * @return min(a,b) if both are positive; a or b if only one of them is positive; 0 if both are null
 */
int min_non_zero(int a, int b)
{
  if (a > 0 && b > 0)
  {
    return MIN(a, b);
  }
  else
  {
    return MAX(0, MAX(a, b));
  }
}

/**
 * @brief First pass: assign a temporary tag to each pixel, and store equivalences in given table
 * @param self the input image (binary)
 * @param tags the (output) image for storing pixel tags
 * @param equiv_out the table holding equivalence classes
 * @return the number of temporary tags assigned
 */
int ccl_temp_tag(
      const image_t *self,
      image_t *tags, 
      int *equiv_out)
{
  assert(self && tags && equiv_out);
  int num_tags = 0;
  int x, y;
  bool bg_color;
  
  int first_line_thread[20] = {0};
  bool first_line_flag[20] = {0};

  DEBUG_PRINT("First step: assign temporary class tag");

  /* Detect background color: by convention, background is the color of the top-left pixel */
  bg_color = image_bmp_getpixel(self, 0, 0).bit;

  #pragma omp parallel shared(equiv_out, first_line_thread, num_tags)
  {  
    #pragma omp for private(x)
    for(y = 0; y < self->height; ++y)
    {
      if(first_line_flag[omp_get_thread_num()] == 0)
      {
        #pragma omp critical
        {
        first_line_thread[omp_get_thread_num()] = y;
        printf("\nThread %d Y = %d",omp_get_thread_num(), y);
        first_line_flag[omp_get_thread_num()] = 1;
        }
      }
      
      for (x = 0; x < self->width; ++x)
      {
        /* read current pixel color */
        bool pxl_color = image_bmp_getpixel(self, x, y).bit;
        
        /* by default, pixel tag is zero (background) */
        int tag = 0;

        if (pxl_color != bg_color) 
        {
          /* Current pixel is foreground color: give it a tag, but which one? */

          /* Read the tag (if any) of the North and West adjacent pixels */
          /* or 0, if outside image coordinate ranges */
          int tag_n = image_coord_check(tags, x, y-1) ? 
                image_gs16_getpixel(tags, x, y-1).gs16 
                : 0;
          int tag_w = image_coord_check(tags, x-1, y) ? 
                image_gs16_getpixel(tags, x-1, y).gs16
                : 0;

          /* Current pixel tag is the minimum non-zero of adjacent tags */
          tag = min_non_zero(tag_n, tag_w);

          /*S'il n'y a pas de voisins connexes alors on incrémentes dans la table des équivalences*/
          if (tag == 0)
          {
            #pragma omp critical
            {
              num_tags+=1;
              if(num_tags%100 == 0)
              {
                printf("\nnumber of tags : %d", num_tags);
              }
              assert(num_tags < MAX_TAGS);
              tag = num_tags;
              equiv_out[tag] = tag;
            }
          }
          
          if (tag_n > 0 && tag_w > 0 && tag_w != tag_n)
          {
            /* if neighbors have different tags: join them */
            join(equiv_out, tag_n, tag_w);
          }
        }
        /* store tag in the tags image structure */
        image_gs16_setpixel(tags, x, y, (color_t){.gs16 = tag});
      }
    }

    //printf("\n\nFIN DE LA PREMIERE SECTION\n");
    #pragma omp barrier

    for(y=0 ; y < 20 ; y++)
    {
      //printf("\nStart_line %d = %d", y, first_line_thread[y]);
    } 
    #pragma omp for private(x)
    for(y=1 ; y < omp_get_max_threads() ; y++)
    {
      for(x=0 ; x < self->width ; x++)
      {
        //printf("\nThread %d x = %d || y = %d",omp_get_thread_num(), x, first_line_thread[y]);
        bool pxl_color = image_bmp_getpixel(self, x, first_line_thread[y]).bit;

        if (pxl_color != bg_color) 
        {
          int tag = image_coord_check(tags, x, first_line_thread[y]) ? 
                  image_gs16_getpixel(tags, x, first_line_thread[y]).gs16 
                  : 0;
          //printf("\nTrouvééé");
          int tag_n = image_coord_check(tags, x, first_line_thread[y]-1) ? 
                  image_gs16_getpixel(tags, x, first_line_thread[y]-1).gs16 
                  : 0;

          if( tag_n > 0 )
          {
            
            join(equiv_out, tag_n, tag);
          }
        }
      }
    }
  }
  return num_tags;
}

/**
 * @brief Reduce equivalence table and renumber classes
 * @param equiv_table the input equivalence table
 * @param num_tags number of used tags in equivalence table
 * @param class_num_out (output) table of tag classes
 * @return number of connected components
 */
int ccl_reduce_equivalences(
      int *equiv_table, 
      int num_tags, 
      int *class_num_out)
{
  int num_classes = 0;

  for (int t = 1; t <= num_tags; ++t)
  {
    if (equiv_table[t] == t)
    {
      /* t is the root of an equivalence class: assign a new definitive class label */
      class_num_out[t] = ++num_classes;
    }
    else
    {
      /* t is not a root: renumber with the root tag of its class.
      Note that find_root(table, t) <= t (by construction), so that class_num[find_root(table, t)] is already set. */
      class_num_out[t] = class_num_out[find_root(equiv_table, t)];
    }
  }

  return num_classes;
}

/**
 * @brief Replace temporary tags by class number (connected component number)
 * @param tags image containing temporary tags (modified in place)
 * @param class_num table that associates tags to number
 */
void ccl_retag(image_t *tags, int *class_num)
{
  int x, y, t;
  
  #pragma omp parallel for private(x, t) shared(tags, class_num)
  for (y = 0; y < tags->height; ++y)
  {
    for (x = 0; x < tags->width; ++x)
    {
      /* initial pixel tag */
      t = image_gs16_getpixel(tags, x, y).gs16;
      if (t != 0) 
      {
        /* get connected component number from tag */
        t = class_num[t];
        image_gs16_setpixel(tags, x, y, (color_t){.gs16 = t});
      }
    }
  }
}

/**
 * @brief Analyze connected components
 * @param tags an image containing pixel (renumbered) tags
 * @param con_cmp table of connected components 
 * @param num_classes
 */
void ccl_analyze(
      const image_t *tags,
    image_connected_component_t *con_cmp,
    int num_classes) {

  const int num_sections = 4;  // Nombre de sections pour diviser l'image
  const int section_height = tags->height / num_sections;

  // Tableaux temporaires pour stocker les mises à jour par thread
  image_connected_component_t *temp_con_cmp =
      (image_connected_component_t *)calloc(num_sections * num_classes, sizeof(image_connected_component_t));
  assert(temp_con_cmp != NULL);

  #pragma omp parallel for shared(tags, con_cmp, num_classes, temp_con_cmp) schedule(dynamic)
  for (int section = 0; section < num_sections; ++section) {
    int start_y = section * section_height;
    int end_y = (section == num_sections - 1) ? tags->height : start_y + section_height;

    for (int y = start_y; y < end_y; ++y) {
      for (int x = 0; x < tags->width; ++x) {
        int tag = image_gs16_getpixel(tags, x, y).gs16;
        if (tag > 0 && tag <= num_classes) {
          int t = tag - 1;
          if (temp_con_cmp[section * num_classes + t].num_pixels == 0) {
            temp_con_cmp[section * num_classes + t] = (image_connected_component_t){
                .num_pixels = 1,
                .x1 = x,
                .y1 = y,
                .x2 = x,
                .y2 = y};
          } else {
            temp_con_cmp[section * num_classes + t].num_pixels++;
            temp_con_cmp[section * num_classes + t].x1 = MIN(temp_con_cmp[section * num_classes + t].x1, x);
            temp_con_cmp[section * num_classes + t].y1 = MIN(temp_con_cmp[section * num_classes + t].y1, y);
            temp_con_cmp[section * num_classes + t].x2 = MAX(temp_con_cmp[section * num_classes + t].x2, x);
            temp_con_cmp[section * num_classes + t].y2 = MAX(temp_con_cmp[section * num_classes + t].y2, y);
          }
        }
      }
    }
  }

  // Fusionner les résultats des threads
  #pragma omp parallel for shared(con_cmp, temp_con_cmp, num_classes)
  for (int t = 0; t < num_classes; ++t) {
    for (int section = 0; section < num_sections; ++section) {
      if (temp_con_cmp[section * num_classes + t].num_pixels > 0) {
        #pragma omp critical
        {
          if (con_cmp[t].num_pixels == 0) {
            con_cmp[t] = temp_con_cmp[section * num_classes + t];
          } else {
            con_cmp[t].num_pixels += temp_con_cmp[section * num_classes + t].num_pixels;
            con_cmp[t].x1 = MIN(con_cmp[t].x1, temp_con_cmp[section * num_classes + t].x1);
            con_cmp[t].y1 = MIN(con_cmp[t].y1, temp_con_cmp[section * num_classes + t].y1);
            con_cmp[t].x2 = MAX(con_cmp[t].x2, temp_con_cmp[section * num_classes + t].x2);
            con_cmp[t].y2 = MAX(con_cmp[t].y2, temp_con_cmp[section * num_classes + t].y2);
          }
        }
      }
    }
  }

  free(temp_con_cmp);
}

/**
 * @brief Draw a color representation of connected components
 * @param tags the reduced & renumbered pixel tags
 * @param color the (output) image structure
 */
void ccl_draw_colors(const image_t *tags, image_t *color)
{
  int x, y, t;
  assert(tags && color);
  for (y = 0; y < tags->height; ++y)
  {
    for (x = 0; x < tags->width; ++x)
    {
      t = image_gs16_getpixel(tags, x, y).gs16;
      if (t != 0)
      {
        image_rgb_setpixel(color, x, y, class_color(t-1));
      }
    }
  }
}

/**
 * @brief Draw a bounding box around each connected component
 * @param color the (output) image structure
 * @param con_cmp the table of identified connected components
 */
void ccl_draw_bounding_boxes(image_t *color, 
    image_connected_component_t *con_cmp, 
    int num_cc)
{
  int x, y, t;
  for (t = 0; t < num_cc; ++t)
  {
    int x1 = con_cmp[t].x1; 
    int y1 = con_cmp[t].y1;
    int x2 = con_cmp[t].x2; 
    int y2 = con_cmp[t].y2;
    color_t c = class_color(t);

    for (x = x1; x <= x2; ++x)
    {
      if ((x + y1) % 2) image_rgb_setpixel(color, x, y1, c);
      if ((x + y2) % 2) image_rgb_setpixel(color, x, y2, c);
    }
    for (y = y1; y <= y2; ++y)
    {
      if ((x1 + y) % 2) image_rgb_setpixel(color, x1, y, c);
      if ((x2 + y) % 2) image_rgb_setpixel(color, x2, y, c);
    }
  }
}

/**
 * @brief 
 * @param 
 * @return 
 */
void write_time_csv(double *time)
{
  FILE *csvFile = fopen("main.csv", "a");  // Ouvre le fichier en mode écriture

  if (csvFile == NULL) {
    perror("Erreur lors de l'ouverture du fichier");
  }

  // Exemple de données
  fprintf(csvFile, "%d,%.6f,%.6f,%.6f,%.6f\n",
    omp_get_max_threads(),
    time[5] - time[0],
    time[1] - time[0],  
    time[4] - time[3],
    time[5] - time[4]);

  fclose(csvFile);  // Ferme le fichier
}

/**
 * @brief Identify connected components in given image
 * @param self the input image (should be a binary black & white image, i.e. self->type = IMAGE_BITMAP)
 * @param tags an image structure for holding the connected components tags (should be a 16-bit grayscale image), or NULL
 * @param color an output image structure for holding a color visualization of connected components, or NULL
 * @param con_cmp_out pointer to a image_connected_component_t[] table that will be written by this procedure, for returning the detected connected components, or NULL
 * @param debug 1 if we shall print out equivalence classes, generate intermediate files, etc. Good for debug, not for performance.
 * @return the number of classes detected 
 */
int image_connected_components(
      const image_t *self, 
      image_t *tags, 
      image_t *color)
{
  int *equiv_table;
  int num_tags = 0;

  int *class_num;
  int num_cc;
  int t;
  double time[7];

  /* ~~~~~~~~~~ Verify input arguments, initialize tables ~~~~~~~~~~ */
  assert(self && 
        (self->type == IMAGE_BITMAP));

  assert(tags && 
        (tags->type == IMAGE_GRAYSCALE_16) &&
        (tags->width >= self->width) && 
        (tags->height >= self->height));

  assert(color &&
        (color->type == IMAGE_RGB_888) &&
        (color->width >= self->width) && 
        (color->height >= self->height));

  /* Allocate the equivalence table */
  equiv_table = calloc(MAX_TAGS, sizeof(int));
  assert(equiv_table);
  
  time[0] = omp_get_wtime();
  /* ~~~~~~~~~~ First step: assign temporary class tags ~~~~~~~~~~ */
  num_tags = ccl_temp_tag(self, tags, equiv_table);

  time[1] = omp_get_wtime();
  

#ifdef DEBUG
  /* save temporay tags as readable file "tags.pgm" */
  image_save_ascii(tags, "tags.pgm");
  DEBUG_PRINT("Tags equivalence table:");
  for (t = 0; t <= num_tags; ++t)
  {
    DEBUG_PRINT("T[%02d] = %02d", t, equiv_table[t]);
  }
#endif
  
  time[2] = omp_get_wtime();

  /* ~~~~~~~~~~ Second step: reduce equivalence classes and renumber ~~~~~~~~~~ */
  DEBUG_PRINT("Now reduce tag equivalence classes, and renumber those classes");
  /**
   * Reduce tag equivalence classes,
   * Renumber classes, so that they are numbered 0 .. num_classes-1
   * Recall: a class root ancestor A is charcterized by table[A] = A.
   */
  class_num = calloc(num_tags+1, sizeof(int));
  assert(class_num);
  num_cc = ccl_reduce_equivalences(equiv_table, num_tags, class_num);

#ifdef DEBUG
  DEBUG_PRINT("Tags renumbering:");
  for (t = 0; t <= num_tags; ++t)
  {
    DEBUG_PRINT("N[%02d] = %02d", t, class_num[t]);
  }
#endif

  time[3] = omp_get_wtime();


  /* ~~~~~~~~~~ Third step: replace temp tags by connected component number ~~~~~~~~~~ */
  DEBUG_PRINT("Re-tag");
  ccl_retag(tags, class_num);

#ifdef DEBUG
  image_save_ascii(tags, "classes.pgm");
#else
  image_save_binary(tags, "classes.pgm");
#endif

  time[4] = omp_get_wtime();


  /* ~~~~~~~~~~ Fourth step: generate useful outputs ~~~~~~~~~~ */
  DEBUG_PRINT("Analyze connected components");

  /* allocate & initialize connected components output structure */
  image_connected_component_t *con_cmp = calloc(num_cc, sizeof(image_connected_component_t));
  assert(con_cmp);
  ccl_analyze(tags, con_cmp, num_cc);

  /* What's the size of the largest connected component found? */
  int largest_cc = 0;
  for (t = 0; t < num_cc; ++t)
  {
    if (con_cmp[t].num_pixels > con_cmp[largest_cc].num_pixels) {
      largest_cc = t;
    }
  }

  time[5] = omp_get_wtime();

#ifdef DEBUG
  DEBUG_PRINT("Draw color output");
  /* draw connected components as a color image */
  ccl_draw_colors(tags, color);

  /* use BIN format for large images: optimize for speed */
  image_save_binary(color, "color.ppm");
#endif

  time[6] = omp_get_wtime();

  /* liberate allocated memory */
  free(equiv_table);
  free(class_num);

  /* note: caller is responsible for liberating the tags and color images */
  DEBUG_PRINT("End of connected components labeling");

  printf("Found %d connected components.\n", num_cc);
  printf("Largest connected component is class #%06d, has %9d pixels.\n", largest_cc, con_cmp[largest_cc].num_pixels);

  printf("Total time: %.6fs; temp tag: %.6f, save tags %.6f, reduce_equiv %.6f, retag/save %.6f, analyze %.6f, color %.6f\n", 
    time[5] - time[0],
    time[1] - time[0], 
    time[2] - time[1], 
    time[3] - time[2], 
    time[4] - time[3],
    time[5] - time[4],
    time[6] - time[5]);

  write_time_csv(time);
    
  return num_cc;
}
