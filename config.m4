dnl config.m4 for extension hoedown

dnl Check PHP version:
AC_MSG_CHECKING(PHP version)
if test ! -z "$phpincludedir"; then
    PHP_VERSION=`grep 'PHP_VERSION ' $phpincludedir/main/php_version.h | sed -e 's/.*"\([[0-9\.]]*\)".*/\1/g' 2>/dev/null`
elif test ! -z "$PHP_CONFIG"; then
    PHP_VERSION=`$PHP_CONFIG --version 2>/dev/null`
fi

if test x"$PHP_VERSION" = "x"; then
    AC_MSG_WARN([none])
else
    PHP_MAJOR_VERSION=`echo $PHP_VERSION | sed -e 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\).*/\1/g' 2>/dev/null`
    PHP_MINOR_VERSION=`echo $PHP_VERSION | sed -e 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\).*/\2/g' 2>/dev/null`
    PHP_RELEASE_VERSION=`echo $PHP_VERSION | sed -e 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\).*/\3/g' 2>/dev/null`
    AC_MSG_RESULT([$PHP_VERSION])
fi

if test $PHP_MAJOR_VERSION -lt 5; then
   AC_MSG_ERROR([need at least PHP 5.4 or newer])
fi

if test $PHP_MAJOR_VERSION -eq 5 -a $PHP_MINOR_VERSION -lt 4; then
    AC_MSG_ERROR([need at least PHP 5.4 or newer])
fi

dnl Hoedown Extension
PHP_ARG_ENABLE(hoedown, whether to enable hoedown support,
[  --enable-hoedown      Enable hoedown support])

if test "$PHP_HOEDOWN" != "no"; then

    dnl Source hoedown
    HOEDOWN_SOURCE="hoedown/src/autolink.c hoedown/src/escape.c hoedown/src/html_smartypants.c hoedown/src/buffer.c hoedown/src/html.c hoedown/src/stack.c hoedown/src/document.c hoedown/src/html_blocks.c hoedown/src/version.c"

    dnl PHP Extension
    PHP_NEW_EXTENSION(hoedown, hoedown.c $HOEDOWN_SOURCE, $ext_shared)
fi

dnl coverage
PHP_ARG_ENABLE(coverage, whether to enable coverage support,
[  --enable-coverage     Enable coverage support], no, no)

if test "$PHP_COVERAGE" != "no"; then
    EXTRA_CFLAGS="--coverage"
    PHP_SUBST(EXTRA_CFLAGS)
fi
