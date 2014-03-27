#ifdef HAVE_CONFIG_H
#    include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#include "php_hoedown.h"

/* hoedown */
#include "hoedown/src/version.h"
#include "hoedown/src/document.h"
#include "hoedown/src/html.h"
#include "hoedown/src/buffer.h"

ZEND_DECLARE_MODULE_GLOBALS(hoedown)

zend_class_entry *php_hoedown_ce;
static zend_object_handlers php_hoedown_handlers;

typedef struct {
    int renderer;
    int html;
    int extension;
    struct {
        int begin;
        int end;
        int unescape;
        zval *header;
        zval *footer;
    } toc;
    struct {
        zval *ul;
        zval *ol;
        zval *task;
    } class;
} php_hoedown_options_t;

typedef struct {
    zend_object std;
    php_hoedown_options_t options;
} php_hoedown_t;

#define HOEDOWN_METHOD(name) ZEND_METHOD(Hoedown, name)
#define HOEDOWN_ME(name, arg_info, flags) ZEND_ME(Hoedown, name, arg_info, flags)
#define HOEDOWN_MALIAS(alias, name, arg_info, flags) \
    ZEND_MALIAS(Hoedown, alias, name, arg_info, flags)
#define HOEDOWN_CONST_LONG(name, value) \
    zend_declare_class_constant_long(php_hoedown_ce, ZEND_STRS(#name)-1, \
                                     value TSRMLS_CC)

PHP_INI_BEGIN()
    STD_ZEND_INI_BOOLEAN("hoedown.disable_default_options", "Off",
                         ZEND_INI_ALL, OnUpdateBool, disable_default_options,
                         zend_hoedown_globals, hoedown_globals)
PHP_INI_END()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hoedown___construct, 0, 0, 0)
    ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hoedown_setoption, 0, 0, 2)
    ZEND_ARG_INFO(0, option)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hoedown_getoption, 0, 0, 1)
    ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hoedown_setoptions, 0, 0, 1)
    ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hoedown_parse, 0, 0, 1)
    ZEND_ARG_INFO(0, string)
    ZEND_ARG_INFO(1, state)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hoedown_parsefile, 0, 0, 1)
    ZEND_ARG_INFO(0, filename)
    ZEND_ARG_INFO(1, state)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hoedown_ofstring, 0, 0, 1)
    ZEND_ARG_INFO(0, string)
    ZEND_ARG_INFO(0, options)
    ZEND_ARG_INFO(1, state)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hoedown_offile, 0, 0, 1)
    ZEND_ARG_INFO(0, filename)
    ZEND_ARG_INFO(0, options)
    ZEND_ARG_INFO(1, state)
ZEND_END_ARG_INFO()

#define HOWDOWN_BUFFER_UNIT 64

