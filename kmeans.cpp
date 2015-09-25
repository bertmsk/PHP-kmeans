/*
  k-means support
  
  Native PHP support for finding clusters centers in images using 
  k-means++ and libgd

  Author: Bert <bert@freecap.ru>


  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Robert Eisele <robert@xarg.org>                              |
  +----------------------------------------------------------------------+
*/

#include <math.h>
#include <algorithm>
#include <vector>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_kmeans.h"

using namespace std;

typedef struct { 
    double x, y, z; 
    int group; 
    uint32_t cnt;
} point_t, *point;
 
double randf(double m)
{
  return m * rand() / (RAND_MAX - 1.0f);
}

// compare square'd distance instead of square root, since 
// real distance are not necessary
inline double dist2(point a, point b)
{
	double 
	 x = a->x - b->x, 
         y = a->y - b->y,
         z = a->z - b->z
	 ;
	return x*x + y*y + z*z;
}
 
inline int
nearest(point pt, point cent, int n_cluster, double *d2)
{
	int i, min_i;
	point c;
	double d, min_d;

 
	for (c = cent, i = 0; i < n_cluster; i++, c++) {
		min_d = HUGE_VAL;
		min_i = pt->group;
		
		for (c = cent, i = 0; i < n_cluster; i++, c++) {
		   d = dist2(c, pt);
		   if (min_d > d) {
		     //php_printf("d= %f\n", d);
			    
		     min_d = d;
		     min_i = i;
		   }
		}
	}
	if (d2) *d2 = min_d;
	return min_i;
}
 
/*
 * k-means++
 */
void kpp(point pts, int len, point cent, int n_cent)
{
	int i, j;
	int n_cluster;
	double sum, *d = (double *)malloc(sizeof(double) * len);
 
	point p, c;
	
	// Select first cluster center
	cent[0] = pts[ rand() % len ];
	
	for (n_cluster = 1; n_cluster < n_cent; n_cluster++) {
		sum = 0;
		for (j = 0, p = pts; j < len; j++, p++) {
			nearest(p, cent, n_cluster, d + j);
			sum += d[j];
		}
		
		sum *= (rand() / (RAND_MAX - 1.0f));
		for (j = 0, p = pts; j < len; j++, p++) {
		   if ((sum -= d[j]) > 0) continue;
		   cent[n_cluster] = pts[j];
		   break;
		}
	}
	
	for (j = 0, p = pts; j < len; j++, p++) 
	  p->group = nearest(p, cent, n_cluster, 0);
	  
	free(d);
}
 
point lloyd(point pts, int len, int n_cluster, bool useplus)
{
	int i, j, min_i;
	int changed;
 
        // centroids
	point cent = (point)malloc(sizeof(point_t) * n_cluster), p, c;
 
	if (!useplus) {
	  // assign init grouping randomly
	  for (j = 0, p = pts; j < len; j++, p++)
	    p->group = j % n_cluster;
        } else {
	  // or call k++ init
	  kpp(pts, len, cent, n_cluster);
        }
	
	do {
		// group element for centroids are used as counters
		for (c = cent, i = 0; i < n_cluster; i++, c++) { 
		  c->group = 0; 
		  c->x = c->y = c->z = 0; 
		}
		for (j = 0, p = pts; j < len; j++, p++) {
			c = cent + p->group;
			
			c->group += p->cnt;
			
			c->x += p->x * p->cnt; 
			c->y += p->y * p->cnt;
			c->z += p->z * p->cnt;
		}
		for (c = cent, i = 0; i < n_cluster; i++, c++) { 
		    c->x /= c->group; 
		    c->y /= c->group; 
		    c->z /= c->group; 
		}
 
		changed = 0;
		/* find closest centroid of each point */
		for (j = 0, p = pts; j < len; j++, p++) {
			min_i = nearest(p, cent, n_cluster, 0);
			if (min_i != p->group) {
				changed++;
				p->group = min_i;
			}
		}
	} while (changed > (len >> 10)); /* stop when 99.9% of points are good */
 
//	for (c = cent, i = 0; i < n_cluster; i++, c++) { c->group = i; }
 
	return cent;
}


static int le_gd;

/* {{{  */
PHP_FUNCTION(kmeans)
{
  zval *IM, *array, *keypoints_array, *desc_array;
  gdImagePtr im;

  long numClusters;
  long useplus = 0;
  
  time_t t;
  srand((unsigned) time(&t));
  
  
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl|l", &IM, &numClusters, &useplus) == FAILURE) {
    RETURN_NULL();
  }

  // link with libgd
  ZEND_GET_RESOURCE_TYPE_ID(le_gd, (char *)"gd");
  ZEND_FETCH_RESOURCE(im, gdImagePtr, &IM, -1, "Image", le_gd);
  
  if (!im)  
    RETURN_FALSE;
  

  int width = im->sx;
  int height = im->sy;
  uint32_t count = 0;

  point p = (point)malloc(sizeof(point_t) * width * height);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      
      if (gdImageBoundsSafe(im, x, y)) {
        float r = 0, g = 0, b = 0;
        if (im->trueColor && im->tpixels) {
          int c = im->tpixels[y][x];
          r = (c >> 16) & 0xff;
          g = (c >> 8) & 0xff;
          b = c & 0xff;
        } else if (!im->trueColor && im->pixels) {
          unsigned char c = im->pixels[y][x];
          r = im->red[c];
          g = im->green[c];
          b = im->blue[c];
        }
/*	
	bool found = false;
	for (int i = 0; i < count; i++) { 
	  if ((int)p[i].x == r && (int)p[i].y == g && (int)p[i].z == b) {
	    found = true;
	    p[i].cnt++;
	    break;
	  }
	}
	if (!found) {
*/	
          p[count].x = r;
          p[count].y = g;
          p[count].z = b;
	  p[count].cnt = 1;
	  count++;
//	}

      }
    }
  }
  
//  php_printf("count = %d", count);
  
  point c = lloyd(p, count, numClusters, useplus);
    
  array_init(return_value);
      
  for (int i = 0 ; i < numClusters; i++) {
     MAKE_STD_ZVAL(array);
     array_init(array);
	    
     add_assoc_double(array, "r", c[i].x);
     add_assoc_double(array, "g", c[i].y);
     add_assoc_double(array, "b", c[i].z);
     add_assoc_double(array, "cnt", c[i].group);
     
     add_next_index_zval(return_value, array);
  }
  
  free(p);
  free(c);
  
}
/* }}} */

