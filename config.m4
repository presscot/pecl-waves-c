dnl +----------------------------------------------------------------------+
dnl | PHP Version 7                                                        |
dnl +----------------------------------------------------------------------+
dnl | Copyrght © 2019 Waves Platform                                       |
dnl +----------------------------------------------------------------------+
dnl | This source file is subject to version 3.01 of the PHP license,      |
dnl | that is bundled with this package in the file LICENSE, and is        |
dnl | available through the world-wide-web at the following url:           |
dnl | http://www.php.net/license/3_01.txt                                  |
dnl | If you did not receive a copy of the PHP license and are unable to   |
dnl | obtain it through the world-wide-web, please send a note to          |
dnl | license@php.net so we can mail you a copy immediately.               |
dnl +----------------------------------------------------------------------+

PHP_ARG_WITH(waves, for Waves support,
[  --with-waves           Include waves support])
PHP_ARG_WITH(secp256k1, for secp256k1 support,
[  --with-secp256k1           Include secp256k1 support])
PHP_ARG_ENABLE(waves-debug, for Waves debug support,
[  --enable-waves-debug   Enable Waves debug support], no, no)

if test "$PHP_WAVES" != "no"; then
  dnl Save LDFLAGS and LIBS (that we may require for the test involving compilation)
  OLD_LDFLAGS="$LDFLAGS"
  OLD_LIBS="$LIBS"

  dnl {{{ Check PHP version
  AC_MSG_CHECKING(whether Waves supports the current PHP version)
  tmp_php_version=$PHP_VERSION
  if test -z "$tmp_php_version"; then
    if test -z "$PHP_CONFIG"; then
      AC_MSG_ERROR([php-config not found])
    fi
    PHP_WAVES_VERSION_ORIG=`$PHP_CONFIG --version`
  else
    PHP_WAVES_VERSION_ORIG=$tmp_php_version
  fi
  if test -z $PHP_WAVES_VERSION_ORIG; then
    AC_MSG_ERROR([failed to detect PHP version, please file a bug])
  fi

  PHP_WAVES_VERSION_MASK=`echo "$PHP_WAVES_VERSION_ORIG" | $AWK 'BEGIN {FS = ".";} {printf "%d", ($1 * 1000 + $2) * 1000 + $3;}'`
  if test $PHP_WAVES_VERSION_MASK -lt 7001000; then
    AC_MSG_ERROR([need at least PHP 7.1.0])
  else
    AC_MSG_RESULT([ok])
  fi
  dnl }}}

  dnl {{{ Include libwaves_c headers
  AC_MSG_CHECKING([for include/waves/b58.h])
  WAVES_DIR=
  for i in "$PHP_WAVES" /usr/local /usr /opt /opt/local; do
    if test -f "$i/include/waves/b58.h" \
      && test -f "$i/include/waves/crypto.h"
    then
      WAVES_DIR="$i"
      break
    fi
  done
  if test "x$WAVES_DIR" = "x"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the libwaves_c library])
  fi
  AC_MSG_RESULT([found in $WAVES_DIR])
  PHP_ADD_INCLUDE($WAVES_DIR/include)
  dnl }}}

  dnl TODO: check for libwaves_c version (try to compile etc.)

  dnl {{{ Setup OpenSSL
  test -z "$PHP_OPENSSL_DIR" && PHP_OPENSSL_DIR=yes
  test -z "$PHP_OPENSSL_DIR" && PHP_OPENSSL_DIR=yes
  PHP_SETUP_OPENSSL(WAVES_SHARED_LIBADD)
  dnl }}}

  WAVES_LIBDIR="$WAVES_DIR/$PHP_LIBDIR/waves"
  WAVES_LIBS="-L$WAVES_LIBDIR -L/usr/local/$PHP_LIBDIR/waves"
  LDFLAGS="$WAVES_LIBS $LDFLAGS"
  LIBS="$LIBS -lwaves_c"

  dnl {{{ libwaves_c
  AC_SEARCH_LIBS(base58_decode, [waves_c], [
    PHP_ADD_LIBRARY_WITH_PATH(waves_c, $WAVES_LIBDIR, WAVES_SHARED_LIBADD)
    LDFLAGS="$LDFLAGS -lwaves_c"
    AC_DEFINE(HAVE_WAVES_C_LIB, 1, [Whether libwaves_c library is installed])
  ], [
    AC_MSG_ERROR([base58_decode not found in libwaves_c library, or the library is not installed.])
  ], [
    dnl libwaves_c depends on libcrypto
    -lcrypto
  ])
  dnl }}}

  dnl {{{ secp256k1
  AC_SEARCH_LIBS(secp256k1_context_create, [libsecp256k1.so.0], [
    PHP_ADD_LIBRARY_WITH_PATH(secp256k1, /usr/local/lib, WAVES_SHARED_LIBADD)
    LDFLAGS="$LDFLAGS -lsecp256k1"
    AC_DEFINE(HAVE_WAVES_C_LIB, 1, [Whether libwaves_c library is installed])
  ], [
WAVES_SHARED_LIBADD="$WAVES_SHARED_LIBADD -L/usr/local/lib/secp256k1 -lsecp256k1"

//    AC_MSG_ERROR([$WAVES_SHARED_LIBADD ,,, $WAVES_LIBDIR])
  ], [
    AC_MSG_ERROR([123])
  ])
  dnl }}}


  dnl {{{ --enable-waves-debug
  if test "$PHP_WAVES_DEBUG" != "no"; then
    PHP_WAVES_CFLAGS="$PHP_WAVES_CFLAGS -Wall -g -ggdb -O0"
    AC_DEFINE(PHP_WAVES_DEBUG, 1, [Enable waves debug support])
  else
    AC_DEFINE(NDEBUG, 1, [With NDEBUG defined assert generates no code])
  fi
  dnl }}}

  dnl {{{ Handle static build
  if test "$ext_shared" != "yes" && test "$ext_shared" != "shared"; then
    PHP_WAVES_CONFIG_H='\"main/php_config.h\"'
    AC_DEFINE(WAVES_CONFIG_H, "main/php_config.h", [config.h used in Waves extension])
    PHP_WAVES_CFLAGS="$PHP_WAVES_CFLAGS -DEV_CONFIG_H="$PHP_WAVES_CONFIG_H
    define('PHP_WAVES_STATIC', 1)
  fi
  dnl }}}

  PHP_WAVES_CFLAGS="-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1 $PHP_WAVES_CFLAGS"

  PHP_ADD_BUILD_DIR($abs_builddir/src, 1)
  PHP_ADD_INCLUDE([$ext_srcdir/src])

  PHP_ADD_BUILD_DIR($abs_builddir/src/classes, 1)
  PHP_ADD_INCLUDE([$ext_srcdir/src/classes])

  WAVES_SOURCES="\
    src/priv.c \
    src/properties.c \
    src/fe.c \
    src/classes/WavesAddress.c \
    src/classes/WavesPublicKey.c \
    src/classes/WavesSignature.c \
    src/classes/WavesPrivateKey.c \
    src/php_waves.c \
    "
  PHP_NEW_EXTENSION(waves, $WAVES_SOURCES, $ext_shared,, $PHP_WAVES_CFLAGS)
  PHP_ADD_INCLUDE($ext_builddir/src)
  PHP_SUBST(WAVES_SHARED_LIBADD)

  dnl Restore original LIBS and LDFLAGS
  LDFLAGS="$OLD_LDFLAGS"
  LIBS="$OLD_LIBS"

  PHP_ADD_MAKEFILE_FRAGMENT
fi

dnl vim: ft=m4.sh fdm=marker cms=dnl\ %s
dnl vim: et ts=2 sts=2 sw=2