enum {
    HOEDOWN_OPT_RENDERER_HTML = 0,
    HOEDOWN_OPT_RENDERER_TOC,
    HOEDOWN_OPT_HTML,
    HOEDOWN_OPT_HTML_SKIP_HTML = HOEDOWN_OPT_HTML,
    HOEDOWN_OPT_HTML_SKIP_STYLE,
    HOEDOWN_OPT_HTML_SKIP_IMAGES,
    HOEDOWN_OPT_HTML_SKIP_LINKS,
    HOEDOWN_OPT_HTML_EXPAND_TABS,
    HOEDOWN_OPT_HTML_SAFELINK,
    HOEDOWN_OPT_HTML_TOC,
    HOEDOWN_OPT_HTML_HARD_WRAP,
    HOEDOWN_OPT_HTML_USE_XHTML,
    HOEDOWN_OPT_HTML_ESCAPE,
#ifdef HOEDOWN_VERSION_EXTRAS
    HOEDOWN_OPT_HTML_TASK_LIST,
    HOEDOWN_OPT_HTML_LINE_CONTINUE,
#endif
    HOEDOWN_OPT_HTML_END,
    HOEDOWN_OPT_EXT,
    HOEDOWN_OPT_EXT_TABLES = HOEDOWN_OPT_EXT,
    HOEDOWN_OPT_EXT_FENCED_CODE,
    HOEDOWN_OPT_EXT_FOOTNOTES,
    HOEDOWN_OPT_EXT_AUTOLINK,
    HOEDOWN_OPT_EXT_STRIKETHROUGH,
    HOEDOWN_OPT_EXT_UNDERLINE,
    HOEDOWN_OPT_EXT_HIGHLIGHT,
    HOEDOWN_OPT_EXT_QUOTE,
    HOEDOWN_OPT_EXT_SUPERSCRIPT,
    HOEDOWN_OPT_EXT_LAX_SPACING,
    HOEDOWN_OPT_EXT_NO_INTRA_EMPHASIS,
    HOEDOWN_OPT_EXT_SPACE_HEADERS,
    HOEDOWN_OPT_EXT_DISABLE_INDENTED_CODE,
#ifdef HOEDOWN_VERSION_EXTRAS
    HOEDOWN_OPT_EXT_SPECIAL_ATTRIBUTE,
#endif
    HOEDOWN_OPT_EXT_END,
    HOEDOWN_OPT_TOC_BEGIN,
    HOEDOWN_OPT_TOC_END,
#ifdef HOEDOWN_VERSION_EXTRAS
    HOEDOWN_OPT_TOC_ESCAPE,
    HOEDOWN_OPT_TOC_HEADER,
    HOEDOWN_OPT_TOC_FOOTER,
    HOEDOWN_OPT_CLASS_LIST,
    HOEDOWN_OPT_CLASS_ORDER_LIST,
    HOEDOWN_OPT_CLASS_TASK_LIST,
#endif
};

#define HOEDOWN_DEFAULT_OPT_EXTENSION \
    HOEDOWN_EXT_TABLES | HOEDOWN_EXT_FENCED_CODE | HOEDOWN_EXT_AUTOLINK | \
    HOEDOWN_EXT_STRIKETHROUGH | HOEDOWN_EXT_NO_INTRA_EMPHASIS
#define HOEDOWN_DEFAULT_OPT_TOC_LEVEL 6

#define HOEDOWN_OBJ(self, obj) \
    self = (php_hoedown_t *)zend_object_store_get_object(obj TSRMLS_CC)

