#ifndef PHP_HOEDOWN_H
#define PHP_HOEDOWN_H

#define HOEDOWN_EXT_VERSION "0.5.0"

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

#ifndef ZED_FE_END
#define ZEND_FE_END { NULL, NULL, NULL, 0, 0 }
#endif

#ifndef ZVAL_COPY_VALUE
#define ZVAL_COPY_VALUE(z, v)      \
    do {                           \
        (z)->value = (v)->value;   \
        Z_TYPE_P(z) = Z_TYPE_P(v); \
    } while (0)
#endif

#ifndef INIT_PZVAL_COPY
#define INIT_PZVAL_COPY(z, v)   \
    do {                        \
        ZVAL_COPY_VALUE(z, v);  \
        Z_SET_REFCOUNT_P(z, 1); \
        Z_UNSET_ISREF_P(z);     \
    } while (0)
#endif

#endif  /* PHP_HOEDOWN_H */
