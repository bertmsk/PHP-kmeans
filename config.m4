PHP_REQUIRE_CXX()
PHP_ADD_LIBRARY(stdc++, 1, PHP5CPP_SHARED_LIBADD)


PHP_ARG_WITH(kmeans, for kmeans support, [  --with-kmeans     Enable KMEANS support])

AC_DEFINE(HAVE_LIBGD20, 1, [Define to 1 if the GD functions are available in PHP])

if test "$PHP_KMEANS" != "no"; then
  PHP_SUBST(KMEANS_SHARED_LIBADD)
  PHP_ADD_INCLUDE(/usr/local/include)
  PHP_ADD_INCLUDE(.)
  PHP_ADD_INCLUDE(./src)

  AC_DEFINE(HAVE_OPENCV, 1, [ ])
  
  CFLAGS="$CFLAGS -Wall -fno-strict-aliasing"
  
  PHP_NEW_EXTENSION(kmeans, kmeans.cpp, $ext_shared, , \\$(GDLIB_CFLAGS))
  PHP_ADD_EXTENSION_DEP(kmeans, gd)
fi