static int
php_hoedown_set_option(php_hoedown_options_t *options,
                       int opt, zval *val TSRMLS_DC)
{
    if (opt >= HOEDOWN_OPT_HTML && opt < HOEDOWN_OPT_HTML_END) {
        convert_to_boolean(val);
        if (Z_BVAL_P(val)) {
            options->html |= (1 << (opt - HOEDOWN_OPT_HTML));
        } else {
            options->html ^= (1 << (opt - HOEDOWN_OPT_HTML));
        }
        return 0;
    } else if (opt >= HOEDOWN_OPT_EXT && opt < HOEDOWN_OPT_EXT_END) {
        convert_to_boolean(val);
        if (Z_BVAL_P(val)) {
            options->extension |= (1 << (opt - HOEDOWN_OPT_EXT));
        } else {
            options->extension ^= (1 << (opt - HOEDOWN_OPT_EXT));
        }
        return 0;
    }

    switch (opt) {
        case HOEDOWN_OPT_RENDERER_HTML:
            convert_to_boolean(val);
            if (Z_BVAL_P(val)) {
                options->renderer = opt;
            }
            return 0;
        case HOEDOWN_OPT_RENDERER_TOC:
            convert_to_boolean(val);
            if (Z_BVAL_P(val)) {
                options->renderer = opt;
            } else {
                options->renderer = HOEDOWN_OPT_RENDERER_HTML;
            }
            return 0;
        case HOEDOWN_OPT_TOC_BEGIN:
            convert_to_long(val);
            options->toc.begin = Z_LVAL_P(val);
            return 0;
        case HOEDOWN_OPT_TOC_END:
            convert_to_long(val);
            options->toc.end = Z_LVAL_P(val);
            return 0;
#ifdef HOEDOWN_VERSION_EXTRAS
        case HOEDOWN_OPT_TOC_ESCAPE:
            convert_to_boolean(val);
            if (Z_BVAL_P(val)) {
                options->toc.unescape = 0;
            } else {
                options->toc.unescape = 1;
            }
            return 0;
        case HOEDOWN_OPT_TOC_HEADER:
            convert_to_string(val);
            if (Z_STRLEN_P(val) <= 0) {
                if (options->toc.header) {
                    zval_ptr_dtor(&options->toc.header);
                    options->toc.header = NULL;
                }
            } else {
                if (!options->toc.header) {
                    MAKE_STD_ZVAL(options->toc.header);
                }
                ZVAL_ZVAL(options->toc.header, val, 1, 0);
            }
            return 0;
        case HOEDOWN_OPT_TOC_FOOTER:
            convert_to_string(val);
            if (Z_STRLEN_P(val) <= 0) {
                if (options->toc.footer) {
                    zval_ptr_dtor(&options->toc.footer);
                    options->toc.footer = NULL;
                }
            } else {
                if (!options->toc.footer) {
                    MAKE_STD_ZVAL(options->toc.footer);
                }
                ZVAL_ZVAL(options->toc.footer, val, 1, 0);
            }
            return 0;
        case HOEDOWN_OPT_CLASS_LIST:
            convert_to_string(val);
            if (Z_STRLEN_P(val) <= 0) {
                if (options->class.ul) {
                    zval_ptr_dtor(&options->class.ul);
                    options->class.ul = NULL;
                }
            } else {
                if (!options->class.ul) {
                    MAKE_STD_ZVAL(options->class.ul);
                }
                ZVAL_ZVAL(options->class.ul, val, 1, 0);
            }
            return 0;
        case HOEDOWN_OPT_CLASS_ORDER_LIST:
            convert_to_string(val);
            if (Z_STRLEN_P(val) <= 0) {
                if (options->class.ol) {
                    zval_ptr_dtor(&options->class.ol);
                    options->class.ol = NULL;
                }
            } else {
                if (!options->class.ol) {
                    MAKE_STD_ZVAL(options->class.ol);
                }
                ZVAL_ZVAL(options->class.ol, val, 1, 0);
            }
            return 0;
        case HOEDOWN_OPT_CLASS_TASK_LIST:
            convert_to_string(val);
            if (Z_STRLEN_P(val) <= 0) {
                if (options->class.task) {
                    zval_ptr_dtor(&options->class.task);
                    options->class.task = NULL;
                }
            } else {
                if (!options->class.task) {
                    MAKE_STD_ZVAL(options->class.task);
                }
                ZVAL_ZVAL(options->class.task, val, 1, 0);
            }
            return 0;
#endif
        default:
            break;
    }

    return 1;
}

static void
php_hoedown_set_options(php_hoedown_options_t *options, zval *val TSRMLS_DC)
{
    zval **value;
    uint key_len;
    char *key;
    ulong key_index;

    for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(val));
         zend_hash_get_current_data(Z_ARRVAL_P(val), (void *)&value) == SUCCESS;
         zend_hash_move_forward(Z_ARRVAL_P(val))) {
        if (zend_hash_get_current_key_ex(Z_ARRVAL_P(val),
                                         &key, &key_len, &key_index,
                                         0, NULL) == HASH_KEY_IS_LONG) {
            php_hoedown_set_option(options, key_index, *value TSRMLS_CC);
        } else {
            HOEDOWN_ERR(E_WARNING, "Invalid configuration options");
        }
    }
}

static void
php_hoedown_options_init(php_hoedown_options_t *options TSRMLS_DC)
{
    memset(options, 0, sizeof(php_hoedown_options_t));

    options->renderer = HOEDOWN_OPT_RENDERER_HTML;
    options->toc.end = HOEDOWN_DEFAULT_OPT_TOC_LEVEL;

    if (!HOEDOWN_G(disable_default_options)) {
        options->extension = HOEDOWN_DEFAULT_OPT_EXTENSION;
    }
}

static void
php_hoedown_options_destroy(php_hoedown_options_t *options TSRMLS_DC)
{
    if (options->toc.header) {
        zval_ptr_dtor(&options->toc.header);
    }
    if (options->toc.footer) {
        zval_ptr_dtor(&options->toc.footer);
    }

    if (options->class.ul) {
        zval_ptr_dtor(&options->class.ul);
    }
    if (options->class.ol) {
        zval_ptr_dtor(&options->class.ol);
    }
    if (options->class.task) {
        zval_ptr_dtor(&options->class.task);
    }
}

