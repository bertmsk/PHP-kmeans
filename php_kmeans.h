/*
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
#ifndef PHP_KMEANS_H
#define PHP_KMEANS_H

#define PHP_KMEANS_VERSION "0.1"
#define PHP_KMEANS_EXTNAME "kmeans"

#ifdef ZTS
extern "C" {
# include "TSRM.h"
}
#endif


#include <map>
#include <vector>
#include <string>

extern "C" {

#include <php.h>
#include <ext/standard/info.h>

#include "gd.h"
#include "gd_io.h"



PHP_MINFO_FUNCTION(kmeans);

PHP_FUNCTION(kmeans);

extern zend_module_entry kmeans_module_entry;
#define phpext_kmeans_ptr &kmeans_module_entry

static zend_function_entry kmeans_functions[] = {
    PHP_FE(kmeans, NULL)
    {NULL, NULL, NULL}
};


#if ZEND_MODULE_API_NO >= 20050617 
static zend_module_dep kmeans_deps[] = { 
 ZEND_MOD_REQUIRED("gd") 
 {NULL, NULL, NULL} 
}; 
#endif
  
zend_module_entry kmeans_module_entry = {
#if ZEND_MODULE_API_NO >= 20050617 
  STANDARD_MODULE_HEADER_EX, NULL, kmeans_deps, 
#elif ZEND_MODULE_API_NO >= 20010901 
  STANDARD_MODULE_HEADER, 
#endif
    PHP_KMEANS_EXTNAME,
    kmeans_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    PHP_MINFO(kmeans),
#if ZEND_MODULE_API_NO >= 20010901
    PHP_KMEANS_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

ZEND_GET_MODULE(kmeans)


static int le_imgreco;
static void php_free_imgreco(zend_rsrc_list_entry *rsrc TSRMLS_DC);


PHP_MINFO_FUNCTION(kmeans)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "kmeans support", "enabled");
	php_info_print_table_row(2, "kmeans version", PHP_KMEANS_VERSION);
	php_info_print_table_end();
}

}

#endif

