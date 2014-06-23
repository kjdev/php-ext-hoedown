#ifdef HAVE_CONFIG_H
#    include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_array.h"
#include "Zend/zend_exceptions.h"

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
    zval *renders;
    zval *is_user;
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

#define HOEDOWN_DEFAULT_OPTIONS \
    "tables,fenced-code,autolink,strikethrough,no-intra-emphasis"

ZEND_INI_BEGIN()
    STD_ZEND_INI_ENTRY("hoedown.options", (char *)HOEDOWN_DEFAULT_OPTIONS,
                       ZEND_INI_ALL, OnUpdateString, options,
                       zend_hoedown_globals, hoedown_globals)
ZEND_INI_END()

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

ZEND_BEGIN_ARG_INFO_EX(arginfo_hoedown_addrender, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, definition)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hoedown_getrenders, 0, 0, 0)
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

#define HOEDOWN_BUFFER_UNIT 64

enum {
    HOEDOWN_OPT_RENDERER_HTML = 0,
    HOEDOWN_OPT_RENDERER_TOC,
    HOEDOWN_OPT_HTML,
    HOEDOWN_OPT_HTML_SKIP_HTML = HOEDOWN_OPT_HTML,
    HOEDOWN_OPT_HTML_ESCAPE,
    HOEDOWN_OPT_HTML_EXPAND_TABS,
    HOEDOWN_OPT_HTML_SAFELINK,
    HOEDOWN_OPT_HTML_HARD_WRAP,
    HOEDOWN_OPT_HTML_USE_XHTML,
    HOEDOWN_OPT_HTML_TASK_LIST,
    HOEDOWN_OPT_HTML_LINE_CONTINUE,
    HOEDOWN_OPT_HTML_HEADER_ID,
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
    HOEDOWN_OPT_EXT_SPECIAL_ATTRIBUTE,
    HOEDOWN_OPT_EXT_END,
    HOEDOWN_OPT_TOC,
    HOEDOWN_OPT_TOC_BEGIN,
    HOEDOWN_OPT_TOC_END,
    HOEDOWN_OPT_TOC_ESCAPE,
    HOEDOWN_OPT_TOC_HEADER,
    HOEDOWN_OPT_TOC_FOOTER,
    HOEDOWN_OPT_RENDERS,
    HOEDOWN_OPT_IS_USER_BLOCK,
};

#define HOEDOWN_DEFAULT_OPT_TOC_LEVEL 6

#define HOEDOWN_OBJ(self, obj) \
    self = (php_hoedown_t *)zend_object_store_get_object(obj TSRMLS_CC)