HOEDOWN_METHOD(__construct)
{
    zval *options = NULL;
    php_hoedown_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|a",
                              &options) == FAILURE) {
        RETURN_FALSE;
    }

    HOEDOWN_OBJ(intern, getThis());

    if (options) {
        php_hoedown_set_options(&intern->options, options TSRMLS_CC);
    }
}

HOEDOWN_METHOD(setOption)
{
    long option;
    zval *value;
    php_hoedown_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lz",
                              &option, &value) == FAILURE) {
        RETURN_FALSE;
    }

    HOEDOWN_OBJ(intern, getThis());

    if (php_hoedown_set_option(&intern->options, option, value TSRMLS_CC) != 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

HOEDOWN_METHOD(getOption)
{
    long option;
    php_hoedown_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",
                              &option) == FAILURE) {
        RETURN_FALSE;
    }

    HOEDOWN_OBJ(intern, getThis());

    if (option >= HOEDOWN_OPT_HTML && option < HOEDOWN_OPT_HTML_END) {
        if (intern->options.html & (1 << (option - HOEDOWN_OPT_HTML))) {
            RETURN_TRUE;
        }
        RETURN_FALSE;
    } else if (option >= HOEDOWN_OPT_EXT && option < HOEDOWN_OPT_EXT_END) {
        if (intern->options.extension & (1 << (option - HOEDOWN_OPT_EXT))) {
            RETURN_TRUE;
        }
        RETURN_FALSE;
    } else {
        switch (option) {
            case HOEDOWN_OPT_TOC_BEGIN:
                RETURN_LONG(intern->options.toc.begin);
            case HOEDOWN_OPT_TOC_END:
                RETURN_LONG(intern->options.toc.end);
#ifdef HOEDOWN_VERSION_EXTRAS
            case HOEDOWN_OPT_TOC_ESCAPE:
                if (intern->options.toc.unescape) {
                    RETURN_FALSE;
                }
                RETURN_TRUE;
            case HOEDOWN_OPT_TOC_HEADER:
                if (intern->options.toc.header) {
                    RETURN_ZVAL(intern->options.toc.header, 1, 0);
                }
                RETURN_NULL();
            case HOEDOWN_OPT_TOC_FOOTER:
                if (intern->options.toc.footer) {
                    RETURN_ZVAL(intern->options.toc.footer, 1, 0);
                }
                RETURN_NULL();
            case HOEDOWN_OPT_CLASS_LIST:
                if (intern->options.class.ul) {
                    RETURN_ZVAL(intern->options.class.ul, 1, 0);
                }
                RETURN_NULL();
            case HOEDOWN_OPT_CLASS_ORDER_LIST:
                if (intern->options.class.ol) {
                    RETURN_ZVAL(intern->options.class.ol, 1, 0);
                }
                RETURN_NULL();
            case HOEDOWN_OPT_CLASS_TASK_LIST:
                if (intern->options.class.task) {
                    RETURN_ZVAL(intern->options.class.task, 1, 0);
                }
                RETURN_NULL();
#endif
            default:
                break;
        }
        RETURN_FALSE;
    }
}

HOEDOWN_METHOD(setOptions)
{
    zval *options;
    php_hoedown_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a",
                              &options) == FAILURE) {
        RETURN_FALSE;
    }

    HOEDOWN_OBJ(intern, getThis());

    php_hoedown_set_options(&intern->options, options TSRMLS_CC);
}

