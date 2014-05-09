#ifndef PHP_HOEDOWN_H
#define PHP_HOEDOWN_H

#define HOEDOWN_EXT_VERSION "0.3.2"

extern zend_module_entry hoedown_module_entry;
#define phpext_hoedown_ptr &hoedown_module_entry

#ifdef PHP_WIN32
#    define PHP_HOEDOWN_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#    define PHP_HOEDOWN_API __attribute__ ((visibility("default")))
#else
#    define PHP_HOEDOWN_API
#endif

#ifdef ZTS
#    include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(hoedown)
    char *options;
ZEND_END_MODULE_GLOBALS(hoedown)

#ifdef ZTS
#    define HOEDOWN_G(v) TSRMG(hoedown_globals_id, zend_hoedown_globals *, v)
#else
#    define HOEDOWN_G(v) (hoedown_globals.v)
#endif

#define HOEDOWN_ERR(e, ...) php_error_docref(NULL TSRMLS_CC, e, __VA_ARGS__)

#endif  /* PHP_HOEDOWN_H */