static int
php_hoedown_set_option(php_hoedown_options_t *options,
                       int opt, zval *val TSRMLS_DC)
{
    if (opt >= HOEDOWN_OPT_HTML && opt < HOEDOWN_OPT_HTML_END) {
        int n = (1 << (opt - HOEDOWN_OPT_HTML));
        convert_to_boolean(val);
        if (Z_BVAL_P(val)) {
            options->html |= n;
        } else if (options->html & n) {
            options->html ^= n;
        }
        return 0;
    } else if (opt >= HOEDOWN_OPT_EXT && opt < HOEDOWN_OPT_EXT_END) {
        int n = (1 << (opt - HOEDOWN_OPT_EXT));
        convert_to_boolean(val);
        if (Z_BVAL_P(val)) {
            options->extension |= n;
        } else if (options->extension & n) {
            options->extension ^= n;
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
        case HOEDOWN_OPT_TOC:
            convert_to_boolean(val);
            if (Z_BVAL_P(val)) {
                options->toc.end = HOEDOWN_DEFAULT_OPT_TOC_LEVEL;
            } else {
                options->toc.end = 0;
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
        case HOEDOWN_OPT_RENDERS:
            if (!options->renders) {
                MAKE_STD_ZVAL(options->renders);
                array_init(options->renders);
            }
            if (Z_TYPE_P(val) == IS_ARRAY) {
                php_array_merge(Z_ARRVAL_P(options->renders), Z_ARRVAL_P(val),
                                1 TSRMLS_CC);
                return 0;
            }
            break;
        case HOEDOWN_OPT_IS_USER_BLOCK:
            if (Z_TYPE_P(val) == IS_NULL) {
                if (options->is_user) {
                    zval_ptr_dtor(&options->is_user);
                    options->is_user = NULL;
                }
            } else {
                if (!options->is_user) {
                    MAKE_STD_ZVAL(options->is_user);
                }
                ZVAL_ZVAL(options->is_user, val, 1, 0);
            }
            return 0;
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

typedef struct {
    unsigned int option;
    const char *name;
} php_hoedown_flag_t;

static void
php_hoedown_set_options_flag(php_hoedown_options_t *options,
                             char *name, size_t size)
{
    php_hoedown_flag_t extension_flags[] = {
        { HOEDOWN_EXT_TABLES, "tables" },
        { HOEDOWN_EXT_FENCED_CODE, "fenced-code" },
        { HOEDOWN_EXT_AUTOLINK, "autolink" },
        { HOEDOWN_EXT_STRIKETHROUGH, "strikethrough" },
        { HOEDOWN_EXT_NO_INTRA_EMPHASIS, "no-intra-emphasis" },
        { HOEDOWN_EXT_FOOTNOTES, "footnotes" },
        { HOEDOWN_EXT_UNDERLINE, "underline" },
        { HOEDOWN_EXT_HIGHLIGHT, "highlight" },
        { HOEDOWN_EXT_QUOTE, "quote" },
        { HOEDOWN_EXT_SUPERSCRIPT, "superscript" },
        /* { HOEDOWN_EXT_LAX_SPACING, "lax-spacing" }, */
        { HOEDOWN_EXT_SPACE_HEADERS, "space-headers" },
        { HOEDOWN_EXT_DISABLE_INDENTED_CODE, "disable-indented-code" },
        { HOEDOWN_EXT_SPECIAL_ATTRIBUTE, "special-attribute" },
    };
    php_hoedown_flag_t html_flags[] = {
        { HOEDOWN_HTML_SKIP_HTML, "skip-html" },
        /* { HOEDOWN_HTML_EXPAND_TABS, "expand-tabs" }, */
        { HOEDOWN_HTML_SAFELINK, "safelink" },
        { HOEDOWN_HTML_HARD_WRAP, "hard-wrap" },
        { HOEDOWN_HTML_USE_XHTML, "xhtml" },
        { HOEDOWN_HTML_ESCAPE, "escape" },
        { HOEDOWN_HTML_USE_TASK_LIST, "task" },
        { HOEDOWN_HTML_LINE_CONTINUE, "line-continue" },
        { HOEDOWN_HTML_HEADER_ID, "header-id" },
    };
    php_hoedown_flag_t *flag;
    int n, i = 0;

    /* extension */
    i = 0;
    n = sizeof(extension_flags)/sizeof(php_hoedown_flag_t);
    do {
        flag = extension_flags + i;
        if (strncasecmp(name, flag->name, size) == 0) {
            options->extension |= flag->option;
            return;
        }
        ++i;
    } while (i < n);

    /* html */
    i = 0;
    n = sizeof(html_flags)/sizeof(php_hoedown_flag_t);
    do {
        flag = html_flags + i;
        if (strncasecmp(name, flag->name, size) == 0) {
            options->html |= flag->option;
            return;
        }
        ++i;
    } while (i < n);
}

static void
php_hoedown_options_init(php_hoedown_options_t *options TSRMLS_DC)
{
    char *opts, *p1, *p2, *endp;

    memset(options, 0, sizeof(php_hoedown_options_t));

    options->renderer = HOEDOWN_OPT_RENDERER_HTML;
    options->toc.end = 0;
    options->extension = 0;
    options->html = 0;

    opts = HOEDOWN_G(options);
    if (!opts || strlen(opts) == 0) {
        return;
    }

    endp = opts + strlen(opts);
    p1 = opts;
    p2 = php_memnstr(opts, ",", 1, endp);

    if (p2 == NULL) {
        php_hoedown_set_options_flag(options, p1, strlen(opts));
    } else {
        do {
            php_hoedown_set_options_flag(options, p1, p2 - p1);
            p1 = p2 + 1;
        } while ((p2 = php_memnstr(p1, ",", 1, endp)) != NULL);
        if (p1 <= endp) {
            php_hoedown_set_options_flag(options, p1, endp - p1);
        }
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
    if (options->renders) {
        zval_ptr_dtor(&options->renders);
    }
    if (options->is_user) {
        zval_ptr_dtor(&options->is_user);
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
            case HOEDOWN_OPT_TOC:
                if (intern->options.toc.end > 0) {
                    RETURN_TRUE;
                }
                RETURN_FALSE;
            case HOEDOWN_OPT_TOC_BEGIN:
                RETURN_LONG(intern->options.toc.begin);
            case HOEDOWN_OPT_TOC_END:
                RETURN_LONG(intern->options.toc.end);
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

static zval *
php_hoedown_is_renderer(char *name, void *opaque TSRMLS_DC)
{
    hoedown_html_renderer_state *state = opaque;
    zval *renders = NULL;
    zval **definition = NULL;

    if (state) {
        renders = ((php_hoedown_options_t *)state->opaque)->renders;
    }

    if (!name || !renders || Z_TYPE_P(renders) != IS_ARRAY) {
        HOEDOWN_ERR(E_WARNING, "Undefined renders");
        return NULL;
    }

    if (zend_hash_find(Z_ARRVAL_P(renders), name, strlen(name)+1,
                       (void **)&definition) == FAILURE || !definition) {
        HOEDOWN_ERR(E_WARNING, "Undefined render '%s'", name);
        return NULL;
    }

    return *definition;
}

static void
php_hoedown_run_renderer(hoedown_buffer *ob, zval *definition,
                         zval *parameters TSRMLS_DC)
{
    zval fname;
    zval *result = NULL, **args[2];

    ZVAL_STRINGL(&fname, "call_user_func_array",
                 sizeof("call_user_func_array")-1, 0);

    args[0] = &definition;
    args[1] = &parameters;

    call_user_function_ex(EG(function_table), NULL, &fname,
                          &result, 2, args, 0, NULL TSRMLS_CC);
    if (result) {
        if (Z_TYPE_P(result) == IS_STRING) {
            hoedown_buffer_put(ob, Z_STRVAL_P(result), Z_STRLEN_P(result));
        }
        zval_ptr_dtor(&result);
    }
}

static void
php_hoedown_renderer_blockcode(hoedown_buffer *ob, const hoedown_buffer *text,
                               const hoedown_buffer *lang,
                               const hoedown_buffer *attr, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("blockcode", opaque TSRMLS_CC);
    if (!definition) {
        return;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 3);
    if (text) {
        add_next_index_stringl(parameters, (char *)text->data, text->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }
    if (lang) {
        add_next_index_stringl(parameters, (char *)lang->data, lang->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }
    if (attr) {
        add_next_index_stringl(parameters, (char *)attr->data, attr->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);
}

static void
php_hoedown_renderer_blockquote(hoedown_buffer *ob,
                                const hoedown_buffer *text, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("blockquote", opaque TSRMLS_CC);
    if (!definition) {
        return;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 1);
    if (text) {
        add_next_index_stringl(parameters, (char *)text->data, text->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);
}

static void
php_hoedown_renderer_blockhtml(hoedown_buffer *ob,
                               const hoedown_buffer *text, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("blockhtml", opaque TSRMLS_CC);
    if (!definition) {
        return;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 1);
    if (text) {
        add_next_index_stringl(parameters, (char *)text->data, text->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);
}

static void
php_hoedown_renderer_header(hoedown_buffer *ob, const hoedown_buffer *text,
                            const hoedown_buffer *attr, int level, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("header", opaque TSRMLS_CC);
    if (!definition) {
        return;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 3);
    if (text) {
        add_next_index_stringl(parameters, (char *)text->data, text->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }
    if (attr) {
        add_next_index_stringl(parameters, (char *)attr->data, attr->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }
    add_next_index_long(parameters, level);

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);
}

static void
php_hoedown_renderer_hrule(hoedown_buffer *ob, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("hrule", opaque TSRMLS_CC);
    if (!definition) {
        return;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 0);

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);
}

static void
php_hoedown_renderer_list(hoedown_buffer *ob, const hoedown_buffer *text,
                          const hoedown_buffer *attr, unsigned int flags,
                          void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("list", opaque TSRMLS_CC);
    if (!definition) {
        return;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 3);
    if (text) {
        add_next_index_stringl(parameters, (char *)text->data, text->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }
    if (attr) {
        add_next_index_stringl(parameters, (char *)attr->data, attr->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }
    add_next_index_long(parameters, flags);

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);
}

static void
php_hoedown_renderer_listitem(hoedown_buffer *ob,
                              const hoedown_buffer *text,
                              const hoedown_buffer *attr,
                              unsigned int *flags, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("listitem", opaque TSRMLS_CC);
    if (!definition) {
        return;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 3);
    if (text) {
        add_next_index_stringl(parameters, (char *)text->data, text->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }
    if (attr) {
        add_next_index_stringl(parameters, (char *)attr->data, attr->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }
    add_next_index_long(parameters, *flags);

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);
}

static void
php_hoedown_renderer_paragraph(hoedown_buffer *ob,
                               const hoedown_buffer *text, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("paragraph", opaque TSRMLS_CC);
    if (!definition) {
        return;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 1);
    if (text) {
        add_next_index_stringl(parameters, (char *)text->data, text->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);
}

static void
php_hoedown_renderer_table(hoedown_buffer *ob,
                           const hoedown_buffer *header,
                           const hoedown_buffer *body,
                           const hoedown_buffer *attr, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("table", opaque TSRMLS_CC);
    if (!definition) {
        return;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 3);
    if (header) {
        add_next_index_stringl(parameters,
                               (char *)header->data, header->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }
    if (body) {
        add_next_index_stringl(parameters, (char *)body->data, body->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }
    if (attr) {
        add_next_index_stringl(parameters, (char *)attr->data, attr->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);
}

static void
php_hoedown_renderer_table_row(hoedown_buffer *ob,
                               const hoedown_buffer *text, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("tablerow", opaque TSRMLS_CC);
    if (!definition) {
        return;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 1);
    if (text) {
        add_next_index_stringl(parameters, (char *)text->data, text->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);
}

static void
php_hoedown_renderer_table_cell(hoedown_buffer *ob, const hoedown_buffer *text,
                                unsigned int flags, void *opaque)
{
   zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("tablecell", opaque TSRMLS_CC);
    if (!definition) {
        return;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 2);
    if (text) {
        add_next_index_stringl(parameters, (char *)text->data, text->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }
    add_next_index_long(parameters, flags);

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);
}

static void
php_hoedown_renderer_footnotes(hoedown_buffer *ob,
                               const hoedown_buffer *text, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("footnotes", opaque TSRMLS_CC);
    if (!definition) {
        return;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 1);
    if (text) {
        add_next_index_stringl(parameters, (char *)text->data, text->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);
}

static void
php_hoedown_renderer_footnote_def(hoedown_buffer *ob,
                                  const hoedown_buffer *text, unsigned int num,
                                  void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("footnotedef", opaque TSRMLS_CC);
    if (!definition) {
        return;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 2);
    if (text) {
        add_next_index_stringl(parameters, (char *)text->data, text->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }
    add_next_index_long(parameters, num);

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);
}

static int
php_hoedown_renderer_autolink(hoedown_buffer *ob, const hoedown_buffer *link,
                              enum hoedown_autolink type, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("autolink", opaque TSRMLS_CC);
    if (!definition) {
        return 0;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 2);
    if (link) {
        add_next_index_stringl(parameters, (char *)link->data, link->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }
    add_next_index_long(parameters, type);

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);

    return 1;
}

static int
php_hoedown_renderer_codespan(hoedown_buffer *ob, const hoedown_buffer *text,
                              const hoedown_buffer *attr, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("codespan", opaque TSRMLS_CC);
    if (!definition) {
        return 0;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 2);
    if (text) {
        add_next_index_stringl(parameters, (char *)text->data, text->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }
    if (attr) {
        add_next_index_stringl(parameters, (char *)attr->data, attr->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);

    return 1;
}

static int
php_hoedown_renderer_double_emphasis(hoedown_buffer *ob,
                                     const hoedown_buffer *text, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("doubleemphasis", opaque TSRMLS_CC);
    if (!definition) {
        return 0;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 1);
    if (text) {
        add_next_index_stringl(parameters, (char *)text->data, text->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);

    return 1;
}

static int
php_hoedown_renderer_emphasis(hoedown_buffer *ob,
                              const hoedown_buffer *text, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("emphasis", opaque TSRMLS_CC);
    if (!definition) {
        return 0;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 1);
    if (text) {
        add_next_index_stringl(parameters, (char *)text->data, text->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);

    return 1;
}

static int
php_hoedown_renderer_underline(hoedown_buffer *ob,
                               const hoedown_buffer *text, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("underline", opaque TSRMLS_CC);
    if (!definition) {
        return 0;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 1);
    if (text) {
        add_next_index_stringl(parameters, (char *)text->data, text->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);

    return 1;
}

static int
php_hoedown_renderer_highlight(hoedown_buffer *ob,
                               const hoedown_buffer *text, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("highlight", opaque TSRMLS_CC);
    if (!definition) {
        return 0;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 1);
    if (text) {
        add_next_index_stringl(parameters, (char *)text->data, text->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);

    return 1;
}

static int
php_hoedown_renderer_quote(hoedown_buffer *ob,
                           const hoedown_buffer *text, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("quote", opaque TSRMLS_CC);
    if (!definition) {
        return 0;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 1);
    if (text) {
        add_next_index_stringl(parameters, (char *)text->data, text->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);

    return 1;
}

static int
php_hoedown_renderer_image(hoedown_buffer *ob,
                           const hoedown_buffer *link,
                           const hoedown_buffer *title,
                           const hoedown_buffer *alt,
                           const hoedown_buffer *attr, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("image", opaque TSRMLS_CC);
    if (!definition) {
        return 0;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 4);
    if (link) {
        add_next_index_stringl(parameters, (char *)link->data, link->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }
    if (title) {
        add_next_index_stringl(parameters, (char *)title->data, title->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }
    if (alt) {
        add_next_index_stringl(parameters, (char *)alt->data, alt->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }
    if (attr) {
        add_next_index_stringl(parameters, (char *)attr->data, attr->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);

    return 1;
}

static int
php_hoedown_renderer_linebreak(hoedown_buffer *ob, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("linebreak", opaque TSRMLS_CC);
    if (!definition) {
        return 0;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 0);

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);

    return 1;
}

static int
php_hoedown_renderer_link(hoedown_buffer *ob,
                          const hoedown_buffer *link,
                          const hoedown_buffer *title,
                          const hoedown_buffer *content,
                          const hoedown_buffer *attr, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("link", opaque TSRMLS_CC);
    if (!definition) {
        return 0;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 4);
    if (link) {
        add_next_index_stringl(parameters, (char *)link->data, link->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }
    if (title) {
        add_next_index_stringl(parameters, (char *)title->data, title->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }
    if (content) {
        add_next_index_stringl(parameters,
                               (char *)content->data, content->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }
    if (attr) {
        add_next_index_stringl(parameters, (char *)attr->data, attr->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);

    return 1;
}

static int
php_hoedown_renderer_raw_html_tag(hoedown_buffer *ob,
                                  const hoedown_buffer *tag, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("rawhtmltag", opaque TSRMLS_CC);
    if (!definition) {
        return 0;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 1);
    if (tag) {
        add_next_index_stringl(parameters, (char *)tag->data, tag->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);

    return 1;
}

static int
php_hoedown_renderer_triple_emphasis(hoedown_buffer *ob,
                                     const hoedown_buffer *text, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("tripleemphasis", opaque TSRMLS_CC);
    if (!definition) {
        return 0;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 1);
    if (text) {
        add_next_index_stringl(parameters, (char *)text->data, text->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);

    return 1;
}

static int
php_hoedown_renderer_strikethrough(hoedown_buffer *ob,
                                   const hoedown_buffer *text, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("strikethrough", opaque TSRMLS_CC);
    if (!definition) {
        return 0;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 1);
    if (text) {
        add_next_index_stringl(parameters, (char *)text->data, text->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);

    return 1;
}

static int
php_hoedown_renderer_superscript(hoedown_buffer *ob,
                                 const hoedown_buffer *text, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("superscript", opaque TSRMLS_CC);
    if (!definition) {
        return 0;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 1);
    if (text) {
        add_next_index_stringl(parameters, (char *)text->data, text->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);

    return 1;
}

static int
php_hoedown_renderer_footnote_ref(hoedown_buffer *ob,
                                  unsigned int num, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("footnoteref", opaque TSRMLS_CC);
    if (!definition) {
        return 0;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 1);
    add_next_index_long(parameters, num);

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);

    return 1;
}

static void
php_hoedown_renderer_entity(hoedown_buffer *ob,
                            const hoedown_buffer *entity, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("entity", opaque TSRMLS_CC);
    if (!definition) {
        return;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 1);
    if (entity) {
        add_next_index_stringl(parameters,
                               (char *)entity->data, entity->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);
}

static void
php_hoedown_renderer_normal_text(hoedown_buffer *ob,
                                 const hoedown_buffer *text, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("normaltext", opaque TSRMLS_CC);
    if (!definition) {
        return;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 1);
    if (text) {
        add_next_index_stringl(parameters, (char *)text->data, text->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);
}

static void
php_hoedown_renderer_user_block(hoedown_buffer *ob,
                                const hoedown_buffer *text, void *opaque)
{
    zval *definition, *parameters;
    TSRMLS_FETCH();

    definition = php_hoedown_is_renderer("userblock", opaque TSRMLS_CC);
    if (!definition) {
        return;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 1);
    if (text) {
        add_next_index_stringl(parameters, (char *)text->data, text->size, 1);
    } else {
        add_next_index_stringl(parameters, "", 0, 1);
    }

    php_hoedown_run_renderer(ob, definition, parameters TSRMLS_CC);

    zval_ptr_dtor(&parameters);
}

static size_t
php_hoedown_renderer_is_user_block(uint8_t *data, size_t size, void *opaque)
{
    hoedown_html_renderer_state *state = opaque;
    zval *definition = NULL, *parameters;
    zval fname;
    zval *result = NULL, **args[2];
    long length = 0;
    TSRMLS_FETCH();

    if (state) {
        definition = ((php_hoedown_options_t *)state->opaque)->is_user;
    }
    if (!definition) {
        HOEDOWN_ERR(E_WARNING, "Undefined is_user");
        return 0;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 1);
    add_next_index_stringl(parameters, (char *)data, size, 1);

    ZVAL_STRINGL(&fname, "call_user_func_array",
                 sizeof("call_user_func_array")-1, 0);

    args[0] = &definition;
    args[1] = &parameters;

    call_user_function_ex(EG(function_table), NULL, &fname,
                          &result, 2, args, 0, NULL TSRMLS_CC);

    if (result) {
        if (Z_TYPE_P(result) == IS_LONG) {
            length = Z_LVAL_P(result);
        }
        zval_ptr_dtor(&result);
    }

    zval_ptr_dtor(&parameters);

    return length;
}

static void
php_hoedown_set_renderer(zval *renders, hoedown_renderer *renderer,
                         char *name TSRMLS_DC)
{
    zval **definition = NULL;
    char *error = NULL;
    zend_bool retval;

    if (!name || !renders || Z_TYPE_P(renders) != IS_ARRAY) {
        HOEDOWN_ERR(E_WARNING, "Undefined renders");
        return;
    }

    if (zend_hash_find(Z_ARRVAL_P(renders), name, strlen(name)+1,
                       (void **)&definition) == FAILURE || !definition) {
        HOEDOWN_ERR(E_WARNING, "Undefined render '%s'", name);
        return;
    }

    retval = zend_is_callable_ex(*definition, NULL, 0, NULL, NULL, NULL,
                                 &error TSRMLS_CC);
    if (error) {
        efree(error);
    }
    if (!retval) {
        HOEDOWN_ERR(E_WARNING, "Call to undefined render '%s'", name);
        /*
        zend_throw_exception_ex(NULL, 0 TSRMLS_CC,
                                "Call to undefined render '%s'", name);
        */
        return;
    }

    if (strcmp(name, "blockcode") == 0) {
        renderer->blockcode = php_hoedown_renderer_blockcode;
    } else if (strcmp(name, "blockquote") == 0) {
        renderer->blockquote = php_hoedown_renderer_blockquote;
    } else if (strcmp(name, "blockhtml") == 0) {
        renderer->blockhtml = php_hoedown_renderer_blockhtml;
    } else if (strcmp(name, "header") == 0) {
        renderer->header = php_hoedown_renderer_header;
    } else if (strcmp(name, "hrule") == 0) {
        renderer->hrule = php_hoedown_renderer_hrule;
    } else if (strcmp(name, "list") == 0) {
        renderer->list = php_hoedown_renderer_list;
    } else if (strcmp(name, "listitem") == 0) {
        renderer->listitem = php_hoedown_renderer_listitem;
    } else if (strcmp(name, "paragraph") == 0) {
        renderer->paragraph = php_hoedown_renderer_paragraph;
    } else if (strcmp(name, "table") == 0) {
        renderer->table = php_hoedown_renderer_table;
    } else if (strcmp(name, "tablerow") == 0) {
        renderer->table_row = php_hoedown_renderer_table_row;
    } else if (strcmp(name, "tablecell") == 0) {
        renderer->table_cell = php_hoedown_renderer_table_cell;
    } else if (strcmp(name, "footnotes") == 0) {
        renderer->footnotes = php_hoedown_renderer_footnotes;
    } else if (strcmp(name, "footnotedef") == 0) {
        renderer->footnote_def = php_hoedown_renderer_footnote_def;
    } else if (strcmp(name, "autolink") == 0) {
        renderer->autolink = php_hoedown_renderer_autolink;
    } else if (strcmp(name, "codespan") == 0) {
        renderer->codespan = php_hoedown_renderer_codespan;
    } else if (strcmp(name, "doubleemphasis") == 0) {
        renderer->double_emphasis = php_hoedown_renderer_double_emphasis;
    } else if (strcmp(name, "emphasis") == 0) {
        renderer->emphasis = php_hoedown_renderer_emphasis;
    } else if (strcmp(name, "underline") == 0) {
        renderer->underline = php_hoedown_renderer_underline;
    } else if (strcmp(name, "highlight") == 0) {
        renderer->highlight = php_hoedown_renderer_highlight;
    } else if (strcmp(name, "quote") == 0) {
        renderer->quote = php_hoedown_renderer_quote;
    } else if (strcmp(name, "image") == 0) {
        renderer->image = php_hoedown_renderer_image;
    } else if (strcmp(name, "linebreak") == 0) {
        renderer->linebreak = php_hoedown_renderer_linebreak;
    } else if (strcmp(name, "link") == 0) {
        renderer->link = php_hoedown_renderer_link;
    } else if (strcmp(name, "rawhtmltag") == 0) {
        renderer->raw_html_tag = php_hoedown_renderer_raw_html_tag;
    } else if (strcmp(name, "tripleemphasis") == 0) {
        renderer->triple_emphasis = php_hoedown_renderer_triple_emphasis;
    } else if (strcmp(name, "strikethrough") == 0) {
        renderer->strikethrough = php_hoedown_renderer_strikethrough;
    } else if (strcmp(name, "superscript") == 0) {
        renderer->superscript = php_hoedown_renderer_superscript;
    } else if (strcmp(name, "footnoteref") == 0) {
        renderer->footnote_ref = php_hoedown_renderer_footnote_ref;
    } else if (strcmp(name, "entity") == 0) {
        renderer->entity = php_hoedown_renderer_entity;
    } else if (strcmp(name, "normaltext") == 0) {
        renderer->normal_text = php_hoedown_renderer_normal_text;
    } else if (strcmp(name, "userblock") == 0) {
        renderer->user_block = php_hoedown_renderer_user_block;
    } else {
        HOEDOWN_ERR(E_WARNING, "Undefined render function '%s'", name);
    }
}

static int
php_hoedown_is_user_block(zval *definition TSRMLS_DC)
{
    char *error = NULL;
    zend_bool retval;

    if (!definition) {
        return 0;
    }

    retval = zend_is_callable_ex(definition, NULL, 0, NULL, NULL, NULL,
                                 &error TSRMLS_CC);
    if (error) {
        efree(error);
    }
    if (!retval) {
        HOEDOWN_ERR(E_WARNING, "Call to undefined is_user");
        return 0;
    }
    return 1;
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
    buf = hoedown_buffer_new(HOEDOWN_BUFFER_UNIT);
    if (!buf) {
        HOEDOWN_ERR(E_WARNING, "Couldn't allocate output buffer");
        return 1;
    }

    /* toc */
    if (options->toc.end > 0) {
        toc_level = options->toc.end;
        if (return_state || options->renderer == HOEDOWN_OPT_RENDERER_TOC) {
            renderer = hoedown_html_toc_renderer_new(0);

            state = (hoedown_html_renderer_state *)renderer->opaque;

            state->toc_data.current_level = 0;
            state->toc_data.level_offset = options->toc.begin;
            state->toc_data.nesting_level = toc_level;
            state->toc_data.unescape = options->toc.unescape;
            if (options->toc.header) {
                state->toc_data.header = Z_STRVAL_P(options->toc.header);
            }
            if (options->toc.footer) {
                state->toc_data.footer = Z_STRVAL_P(options->toc.footer);
            }

            if (php_hoedown_is_user_block(options->is_user TSRMLS_CC)) {
                document = hoedown_document_new(
                    renderer, options->extension,
                    16, php_hoedown_renderer_is_user_block);
            } else {
                document = hoedown_document_new(renderer, options->extension,
                                                16, NULL);
            }

            hoedown_document_render(document, buf,
                                    (const uint8_t *)contents, length);

            hoedown_document_free(document);
            hoedown_html_renderer_free(renderer);

            if (options->renderer == HOEDOWN_OPT_RENDERER_TOC) {
                RETVAL_STRINGL((char *)buf->data, buf->size, 1);
                hoedown_buffer_free(buf);
                return 0;
            } else {
                zval *zv;

                MAKE_STD_ZVAL(zv);
                ZVAL_STRINGL(zv, (char *)buf->data, buf->size, 1);

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

    /* override render */
    if (options->renders && Z_TYPE_P(options->renders) == IS_ARRAY) {
        HashTable *h = Z_ARRVAL_P(options->renders);
        HashPosition pos;
        ulong int_key;
        char *str_key = NULL;
        uint str_key_len;

        zend_hash_internal_pointer_reset_ex(h, &pos);
        do {
            if (zend_hash_get_current_key_ex(h, &str_key, &str_key_len,
                                             &int_key, 0, &pos)
                != HASH_KEY_IS_STRING || !str_key) {
                break;
            }
            php_hoedown_set_renderer(options->renders, renderer,
                                     str_key TSRMLS_CC);
        } while (!zend_hash_move_forward_ex(h, &pos));
    }

    /* setting state */
    state = (hoedown_html_renderer_state *)renderer->opaque;
    state->opaque = (void *)options;

    /* init document */
    if (php_hoedown_is_user_block(options->is_user TSRMLS_CC)) {
        document = hoedown_document_new(renderer, options->extension,
                                        16, php_hoedown_renderer_is_user_block);
    } else {
        document = hoedown_document_new(renderer, options->extension, 16, NULL);
    }

    /* execute parse */
    hoedown_document_render(document, buf,
                            (const uint8_t *)contents, length);

    /* cleanup */
    hoedown_document_free(document);
    hoedown_html_renderer_free(renderer);

    /* setting return value */
    RETVAL_STRINGL((char *)buf->data, buf->size, 1);

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

HOEDOWN_METHOD(addRender)
{
    char *name;
    int name_len;
    zval *definition;
    php_hoedown_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz",
                              &name, &name_len, &definition) == FAILURE) {
        RETURN_FALSE;
    }

    if (name_len == 0) {
        RETURN_FALSE;
    }

    HOEDOWN_OBJ(intern, getThis());

    if (!intern->options.renders) {
        MAKE_STD_ZVAL(intern->options.renders);
        array_init(intern->options.renders);
    }

    Z_ADDREF_P(definition);

    zend_symtable_update(Z_ARRVAL_P(intern->options.renders), name, name_len+1,
                         &definition, sizeof(zval*), NULL);

    RETVAL_TRUE;
}

HOEDOWN_METHOD(getRenders)
{
    php_hoedown_t *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        RETURN_FALSE;
    }

    HOEDOWN_OBJ(intern, getThis());

    if (!intern->options.renders) {
        RETVAL_NULL();
    } else {
        RETVAL_ZVAL(intern->options.renders, 1, 0);
    }
}

HOEDOWN_METHOD(ofString)
{
    char *buf;
    int buf_len;
    zval *opts = NULL, *state = NULL;
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
    HOEDOWN_ME(addRender, arginfo_hoedown_addrender, ZEND_ACC_PUBLIC)
    HOEDOWN_ME(getRenders, arginfo_hoedown_getrenders, ZEND_ACC_PUBLIC)
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
#if ZEND_MODULE_API_NO < 20100525
    zval *tmp;
#endif

    intern = (php_hoedown_t *)emalloc(sizeof(php_hoedown_t));
    memset(intern, 0, sizeof(php_hoedown_t));
    if (ptr) {
        *ptr = intern;
    }

    zend_object_std_init(&intern->std, ce TSRMLS_CC);
#if ZEND_MODULE_API_NO >= 20100525
    object_properties_init(&intern->std, ce);
#else
    zend_hash_copy(intern->std.properties, &ce->default_properties,
                   (copy_ctor_func_t)zval_add_ref, (void *)&tmp, sizeof(zval *));
#endif

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
    hoedown_globals->options = HOEDOWN_DEFAULT_OPTIONS;
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
    HOEDOWN_CONST_LONG(EXPAND_TABS, HOEDOWN_OPT_HTML_EXPAND_TABS);
    HOEDOWN_CONST_LONG(SAFELINK, HOEDOWN_OPT_HTML_SAFELINK);
    HOEDOWN_CONST_LONG(HARD_WRAP, HOEDOWN_OPT_HTML_HARD_WRAP);
    HOEDOWN_CONST_LONG(USE_XHTML, HOEDOWN_OPT_HTML_USE_XHTML);
    HOEDOWN_CONST_LONG(ESCAPE, HOEDOWN_OPT_HTML_ESCAPE);
    HOEDOWN_CONST_LONG(TASK_LIST, HOEDOWN_OPT_HTML_TASK_LIST);
    HOEDOWN_CONST_LONG(LINE_CONTINUE, HOEDOWN_OPT_HTML_LINE_CONTINUE);
    HOEDOWN_CONST_LONG(HEADER_ID, HOEDOWN_OPT_HTML_HEADER_ID);

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
    HOEDOWN_CONST_LONG(SPECIAL_ATTRIBUTE, HOEDOWN_OPT_EXT_SPECIAL_ATTRIBUTE);

    HOEDOWN_CONST_LONG(TOC, HOEDOWN_OPT_TOC);
    HOEDOWN_CONST_LONG(TOC_BEGIN, HOEDOWN_OPT_TOC_BEGIN);
    HOEDOWN_CONST_LONG(TOC_END, HOEDOWN_OPT_TOC_END);
    HOEDOWN_CONST_LONG(TOC_ESCAPE, HOEDOWN_OPT_TOC_ESCAPE);
    HOEDOWN_CONST_LONG(TOC_HEADER, HOEDOWN_OPT_TOC_HEADER);
    HOEDOWN_CONST_LONG(TOC_FOOTER, HOEDOWN_OPT_TOC_FOOTER);

    HOEDOWN_CONST_LONG(RENDERS, HOEDOWN_OPT_RENDERS);
    HOEDOWN_CONST_LONG(IS_USER, HOEDOWN_OPT_IS_USER_BLOCK);

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