static int
php_hoedown_parse(zval *return_value, zval *return_state,
                  char *contents, size_t length,
                  php_hoedown_options_t *options TSRMLS_DC)
{
    hoedown_buffer *buf;
    hoedown_renderer *renderer;
    hoedown_document *document;
    hoedown_html_renderer_state *state;
    int toc_level = 0;

    if (length == 0 || !contents) {
        return 1;
    }

    /* init buffer */
    buf = hoedown_buffer_new(HOWDOWN_BUFFER_UNIT);
    if (!buf) {
        HOEDOWN_ERR(E_WARNING, "Couldn't allocate output buffer");
        return 1;
    }

    /* toc */
    if (options->html & HOEDOWN_HTML_TOC) {
        toc_level = options->toc.end;
        if (return_state || options->renderer == HOEDOWN_OPT_RENDERER_TOC) {
            renderer = hoedown_html_toc_renderer_new(0);

            state = (hoedown_html_renderer_state *)renderer->opaque;

            state->toc_data.current_level = 0;
            state->toc_data.level_offset = options->toc.begin;
            state->toc_data.nesting_level = toc_level;
#ifdef HOEDOWN_VERSION_EXTRAS
            state->toc_data.unescape = options->toc.unescape;
            if (options->toc.header) {
                state->toc_data.header = Z_STRVAL_P(options->toc.header);
            }
            if (options->toc.footer) {
                state->toc_data.footer = Z_STRVAL_P(options->toc.footer);
            }
#endif
            document = hoedown_document_new(renderer, options->extension, 16);

            hoedown_document_render(document, buf, contents, length);

            hoedown_document_free(document);
            hoedown_html_renderer_free(renderer);

            if (options->renderer == HOEDOWN_OPT_RENDERER_TOC) {
                RETVAL_STRINGL(buf->data, buf->size, 1);
                hoedown_buffer_free(buf);
                return 0;
            } else {
                zval *zv;

                MAKE_STD_ZVAL(zv);
                ZVAL_STRINGL(zv, buf->data, buf->size, 1);

                if (Z_TYPE_P(return_state) != IS_ARRAY) {
                    array_init(return_state);
                }

                zend_symtable_update(Z_ARRVAL_P(return_state),
                                     "toc", sizeof("toc"),
                                     &zv, sizeof(zv), NULL);

                hoedown_buffer_reset(buf);
            }
        }
    }

    /* init renderer */
    renderer = hoedown_html_renderer_new(options->html, toc_level);

    /* setting state */
    state = (hoedown_html_renderer_state *)renderer->opaque;

    if (options->class.ul) {
        state->class_data.ul = Z_STRVAL_P(options->class.ul);
    }
    if (options->class.ol) {
        state->class_data.ol = Z_STRVAL_P(options->class.ol);
    }
    if (options->class.task) {
        state->class_data.task = Z_STRVAL_P(options->class.task);
    }

    /* init document */
    document = hoedown_document_new(renderer, options->extension, 16);

    /* execute parse */
    hoedown_document_render(document, buf, contents, length);

    /* cleanup */
    hoedown_document_free(document);
    hoedown_html_renderer_free(renderer);

    /* setting return value */
    RETVAL_STRINGL(buf->data, buf->size, 1);

    /* cleanup buffer */
    hoedown_buffer_free(buf);

    return 0;
}

HOEDOWN_METHOD(parse)
{
    char *buf;
    int buf_len;
    zval *state = NULL;
    php_hoedown_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|z",
                              &buf, &buf_len, &state) == FAILURE) {
        RETURN_FALSE;
    }

    if (buf_len == 0) {
        RETURN_FALSE;
    }

    HOEDOWN_OBJ(intern, getThis());

    /* markdown parse */
    if (php_hoedown_parse(return_value, state, buf, buf_len,
                          &intern->options TSRMLS_CC) != 0) {
        RETURN_FALSE;
    }
}

HOEDOWN_METHOD(parseFile)
{
    char *filename;
    int filename_len;
    zval *state = NULL;
    int len;
    long maxlen = PHP_STREAM_COPY_ALL;
    char *contents = NULL;
    php_stream *stream;
    php_hoedown_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|z",
                              &filename, &filename_len, &state) == FAILURE) {
        RETURN_FALSE;
    }

    if (filename_len == 0) {
        RETURN_FALSE;
    }

    HOEDOWN_OBJ(intern, getThis());

    /* read file */
    stream = php_stream_open_wrapper_ex(filename, "rb",
                                        REPORT_ERRORS, NULL, NULL);
    if (!stream) {
        RETURN_FALSE;
    }

    if ((len = php_stream_copy_to_mem(stream, &contents, maxlen, 0)) > 0) {
        /* markdown parse */
        if (php_hoedown_parse(return_value, state, contents, len,
                              &intern->options TSRMLS_CC) != 0) {
            RETVAL_FALSE;
        }
    } else if (len == 0) {
        RETVAL_EMPTY_STRING();
    } else {
        RETVAL_FALSE;
    }

    if (contents) {
        efree(contents);
    }

    php_stream_close(stream);
}

