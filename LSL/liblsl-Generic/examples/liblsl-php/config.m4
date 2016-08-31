dnl config.m4 for extension php_liblsl
  
PHP_ARG_ENABLE(liblsl, whether to enable liblsl support,
 [  --enable-liblsl           Enable liblsl support])
  
if test "$PHP_LIBLSL" != "no"; then
    PHP_REQUIRE_CXX()
    PHP_SUBST(LIBLSL_SHARED_LIBADD)
    PHP_ADD_LIBRARY(stdc++, 1, LIBLSL_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(lsl64, ./libs, LIBLSL_SHARED_LIBADD)
    PHP_ADD_INCLUDE(./)
    PHP_NEW_EXTENSION(liblsl, liblsl_c_wrap.cpp, $ext_shared)
fi