HOEDOWN_METHOD(ofString)
{
    char *buf;
    int buf_len;
    zval *opts = NULL, *state = NULL;
    php_hoedown_t *intern;
    php_hoedown_options_t options;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|az",
                              &buf, &buf_len, &opts, &state) == FAILURE) {
        RETURN_FALSE;
    }

    if (buf_len == 0) {
        RETURN_FALSE;
    }

    /* setting options */
    php_hoedown_options_init(&options TSRMLS_CC);
    if (opts) {
        php_hoedown_set_options(&options, opts TSRMLS_CC);
    }

    /* markdown parse */
    if (php_hoedown_parse(return_value, state, buf, buf_len,
                          &options TSRMLS_CC) != 0) {
        RETVAL_FALSE;
    }

    php_hoedown_options_destroy(&options TSRMLS_CC);
}

HOEDOWN_METHOD(ofFile)
{
    char *filename;
    int filename_len, len;
    long maxlen = PHP_STREAM_COPY_ALL;
    char *contents = NULL;
    php_stream *stream;
    zval *opts = NULL, *state = NULL;
    php_hoedown_t *intern;
    php_hoedown_options_t options;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|az",
                              &filename, &filename_len,
                              &opts, &state) == FAILURE) {
        RETURN_FALSE;
    }

    if (filename_len == 0) {
        RETURN_FALSE;
    }

    /* setting options */
    php_hoedown_options_init(&options TSRMLS_CC);
    if (opts) {
        php_hoedown_set_options(&options, opts TSRMLS_CC);
    }

    /* read file */
    stream = php_stream_open_wrapper_ex(filename, "rb",
                                        REPORT_ERRORS, NULL, NULL);
    if (!stream) {
        php_hoedown_options_destroy(&options TSRMLS_CC);
        RETURN_FALSE;
    }

    if ((len = php_stream_copy_to_mem(stream, &contents, maxlen, 0)) > 0) {
        /* markdown parse */
        if (php_hoedown_parse(return_value, state, contents, len,
                              &options TSRMLS_CC) != 0) {
            RETVAL_FALSE;
        }
    } else if (len == 0) {
        RETVAL_EMPTY_STRING();
    } else {
        RETVAL_FALSE;
    }

    if (contents) {
        efree(contents);
    }

    php_stream_close(stream);
    php_hoedown_options_destroy(&options TSRMLS_CC);
}

static zend_function_entry php_hoedown_methods[] = {
    HOEDOWN_ME(__construct, arginfo_hoedown___construct,
               ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    HOEDOWN_ME(setOption, arginfo_hoedown_setoption, ZEND_ACC_PUBLIC)
    HOEDOWN_ME(getOption, arginfo_hoedown_getoption, ZEND_ACC_PUBLIC)
    HOEDOWN_ME(setOptions, arginfo_hoedown_setoptions, ZEND_ACC_PUBLIC)
    HOEDOWN_ME(parse, arginfo_hoedown_parse, ZEND_ACC_PUBLIC)
    HOEDOWN_MALIAS(parseString, parse, arginfo_hoedown_parse, ZEND_ACC_PUBLIC)
    HOEDOWN_ME(parseFile, arginfo_hoedown_parsefile, ZEND_ACC_PUBLIC)
    HOEDOWN_ME(ofString, arginfo_hoedown_ofstring,
               ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    HOEDOWN_ME(ofFile, arginfo_hoedown_offile,
               ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_FE_END
};

static void
php_hoedown_free_storage(void *object TSRMLS_DC)
{
    php_hoedown_t *intern = (php_hoedown_t *)object;

    if (!intern) {
        return;
    }

    php_hoedown_options_destroy(&intern->options TSRMLS_CC);

    zend_object_std_dtor(&intern->std TSRMLS_CC);
    efree(object);
}

static zend_object_value
php_hoedown_new_ex(zend_class_entry *ce, php_hoedown_t **ptr TSRMLS_DC)
{
    php_hoedown_t *intern;
    zend_object_value retval;

    intern = (php_hoedown_t *)emalloc(sizeof(php_hoedown_t));
    memset(intern, 0, sizeof(php_hoedown_t));
    if (ptr) {
        *ptr = intern;
    }

    zend_object_std_init(&intern->std, ce TSRMLS_CC);
    object_properties_init(&intern->std, ce);

    retval.handle = zend_objects_store_put(
        intern, (zend_objects_store_dtor_t)zend_objects_destroy_object,
        (zend_objects_free_object_storage_t)php_hoedown_free_storage,
        NULL TSRMLS_CC);
    retval.handlers = &php_hoedown_handlers;

    php_hoedown_options_init(&intern->options TSRMLS_CC);

    return retval;
}

static zend_object_value
php_hoedown_new(zend_class_entry *ce TSRMLS_DC)
{
    return php_hoedown_new_ex(ce, NULL TSRMLS_CC);
}

static void
hoedown_init_globals(zend_hoedown_globals *hoedown_globals)
{
    hoedown_globals->disable_default_options = 0;
}

ZEND_MINIT_FUNCTION(hoedown)
{
    zend_class_entry ce;

    /* class register */
    INIT_CLASS_ENTRY(ce, "Hoedown", php_hoedown_methods);

    ce.create_object = php_hoedown_new;

    php_hoedown_ce = zend_register_internal_class(&ce TSRMLS_CC);
    if (php_hoedown_ce == NULL) {
        return FAILURE;
    }

    memcpy(&php_hoedown_handlers, zend_get_std_object_handlers(),
           sizeof(zend_object_handlers));

    php_hoedown_handlers.clone_obj = NULL;

    /* class constant */
    HOEDOWN_CONST_LONG(RENDERER_HTML, HOEDOWN_OPT_RENDERER_HTML);
    HOEDOWN_CONST_LONG(RENDERER_TOC, HOEDOWN_OPT_RENDERER_TOC);

    HOEDOWN_CONST_LONG(SKIP_HTML, HOEDOWN_OPT_HTML_SKIP_HTML);
    HOEDOWN_CONST_LONG(SKIP_STYLE, HOEDOWN_OPT_HTML_SKIP_STYLE);
    HOEDOWN_CONST_LONG(SKIP_IMAGES, HOEDOWN_OPT_HTML_SKIP_IMAGES);
    HOEDOWN_CONST_LONG(SKIP_LINKS, HOEDOWN_OPT_HTML_SKIP_LINKS);
    HOEDOWN_CONST_LONG(EXPAND_TABS, HOEDOWN_OPT_HTML_EXPAND_TABS);
    HOEDOWN_CONST_LONG(SAFELINK, HOEDOWN_OPT_HTML_SAFELINK);
    HOEDOWN_CONST_LONG(TOC, HOEDOWN_OPT_HTML_TOC);
    HOEDOWN_CONST_LONG(HARD_WRAP, HOEDOWN_OPT_HTML_HARD_WRAP);
    HOEDOWN_CONST_LONG(USE_XHTML, HOEDOWN_OPT_HTML_USE_XHTML);
    HOEDOWN_CONST_LONG(ESCAPE, HOEDOWN_OPT_HTML_ESCAPE);
#ifdef HOEDOWN_VERSION_EXTRAS
    HOEDOWN_CONST_LONG(TASK_LIST, HOEDOWN_OPT_HTML_TASK_LIST);
    HOEDOWN_CONST_LONG(LINE_CONTINUE, HOEDOWN_OPT_HTML_LINE_CONTINUE);
#endif

    HOEDOWN_CONST_LONG(TABLES, HOEDOWN_OPT_EXT_TABLES);
    HOEDOWN_CONST_LONG(FENCED_CODE, HOEDOWN_OPT_EXT_FENCED_CODE);
    HOEDOWN_CONST_LONG(FOOTNOTES, HOEDOWN_OPT_EXT_FOOTNOTES);
    HOEDOWN_CONST_LONG(AUTOLINK, HOEDOWN_OPT_EXT_AUTOLINK);
    HOEDOWN_CONST_LONG(STRIKETHROUGH, HOEDOWN_OPT_EXT_STRIKETHROUGH);
    HOEDOWN_CONST_LONG(UNDERLINE, HOEDOWN_OPT_EXT_UNDERLINE);
    HOEDOWN_CONST_LONG(HIGHLIGHT, HOEDOWN_OPT_EXT_HIGHLIGHT);
    HOEDOWN_CONST_LONG(QUOTE, HOEDOWN_OPT_EXT_QUOTE);
    HOEDOWN_CONST_LONG(SUPERSCRIPT, HOEDOWN_OPT_EXT_SUPERSCRIPT);
    HOEDOWN_CONST_LONG(LAX_SPACING, HOEDOWN_OPT_EXT_LAX_SPACING);
    HOEDOWN_CONST_LONG(NO_INTRA_EMPHASIS, HOEDOWN_OPT_EXT_NO_INTRA_EMPHASIS);
    HOEDOWN_CONST_LONG(SPACE_HEADERS, HOEDOWN_OPT_EXT_SPACE_HEADERS);
    HOEDOWN_CONST_LONG(DISABLE_INDENTED_CODE,
                       HOEDOWN_OPT_EXT_DISABLE_INDENTED_CODE);
#ifdef HOEDOWN_VERSION_EXTRAS
    HOEDOWN_CONST_LONG(SPECIAL_ATTRIBUTE, HOEDOWN_OPT_EXT_SPECIAL_ATTRIBUTE);
#endif

    HOEDOWN_CONST_LONG(TOC_BEGIN, HOEDOWN_OPT_TOC_BEGIN);
    HOEDOWN_CONST_LONG(TOC_END, HOEDOWN_OPT_TOC_END);
#ifdef HOEDOWN_VERSION_EXTRAS
    HOEDOWN_CONST_LONG(TOC_ESCAPE, HOEDOWN_OPT_TOC_ESCAPE);
    HOEDOWN_CONST_LONG(TOC_HEADER, HOEDOWN_OPT_TOC_HEADER);
    HOEDOWN_CONST_LONG(TOC_FOOTER, HOEDOWN_OPT_TOC_FOOTER);
#endif

#ifdef HOEDOWN_VERSION_EXTRAS
    HOEDOWN_CONST_LONG(CLASS_LIST, HOEDOWN_OPT_CLASS_LIST);
    HOEDOWN_CONST_LONG(CLASS_ORDER_LIST, HOEDOWN_OPT_CLASS_ORDER_LIST);
    HOEDOWN_CONST_LONG(CLASS_TASK_LIST, HOEDOWN_OPT_CLASS_TASK_LIST);
#endif

    /* ini */
    ZEND_INIT_MODULE_GLOBALS(hoedown, hoedown_init_globals, NULL);
    REGISTER_INI_ENTRIES();

    return SUCCESS;
}

ZEND_MSHUTDOWN_FUNCTION(hoedown)
{
    UNREGISTER_INI_ENTRIES();
    return SUCCESS;
}

ZEND_MINFO_FUNCTION(hoedown)
{
    php_info_print_table_start();
    php_info_print_table_row(2, "Hoedown support", "enabled");
    php_info_print_table_row(2, "Extension Version", HOEDOWN_EXT_VERSION);
    php_info_print_table_row(2, "library version", HOEDOWN_VERSION);
#ifdef HOEDOWN_VERSION_EXTRAS
    php_info_print_table_row(2, "library extras", "enabled");
#endif
    php_info_print_table_end();

    DISPLAY_INI_ENTRIES();
}

zend_module_entry hoedown_module_entry = {
    STANDARD_MODULE_HEADER,
    "hoedown",
    NULL,
    ZEND_MINIT(hoedown),
    ZEND_MSHUTDOWN(hoedown),
    NULL,
    NULL,
    ZEND_MINFO(hoedown),
    HOEDOWN_EXT_VERSION,
    STANDARD_MODULE_PROPERTIES
};

#if COMPILE_DL_HOEDOWN
ZEND_GET_MODULE(hoedown)
#endif
