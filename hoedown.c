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
#ifdef ZEND_ENGINE_3
        zval header;
        zval footer;
#else
        zval *header;
        zval *footer;
#endif
    } toc;
#ifdef ZEND_ENGINE_3
    zval renders;
    zval user_block;
    zval meta_parse;
#else
    zval *renders;
    zval *user_block;
    zval *meta_parse;
#endif
} php_hoedown_options_t;

typedef struct {
#ifndef ZEND_ENGINE_3
    zend_object std;
#endif
    php_hoedown_options_t options;
#ifdef ZEND_ENGINE_3
    zend_object std;
#endif
} php_hoedown_t;

#define HOEDOWN_METHOD(name) ZEND_METHOD(Hoedown, name)
#define HOEDOWN_ME(name, arg_info, flags) ZEND_ME(Hoedown, name, arg_info, flags)
#define HOEDOWN_MALIAS(alias, name, arg_info, flags) \
    ZEND_MALIAS(Hoedown, alias, name, arg_info, flags)
#define HOEDOWN_CONST_LONG(name, value) \
    zend_declare_class_constant_long(php_hoedown_ce, ZEND_STRS(#name)-1, \
                                     value)

#define HOEDOWN_DEFAULT_OPTIONS \
    "tables,fenced-code,autolink,strikethrough,no-intra-emphasis"

ZEND_INI_BEGIN()
    STD_ZEND_INI_ENTRY("hoedown.options", HOEDOWN_DEFAULT_OPTIONS,
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
    HOEDOWN_OPT_HTML_HARD_WRAP,
    HOEDOWN_OPT_HTML_USE_XHTML,
    HOEDOWN_OPT_HTML_USE_TASK_LIST,
    HOEDOWN_OPT_HTML_LINE_CONTINUE,
    HOEDOWN_OPT_HTML_HEADER_ID,
    HOEDOWN_OPT_HTML_FENCED_CODE_SCRIPT,
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
    HOEDOWN_OPT_EXT_MATH,
    HOEDONE_OPT_EXT__DUMMY,
    HOEDOWN_OPT_EXT_NO_INTRA_EMPHASIS,
    HOEDOWN_OPT_EXT_SPACE_HEADERS,
    HOEDOWN_OPT_EXT_MATH_EXPLICIT,
    HOEDOWN_OPT_EXT_DISABLE_INDENTED_CODE,
    HOEDOWN_OPT_EXT_SPECIAL_ATTRIBUTE,
    HOEDOWN_OPT_EXT_SCRIPT_TAGS,
    HOEDOWN_OPT_EXT_META_BLOCK,
    HOEDOWN_OPT_EXT_END,
    HOEDOWN_OPT_TOC,
    HOEDOWN_OPT_TOC_BEGIN,
    HOEDOWN_OPT_TOC_END,
    HOEDOWN_OPT_TOC_HEADER,
    HOEDOWN_OPT_TOC_FOOTER,
    HOEDOWN_OPT_RENDERS,
    HOEDOWN_OPT_USER_BLOCK,
    HOEDOWN_OPT_META_PARSE,
};

#define HOEDOWN_DEFAULT_OPT_TOC_LEVEL 6

#ifdef ZEND_ENGINE_3
#define HOEDOWN_OBJ(self, obj) \
    self = (php_hoedown_t *)((char *)Z_OBJ_P(obj) - XtOffsetOf(php_hoedown_t, std))
#else
#define HOEDOWN_OBJ(self, obj) \
    self = (php_hoedown_t *)zend_object_store_get_object(obj)
#endif

static int
php_hoedown_set_option(php_hoedown_options_t *options,
                       int opt, zval *val)
{
#ifdef ZEND_ENGINE_3
    if (Z_TYPE_P(val) == IS_LONG &&
        (opt != HOEDOWN_OPT_TOC_END && opt != HOEDOWN_OPT_TOC_BEGIN)) {
        opt = Z_LVAL_P(val);
    }
#endif

    if (opt >= HOEDOWN_OPT_HTML && opt < HOEDOWN_OPT_HTML_END) {
        int n = (1 << (opt - HOEDOWN_OPT_HTML));
        convert_to_boolean(val);
#ifdef ZEND_ENGINE_3
        if (Z_TYPE_P(val) == IS_TRUE) {
#else
        if (Z_BVAL_P(val)) {
#endif
            options->html |= n;
            if (opt == HOEDOWN_OPT_HTML_FENCED_CODE_SCRIPT) {
                options->extension |= HOEDOWN_OPT_EXT_FENCED_CODE;
            }
        } else if (options->html & n) {
            options->html ^= n;
        }
        return 0;
    } else if (opt >= HOEDOWN_OPT_EXT && opt < HOEDOWN_OPT_EXT_END) {
        int n = (1 << (opt - HOEDOWN_OPT_EXT));
        convert_to_boolean(val);
#ifdef ZEND_ENGINE_3
        if (Z_TYPE_P(val) == IS_TRUE) {
#else
        if (Z_BVAL_P(val)) {
#endif
            options->extension |= n;
        } else if (options->extension & n) {
            options->extension ^= n;
        }
        return 0;
    }

    switch (opt) {
        case HOEDOWN_OPT_RENDERER_HTML:
            convert_to_boolean(val);
#ifdef ZEND_ENGINE_3
            if (Z_TYPE_P(val) == IS_TRUE) {
#else
            if (Z_BVAL_P(val)) {
#endif
                options->renderer = opt;
            }
            return 0;
        case HOEDOWN_OPT_RENDERER_TOC:
            convert_to_boolean(val);
#ifdef ZEND_ENGINE_3
            if (Z_TYPE_P(val) == IS_TRUE) {
#else
            if (Z_BVAL_P(val)) {
#endif
                options->renderer = opt;
            } else {
                options->renderer = HOEDOWN_OPT_RENDERER_HTML;
            }
            return 0;
        case HOEDOWN_OPT_TOC:
            convert_to_boolean(val);
#ifdef ZEND_ENGINE_3
            if (Z_TYPE_P(val) == IS_TRUE) {
#else
            if (Z_BVAL_P(val)) {
#endif
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
        case HOEDOWN_OPT_TOC_HEADER:
            convert_to_string(val);
#ifdef ZEND_ENGINE_3
            if (Z_STRLEN_P(val) <= 0) {
                if (!Z_ISUNDEF(options->toc.header)) {
                    zval_dtor(&options->toc.header);
                    ZVAL_UNDEF(&options->toc.header);
                }
            } else {
                ZVAL_ZVAL(&options->toc.header, val, 1, 0);
            }
#else
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
#endif
            return 0;
        case HOEDOWN_OPT_TOC_FOOTER:
            convert_to_string(val);
#ifdef ZEND_ENGINE_3
            if (Z_STRLEN_P(val) <= 0) {
                if (!Z_ISUNDEF(options->toc.footer)) {
                    zval_dtor(&options->toc.footer);
                    ZVAL_UNDEF(&options->toc.footer);
                }
            } else {
                ZVAL_ZVAL(&options->toc.footer, val, 1, 0);
            }
#else
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
#endif
            return 0;
        case HOEDOWN_OPT_RENDERS:
#ifdef ZEND_ENGINE_3
            php_array_merge(Z_ARRVAL_P(&options->renders), Z_ARRVAL_P(val));
            return 0;
#else
            if (!options->renders) {
                MAKE_STD_ZVAL(options->renders);
                array_init(options->renders);
            }
            if (Z_TYPE_P(val) == IS_ARRAY) {
                php_array_merge(Z_ARRVAL_P(options->renders), Z_ARRVAL_P(val),
                                1);
                return 0;
            }
            break;
#endif
        case HOEDOWN_OPT_USER_BLOCK:
#ifdef ZEND_ENGINE_3
            if (!Z_ISUNDEF(options->user_block)) {
                zval_dtor(&options->user_block);
                ZVAL_UNDEF(&options->user_block);
            }
            if (Z_TYPE_P(val) != IS_NULL) {
                ZVAL_ZVAL(&options->user_block, val, 1, 0);
            }
#else
            if (Z_TYPE_P(val) == IS_NULL) {
                if (options->user_block) {
                    zval_ptr_dtor(&options->user_block);
                    options->user_block = NULL;
                }
            } else {
                if (!options->user_block) {
                    MAKE_STD_ZVAL(options->user_block);
                }
                ZVAL_ZVAL(options->user_block, val, 1, 0);
            }
#endif
            return 0;
        case HOEDOWN_OPT_META_PARSE:
#ifdef ZEND_ENGINE_3
            if (Z_TYPE_P(val) == IS_NULL) {
                if (!Z_ISUNDEF(options->meta_parse)) {
                    zval_dtor(&options->meta_parse);
                    ZVAL_UNDEF(&options->meta_parse);
                }
            } else {
                ZVAL_ZVAL(&options->meta_parse, val, 1, 0);
            }
#else
            if (Z_TYPE_P(val) == IS_NULL) {
                if (options->meta_parse) {
                    zval_ptr_dtor(&options->meta_parse);
                    options->meta_parse = NULL;
                }
            } else {
                if (!options->meta_parse) {
                    MAKE_STD_ZVAL(options->meta_parse);
                }
                ZVAL_ZVAL(options->meta_parse, val, 1, 0);
            }
#endif
            return 0;
        default:
            break;
    }

    return 1;
}

static void
php_hoedown_set_options(php_hoedown_options_t *options, zval *val)
{
#ifdef ZEND_ENGINE_3
    zval *value;
    ulong key;

    ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(val), key, value) {
        php_hoedown_set_option(options, key, value);
    } ZEND_HASH_FOREACH_END();
#else
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
            php_hoedown_set_option(options, key_index, *value);
        } else {
            HOEDOWN_ERR(E_WARNING, "Invalid configuration options");
        }
    }
#endif
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
        { HOEDOWN_EXT_MATH, "math" },
        { HOEDOWN_EXT_SPACE_HEADERS, "space-headers" },
        { HOEDOWN_EXT_MATH_EXPLICIT, "math-explicit" },
        { HOEDOWN_EXT_DISABLE_INDENTED_CODE, "disable-indented-code" },
        { HOEDOWN_EXT_SPECIAL_ATTRIBUTE, "special-attribute" },
        { HOEDOWN_EXT_SCRIPT_TAGS, "script-tags" },
        { HOEDOWN_EXT_META_BLOCK, "meta-block" },
    };
    php_hoedown_flag_t html_flags[] = {
        { HOEDOWN_HTML_SKIP_HTML, "skip-html" },
        { HOEDOWN_HTML_HARD_WRAP, "hard-wrap" },
        { HOEDOWN_HTML_USE_XHTML, "xhtml" },
        { HOEDOWN_HTML_ESCAPE, "escape" },
        { HOEDOWN_HTML_USE_TASK_LIST, "task" },
        { HOEDOWN_HTML_LINE_CONTINUE, "line-continue" },
        { HOEDOWN_HTML_HEADER_ID, "header-id" },
        { HOEDOWN_HTML_FENCED_CODE_SCRIPT, "fenced-code-script" },
    };
    php_hoedown_flag_t *flag;
    int n, i = 0, j = 0;

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
            if (flag->option == HOEDOWN_HTML_FENCED_CODE_SCRIPT) {
                options->extension |= HOEDOWN_EXT_FENCED_CODE;
            }
            return;
        }
        ++i;
    } while (i < n);

    /* toc */
    if (strncasecmp(name, "toc", 3) == 0) {
        options->toc.end = HOEDOWN_DEFAULT_OPT_TOC_LEVEL;
        if (size > 3 && name[3] == ':') {
            n = 0;
            for (i = 4; i < size; i++) {
                if (isdigit(name[i])) {
                    if (n) {
                        n = (n * 10) + (name[i] - '0');
                    } else {
                        n = (name[i] - '0');
                    }
                } else if (name[i] == ':') {
                    if (n) {
                        options->toc.begin = n;
                    }
                    n = 0;
                    j++;
                }
            }
            if (n) {
                if (j) {
                    options->toc.end = n;
                } else {
                    options->toc.begin = n;
                }
            }
        }
    }
}

static void
php_hoedown_options_init(php_hoedown_options_t *options)
{
    char *opts, *p1, *p2, *endp;

    memset(options, 0, sizeof(php_hoedown_options_t));

    options->renderer = HOEDOWN_OPT_RENDERER_HTML;
    options->html = 0;
    options->extension = 0;

    options->toc.begin = 0;
    options->toc.end = 0;
#ifdef ZEND_ENGINE_3
    ZVAL_UNDEF(&options->toc.header);
    ZVAL_UNDEF(&options->toc.footer);

    array_init(&options->renders);

    ZVAL_UNDEF(&options->user_block);
    ZVAL_UNDEF(&options->meta_parse);
#endif

    opts = HOEDOWN_G(options);
    if (!opts || strlen(opts) == 0) {
        return;
    }

    endp = opts + strlen(opts);
    p1 = opts;
    p2 = (char *)php_memnstr(opts, ",", 1, endp);

    if (p2 == NULL) {
        php_hoedown_set_options_flag(options, p1, strlen(opts));
    } else {
        do {
            php_hoedown_set_options_flag(options, p1, p2 - p1);
            p1 = p2 + 1;
        } while ((p2 = (char *)php_memnstr(p1, ",", 1, endp)) != NULL);
        if (p1 <= endp) {
            php_hoedown_set_options_flag(options, p1, endp - p1);
        }
    }
}

static void
php_hoedown_options_destroy(php_hoedown_options_t *options)
{
#ifdef ZEND_ENGINE_3
    if (!Z_ISUNDEF(options->toc.header)) {
        zval_dtor(&options->toc.header);
    }
    if (!Z_ISUNDEF(options->toc.footer)) {
        zval_dtor(&options->toc.footer);
    }
    zval_dtor(&options->renders);
    if (!Z_ISUNDEF(options->user_block)) {
        zval_dtor(&options->user_block);
    }
    if (!Z_ISUNDEF(options->meta_parse)) {
        zval_dtor(&options->meta_parse);
    }
#else
    if (options->toc.header) {
        zval_ptr_dtor(&options->toc.header);
    }
    if (options->toc.footer) {
        zval_ptr_dtor(&options->toc.footer);
    }
    if (options->renders) {
        zval_ptr_dtor(&options->renders);
    }
    if (options->user_block) {
        zval_ptr_dtor(&options->user_block);
    }
    if (options->meta_parse) {
        zval_ptr_dtor(&options->meta_parse);
    }
#endif
}

HOEDOWN_METHOD(__construct)
{
    zval *options = NULL;
    php_hoedown_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|a",
                              &options) == FAILURE) {
        RETURN_FALSE;
    }

    HOEDOWN_OBJ(intern, getThis());

    if (options) {
        php_hoedown_set_options(&intern->options, options);
    }
}

HOEDOWN_METHOD(setOption)
{
#ifdef ZEND_ENGINE_3
    zend_long option;
#else
    long option;
#endif
    zval *value;
    php_hoedown_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "lz",
                              &option, &value) == FAILURE) {
        RETURN_FALSE;
    }

    HOEDOWN_OBJ(intern, getThis());

    if (php_hoedown_set_option(&intern->options, option, value) != 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

HOEDOWN_METHOD(getOption)
{
#ifdef ZEND_ENGINE_3
    zend_long option;
#else
    long option;
#endif
    php_hoedown_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l",
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
            case HOEDOWN_OPT_TOC_HEADER:
#ifdef ZEND_ENGINE_3
                if (!Z_ISUNDEF(intern->options.toc.header)) {
                    RETURN_ZVAL(&intern->options.toc.header, 1, 0);
                }
#else
                if (intern->options.toc.header) {
                    RETURN_ZVAL(intern->options.toc.header, 1, 0);
                }
#endif
                RETURN_NULL();
            case HOEDOWN_OPT_TOC_FOOTER:
#ifdef ZEND_ENGINE_3
                if (!Z_ISUNDEF(intern->options.toc.footer)) {
                    RETURN_ZVAL(&intern->options.toc.footer, 1, 0);
                }
#else
                if (intern->options.toc.footer) {
                    RETURN_ZVAL(intern->options.toc.footer, 1, 0);
                }
#endif
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

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a",
                              &options) == FAILURE) {
        RETURN_FALSE;
    }

    HOEDOWN_OBJ(intern, getThis());

    php_hoedown_set_options(&intern->options, options);
}

static zval *
php_hoedown_is_renderer(char *name, const hoedown_renderer_data *data)
{
    hoedown_html_renderer_state *state = data->opaque;
    zval *renders = NULL;
#ifdef ZEND_ENGINE_3
    zval *definition = NULL;

    if (state) {
        renders = &((php_hoedown_options_t *)state->opaque)->renders;
    }

    if (!name || !renders) {
        HOEDOWN_ERR(E_WARNING, "Undefined renders");
        return NULL;
    }

    definition = zend_hash_str_find(Z_ARRVAL_P(renders), name, strlen(name));
    if (!definition) {
        HOEDOWN_ERR(E_WARNING, "Undefined render '%s'", name);
        return NULL;
    }

    return definition;
#else
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
#endif
}

static void
php_hoedown_run_renderer(hoedown_buffer *ob, zval *definition,
                         zval *parameters)
{
    zval fname;
#ifdef ZEND_ENGINE_3
    zval result, args[2];

    ZVAL_STRINGL(&fname, "call_user_func_array",
                 sizeof("call_user_func_array")-1);

    args[0] = *definition;
    args[1] = *parameters;

    call_user_function(EG(function_table), NULL, &fname,
                          &result, 2, args);
    if (Z_TYPE(result) == IS_STRING) {
        hoedown_buffer_put(ob, Z_STRVAL(result), Z_STRLEN(result));
    }

    zval_dtor(&fname);
    zval_dtor(&result);
#else
    zval *result = NULL, **args[2];

    ZVAL_STRINGL(&fname, "call_user_func_array",
                 sizeof("call_user_func_array")-1, 0);

    args[0] = &definition;
    args[1] = &parameters;

    call_user_function(EG(function_table), NULL, &fname,
                          &result, 2, args);
    if (result) {
        if (Z_TYPE_P(result) == IS_STRING) {
            hoedown_buffer_put(ob, Z_STRVAL_P(result), Z_STRLEN_P(result));
        }
        zval_ptr_dtor(&result);
    }
#endif
}

#ifdef ZEND_ENGINE_3
#define PHP_HOEDOWN_RENDERER_SETUP() zval *definition, parameters
#define PHP_HOEDOWN_RENDERER_INIT(f, x, n, r)                 \
    do {                                                      \
        definition = php_hoedown_is_renderer(f, x); \
        if (!definition) {                                    \
            r;                                                \
        }                                                     \
        array_init_size(&parameters, n);                      \
    } while(0)
#define PHP_HOEDOWN_RENDERER_PARAMETER_STRING(x)                               \
    do {                                                                       \
        if (x) {                                                               \
            add_next_index_stringl(&parameters, (char *)(x)->data, (x)->size); \
        } else {                                                               \
            add_next_index_stringl(&parameters, "", 0);                        \
        }                                                                      \
    } while(0)
#define PHP_HOEDOWN_RENDERER_PARAMETER_LONG(n) \
    add_next_index_long(&parameters, n)
#define PHP_HOEDOWN_RENDERER_RUN() \
    php_hoedown_run_renderer(ob, definition, &parameters)
#define PHP_HOEDOWN_RENDERER_DESTROY() zval_dtor(&parameters)
#else
#define PHP_HOEDOWN_RENDERER_SETUP() zval *definition, *parameters
#define PHP_HOEDOWN_RENDERER_INIT(f, x, n, r)                 \
    do {                                                      \
        definition = php_hoedown_is_renderer(f, x); \
        if (!definition) {                                    \
            r;                                                \
        }                                                     \
        MAKE_STD_ZVAL(parameters);                            \
        array_init_size(parameters, n);                       \
    } while(0)
#define PHP_HOEDOWN_RENDERER_PARAMETER_STRING(x)                              \
    do {                                                                      \
        if (x) {                                                              \
            add_next_index_stringl(parameters,(char *)(x)->data,(x)->size,1); \
        } else {                                                              \
            add_next_index_stringl(parameters,"",0,1);                        \
        }                                                                     \
    } while(0)
#define PHP_HOEDOWN_RENDERER_PARAMETER_LONG(n) \
    add_next_index_long(parameters, n)
#define PHP_HOEDOWN_RENDERER_RUN() \
    php_hoedown_run_renderer(ob, definition, parameters)
#define PHP_HOEDOWN_RENDERER_DESTROY() zval_ptr_dtor(&parameters)
#endif

static void
php_hoedown_renderer_blockcode(hoedown_buffer *ob,
                               const hoedown_buffer *text,
                               const hoedown_buffer *lang,
                               const hoedown_buffer *attr,
                               const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("blockcode", data, 3, return);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);
    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(lang);
    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(attr);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();
}

static void
php_hoedown_renderer_blockquote(hoedown_buffer *ob,
                                const hoedown_buffer *text,
                                const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("blockquote", data, 1, return);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();
}

static void
php_hoedown_renderer_blockhtml(hoedown_buffer *ob,
                               const hoedown_buffer *text,
                               const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("blockhtml", data, 1, return);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();
}

static void
php_hoedown_renderer_header(hoedown_buffer *ob,
                            const hoedown_buffer *text,
                            const hoedown_buffer *attr,
                            int level,
                            const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("header", data, 3, return);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);
    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(attr);
    PHP_HOEDOWN_RENDERER_PARAMETER_LONG(level);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();
}

static void
php_hoedown_renderer_hrule(hoedown_buffer *ob,
                           const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("hrule", data, 0, return);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();
}

static void
php_hoedown_renderer_list(hoedown_buffer *ob,
                          const hoedown_buffer *text,
                          const hoedown_buffer *attr,
                          unsigned int flags,
                          const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("list", data, 3, return);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);
    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(attr);
    PHP_HOEDOWN_RENDERER_PARAMETER_LONG(flags);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();
}

static void
php_hoedown_renderer_listitem(hoedown_buffer *ob,
                              const hoedown_buffer *text,
                              const hoedown_buffer *attr,
                              unsigned int *flags,
                              const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("listitem", data, 3, return);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);
    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(attr);
    PHP_HOEDOWN_RENDERER_PARAMETER_LONG(*flags);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();
}

static void
php_hoedown_renderer_paragraph(hoedown_buffer *ob,
                               const hoedown_buffer *text,
                               const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("paragraph", data, 1, return);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();
}

static void
php_hoedown_renderer_table(hoedown_buffer *ob,
                           const hoedown_buffer *content,
                           const hoedown_buffer *attr,
                           const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("table", data, 2, return);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(content);
    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(attr);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();
}

static void
php_hoedown_renderer_table_header(hoedown_buffer *ob,
                                  const hoedown_buffer *text,
                                  const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("tableheader", data, 1, return);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();
}

static void
php_hoedown_renderer_table_body(hoedown_buffer *ob,
                                const hoedown_buffer *text,
                                const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("tablebody", data, 1, return);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();
}

static void
php_hoedown_renderer_table_row(hoedown_buffer *ob,
                               const hoedown_buffer *text,
                               const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("tablerow", data, 1, return);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();
}

static void
php_hoedown_renderer_table_cell(hoedown_buffer *ob,
                                const hoedown_buffer *text,
                                unsigned int flags,
                                const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("tablecell", data, 2, return);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);
    PHP_HOEDOWN_RENDERER_PARAMETER_LONG(flags);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();
}

static void
php_hoedown_renderer_footnotes(hoedown_buffer *ob,
                               const hoedown_buffer *text,
                               const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("footnotes", data, 1, return);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();
}

static void
php_hoedown_renderer_footnote_def(hoedown_buffer *ob,
                                  const hoedown_buffer *text,
                                  unsigned int num,
                                  const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("footnotedef", data, 2, return);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);
    PHP_HOEDOWN_RENDERER_PARAMETER_LONG(num);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();
}

static int
php_hoedown_renderer_autolink(hoedown_buffer *ob,
                              const hoedown_buffer *link,
                              hoedown_autolink_type type,
                              const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("autolink", data, 2, return 0);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(link);
    PHP_HOEDOWN_RENDERER_PARAMETER_LONG(type);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();

    return 1;
}

static int
php_hoedown_renderer_codespan(hoedown_buffer *ob,
                              const hoedown_buffer *text,
                              const hoedown_buffer *attr,
                              const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("codespan", data, 2, return 0);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);
    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(attr);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();

    return 1;
}

static int
php_hoedown_renderer_double_emphasis(hoedown_buffer *ob,
                                     const hoedown_buffer *text,
                                     const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("doubleemphasis", data, 1, return 0);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();

    return 1;
}

static int
php_hoedown_renderer_emphasis(hoedown_buffer *ob,
                              const hoedown_buffer *text,
                              const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("emphasis", data, 1, return 0);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();

    return 1;
}

static int
php_hoedown_renderer_underline(hoedown_buffer *ob,
                               const hoedown_buffer *text,
                               const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("underline", data, 1, return 0);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();

    return 1;
}

static int
php_hoedown_renderer_highlight(hoedown_buffer *ob,
                               const hoedown_buffer *text,
                               const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("highlight", data, 1, return 0);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();

    return 1;
}

static int
php_hoedown_renderer_quote(hoedown_buffer *ob,
                           const hoedown_buffer *text,
                           const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("quote", data, 1, return 0);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();

    return 1;
}

static int
php_hoedown_renderer_image(hoedown_buffer *ob,
                           const hoedown_buffer *link,
                           const hoedown_buffer *title,
                           const hoedown_buffer *alt,
                           const hoedown_buffer *attr,
                           const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("image", data, 4, return 0);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(link);
    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(title);
    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(alt);
    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(attr);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();

    return 1;
}

static int
php_hoedown_renderer_linebreak(hoedown_buffer *ob,
                               const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("linebreak", data, 0, return 0);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();

    return 1;
}

static int
php_hoedown_renderer_link(hoedown_buffer *ob,
                          const hoedown_buffer *content,
                          const hoedown_buffer *link,
                          const hoedown_buffer *title,
                          const hoedown_buffer *attr,
                          const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("link", data, 4, return 0);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(content);
    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(link);
    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(title);
    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(attr);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();

    return 1;
}

static int
php_hoedown_renderer_raw_html(hoedown_buffer *ob,
                              const hoedown_buffer *tag,
                              const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("rawhtml", data, 1, return 0);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(tag);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();

    return 1;
}

static int
php_hoedown_renderer_triple_emphasis(hoedown_buffer *ob,
                                     const hoedown_buffer *text,
                                     const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("tripleemphasis", data, 1, return 0);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();

    return 1;
}

static int
php_hoedown_renderer_strikethrough(hoedown_buffer *ob,
                                   const hoedown_buffer *text,
                                   const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("strikethrough", data, 1, return 0);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();

    return 1;
}

static int
php_hoedown_renderer_superscript(hoedown_buffer *ob,
                                 const hoedown_buffer *text,
                                 const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("superscript", data, 1, return 0);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();

    return 1;
}

static int
php_hoedown_renderer_footnote_ref(hoedown_buffer *ob,
                                  unsigned int num,
                                  const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("footnoteref", data, 1, return 0);

    PHP_HOEDOWN_RENDERER_PARAMETER_LONG(num);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();

    return 1;
}

static void
php_hoedown_renderer_entity(hoedown_buffer *ob,
                            const hoedown_buffer *entity,
                            const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("entity", data, 1, return);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(entity);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();
}

static int
php_hoedown_renderer_math(hoedown_buffer *ob,
                          const hoedown_buffer *text, int displaymode,
                          const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("math", data, 2, return 0);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);
    PHP_HOEDOWN_RENDERER_PARAMETER_LONG(displaymode);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();

    return 1;
}

static void
php_hoedown_renderer_normal_text(hoedown_buffer *ob,
                                 const hoedown_buffer *text,
                                 const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("normaltext", data, 1, return);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();
}

static void
php_hoedown_renderer_user_block(hoedown_buffer *ob,
                                const hoedown_buffer *text,
                                const hoedown_renderer_data *data)
{
    PHP_HOEDOWN_RENDERER_SETUP();

    PHP_HOEDOWN_RENDERER_INIT("userblock", data, 1, return);

    PHP_HOEDOWN_RENDERER_PARAMETER_STRING(text);

    PHP_HOEDOWN_RENDERER_RUN();

    PHP_HOEDOWN_RENDERER_DESTROY();
}

static size_t
php_hoedown_callback_user_block(uint8_t *text, size_t size,
                                const hoedown_renderer_data *data)
{
    hoedown_html_renderer_state *state = data->opaque;
    zval *definition = NULL;
    zval fname;
    long length = 0;

#ifdef ZEND_ENGINE_3
    zval parameters;
    zval result, args[2];

    if (state) {
        definition = &((php_hoedown_options_t *)state->opaque)->user_block;
    }
    if (!definition) {
        HOEDOWN_ERR(E_WARNING, "Undefined user_block");
        return 0;
    }

    ZVAL_STRINGL(&fname, "call_user_func_array",
                 sizeof("call_user_func_array")-1);

    args[0] = *definition;
    array_init_size(&args[1], 1);
    add_next_index_stringl(&args[1], (char *)text, size);

    call_user_function(EG(function_table), NULL, &fname,
                          &result, 2, args);

    if (Z_TYPE(result) == IS_LONG) {
        length = Z_LVAL(result);
    }

    zval_dtor(&fname);
    zval_dtor(&args[1]);
    zval_dtor(&result);

#else
    zval *parameters;
    zval *result = NULL, **args[2];

    if (state) {
        definition = ((php_hoedown_options_t *)state->opaque)->user_block;
    }
    if (!definition) {
        HOEDOWN_ERR(E_WARNING, "Undefined user_block");
        return 0;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 1);
    add_next_index_stringl(parameters, (char *)text, size, 1);

    ZVAL_STRINGL(&fname, "call_user_func_array",
                 sizeof("call_user_func_array")-1, 0);

    args[0] = &definition;
    args[1] = &parameters;

    call_user_function(EG(function_table), NULL, &fname,
                          &result, 2, args);

    if (result) {
        if (Z_TYPE_P(result) == IS_LONG) {
            length = Z_LVAL_P(result);
        }
        zval_ptr_dtor(&result);
    }

    zval_ptr_dtor(&parameters);
#endif

    return length;
}

static void
php_hoedown_set_renderer(zval *renders, hoedown_renderer *renderer,
                         char *name)
{
    char *error = NULL;
    zend_bool retval;

#ifdef ZEND_ENGINE_3
    zval *definition = NULL;

    if (!name || !renders) {
        HOEDOWN_ERR(E_WARNING, "Undefined renders");
        return;
    }

    definition = zend_hash_str_find(Z_ARRVAL_P(renders), name, strlen(name));
    if (!definition) {
        HOEDOWN_ERR(E_WARNING, "Undefined render '%s'", name);
        return;
    }

    retval = zend_is_callable_ex(definition, NULL, 0, NULL, NULL, &error);

#else
    zval **definition = NULL;

    if (!name || !renders || Z_TYPE_P(renders) != IS_ARRAY) {
        return;
    }

    if (zend_hash_find(Z_ARRVAL_P(renders), name, strlen(name)+1,
                       (void **)&definition) == FAILURE || !definition) {
        return;
    }

    retval = zend_is_callable_ex(*definition, NULL, 0, NULL, NULL, NULL,
                                 &error);
#endif
    if (error) {
        efree(error);
    }
    if (!retval) {
        HOEDOWN_ERR(E_WARNING, "Call to undefined render '%s'", name);
        /*
        zend_throw_exception_ex(NULL, 0,
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
    } else if (strcmp(name, "tableheader") == 0) {
        renderer->table_header = php_hoedown_renderer_table_header;
    } else if (strcmp(name, "tablebody") == 0) {
        renderer->table_body = php_hoedown_renderer_table_body;
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
    } else if (strcmp(name, "rawhtml") == 0) {
        renderer->raw_html = php_hoedown_renderer_raw_html;
    } else if (strcmp(name, "tripleemphasis") == 0) {
        renderer->triple_emphasis = php_hoedown_renderer_triple_emphasis;
    } else if (strcmp(name, "strikethrough") == 0) {
        renderer->strikethrough = php_hoedown_renderer_strikethrough;
    } else if (strcmp(name, "superscript") == 0) {
        renderer->superscript = php_hoedown_renderer_superscript;
    } else if (strcmp(name, "footnoteref") == 0) {
        renderer->footnote_ref = php_hoedown_renderer_footnote_ref;
    } else if (strcmp(name, "math") == 0) {
        renderer->math = php_hoedown_renderer_math;
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
php_hoedown_user_block(zval *definition)
{
    char *error = NULL;
    zend_bool retval;

#ifdef ZEND_ENGINE_3
    if (!definition || Z_ISUNDEF(*definition)) {
        return 0;
    }

    retval = zend_is_callable_ex(definition, NULL, 0, NULL, NULL, &error);

#else
    if (!definition) {
        return 0;
    }

    retval = zend_is_callable_ex(definition, NULL, 0, NULL, NULL, NULL,
                                 &error);
#endif
    if (error) {
        efree(error);
    }
    if (!retval) {
        HOEDOWN_ERR(E_WARNING, "Call to undefined user_block");
        return 0;
    }
    return 1;
}

static int
php_hoedown_callback_meta_parse(uint8_t *text, size_t size,
                                zval *definition, zval **return_state)
{
    char *error = NULL;
    zval fname;
    int retval = 0;

#ifdef ZEND_ENGINE_3
    zval parameters;
    zval result, args[2];

    if (!definition || Z_ISUNDEF(*definition)) {
        return 0;
    }

    retval = zend_is_callable_ex(definition, NULL, 0, NULL, NULL, &error);
    if (error) {
        efree(error);
    }
    if (!retval) {
        HOEDOWN_ERR(E_WARNING, "Call to undefined meta_parse");
        return 0;
    }

    ZVAL_STRINGL(&fname, "call_user_func_array",
                 sizeof("call_user_func_array")-1);

    args[0] = *definition;
    array_init_size(&args[1], 1);
    add_next_index_stringl(&args[1], (char *)text, size);

    call_user_function(EG(function_table), NULL, &fname,
                          &result, 2, args);

    if (!Z_ISUNDEF(result)) {
        zend_string *key = zend_string_init("meta", sizeof("meta")-1, 0);

        if (Z_TYPE_P(*return_state) != IS_ARRAY) {
            array_init(*return_state);
        }

        zend_symtable_update(Z_ARRVAL_P(*return_state), key, &result);

        retval = 1;

        zend_string_release(key);
    }

    zval_dtor(&fname);
    zval_dtor(&args[1]);
    zval_dtor(&result);

#else
    zval *parameters;
    zval *result = NULL, **args[2];

    if (!definition) {
        return 0;
    }

    retval = zend_is_callable_ex(definition, NULL, 0, NULL, NULL, NULL,
                                 &error);
    if (error) {
        efree(error);
    }
    if (!retval) {
        HOEDOWN_ERR(E_WARNING, "Call to undefined meta_parse");
        return 0;
    }

    MAKE_STD_ZVAL(parameters);
    array_init_size(parameters, 1);
    add_next_index_stringl(parameters, (char *)text, size, 1);

    ZVAL_STRINGL(&fname, "call_user_func_array",
                 sizeof("call_user_func_array")-1, 0);

    args[0] = &definition;
    args[1] = &parameters;

    call_user_function(EG(function_table), NULL, &fname,
                          &result, 2, args);

    if (result) {
        if (Z_TYPE_PP(return_state) != IS_ARRAY) {
            array_init(*return_state);
        }

        zend_symtable_update(Z_ARRVAL_PP(return_state),
                             "meta", sizeof("meta"),
                             &result, sizeof(result), NULL);

        retval = 1;
    }

    zval_ptr_dtor(&parameters);
#endif

    return retval;
}


static int
php_hoedown_parse(zval *return_value, zval *return_state,
                  char *contents, size_t length,
                  php_hoedown_options_t *options)
{
    hoedown_buffer *buf, *meta;
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
    meta = hoedown_buffer_new(HOEDOWN_BUFFER_UNIT);
    if (!meta) {
        hoedown_buffer_free(buf);
        HOEDOWN_ERR(E_WARNING, "Couldn't allocate meta block buffer");
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
#ifdef ZEND_ENGINE_3
            if (!Z_ISUNDEF(options->toc.header)) {
                state->toc_data.header = Z_STRVAL_P(&options->toc.header);
            }
            if (!Z_ISUNDEF(options->toc.footer)) {
                state->toc_data.footer = Z_STRVAL_P(&options->toc.footer);
            }

            if (php_hoedown_user_block(&options->user_block)) {
#else
            if (options->toc.header) {
                state->toc_data.header = Z_STRVAL_P(options->toc.header);
            }
            if (options->toc.footer) {
                state->toc_data.footer = Z_STRVAL_P(options->toc.footer);
            }

            if (php_hoedown_user_block(options->user_block)) {
#endif
                document = hoedown_document_new(renderer,
                                                options->extension, 16,
                                                php_hoedown_callback_user_block,
                                                NULL);
            } else {
                document = hoedown_document_new(renderer,
                                                options->extension, 16,
                                                NULL, NULL);
            }

            hoedown_document_render(document, buf,
                                    (const uint8_t *)contents, length);

            hoedown_document_free(document);
            hoedown_html_renderer_free(renderer);

            if (options->renderer == HOEDOWN_OPT_RENDERER_TOC) {
#ifdef ZEND_ENGINE_3
                RETVAL_STRINGL((char *)buf->data, buf->size);
#else
                RETVAL_STRINGL((char *)buf->data, buf->size, 1);
#endif
                hoedown_buffer_free(buf);
                hoedown_buffer_free(meta);
                return 0;
            } else {
#ifdef ZEND_ENGINE_3
                zval zv;
                zend_string *key = zend_string_init("toc", sizeof("toc")-1, 0);

                ZVAL_STRINGL(&zv, (char *)buf->data, buf->size);

                if (Z_TYPE_P(return_state) != IS_ARRAY) {
                    array_init(return_state);
                }

                zend_symtable_update(Z_ARRVAL_P(return_state), key, &zv);

                zend_string_release(key);
#else
                zval *zv;

                MAKE_STD_ZVAL(zv);
                ZVAL_STRINGL(zv, (char *)buf->data, buf->size, 1);

                if (Z_TYPE_P(return_state) != IS_ARRAY) {
                    array_init(return_state);
                }

                zend_symtable_update(Z_ARRVAL_P(return_state),
                                     "toc", sizeof("toc"),
                                     &zv, sizeof(zv), NULL);
#endif
                hoedown_buffer_reset(buf);
            }
        }
    }

    /* init renderer */
    renderer = hoedown_html_renderer_new(options->html, toc_level);

    /* override render */
#ifdef ZEND_ENGINE_3
    if (zend_hash_num_elements(HASH_OF(&options->renders))) {
        zval *value;
        zend_string *key;

        ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(&options->renders),
                                      key, value) {
            if (key) {
                php_hoedown_set_renderer(&options->renders, renderer,
                                         ZSTR_VAL(key));
            }
        } ZEND_HASH_FOREACH_END();
    }
#else
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
                                     str_key);
        } while (!zend_hash_move_forward_ex(h, &pos));
    }
#endif

    /* setting state */
    state = (hoedown_html_renderer_state *)renderer->opaque;
    state->opaque = (void *)options;

    /* init document */
#ifdef ZEND_ENGINE_3
    if (php_hoedown_user_block(&options->user_block)) {
#else
    if (php_hoedown_user_block(options->user_block)) {
#endif
        document = hoedown_document_new(renderer,
                                        options->extension, 16,
                                        php_hoedown_callback_user_block, meta);
    } else {
        document = hoedown_document_new(renderer,
                                        options->extension, 16,
                                        NULL, meta);
    }

    /* execute parse */
    hoedown_document_render(document, buf,
                            (const uint8_t *)contents, length);

    /* cleanup */
    hoedown_document_free(document);
    hoedown_html_renderer_free(renderer);

    /* setting return value */
#ifdef ZEND_ENGINE_3
    RETVAL_STRINGL((char *)buf->data, buf->size);
#else
    RETVAL_STRINGL((char *)buf->data, buf->size, 1);
#endif

    /* meta block */
    if (options->extension & HOEDOWN_OPT_EXT_META_BLOCK &&
        return_state && meta->size > 0) {
#ifdef ZEND_ENGINE_3
        if (php_hoedown_callback_meta_parse(meta->data, meta->size,
                                            &options->meta_parse,
                                            &return_state) == 0) {
            zval zv;
            zend_string *key = zend_string_init("meta", sizeof("meta")-1, 0);

            ZVAL_STRINGL(&zv, (char *)meta->data, meta->size);

            if (Z_TYPE_P(return_state) != IS_ARRAY) {
                array_init(return_state);
            }

            zend_symtable_update(Z_ARRVAL_P(return_state), key,  &zv);

            zend_string_release(key);
        }
#else
        if (php_hoedown_callback_meta_parse(meta->data, meta->size,
                                            options->meta_parse,
                                            &return_state) == 0) {
            zval *zv;

            MAKE_STD_ZVAL(zv);
            ZVAL_STRINGL(zv, (char *)meta->data, meta->size, 1);

            if (Z_TYPE_P(return_state) != IS_ARRAY) {
                array_init(return_state);
            }

            zend_symtable_update(Z_ARRVAL_P(return_state),
                                 "meta", sizeof("meta"),
                                 &zv, sizeof(zv), NULL);
        }
#endif
    }

    /* cleanup buffer */
    hoedown_buffer_free(buf);
    hoedown_buffer_free(meta);

    return 0;
}

HOEDOWN_METHOD(parse)
{
    char *buf;
    zval *state = NULL;
    php_hoedown_t *intern;
#ifdef ZEND_ENGINE_3
    size_t buf_len;
#else
    int buf_len;
#endif

    if (zend_parse_parameters(ZEND_NUM_ARGS(),
#ifdef ZEND_ENGINE_3
                              "s|z/",
#else
                              "s|z",
#endif
                              &buf, &buf_len, &state) == FAILURE) {
        RETURN_FALSE;
    }

    if (buf_len == 0) {
        RETURN_FALSE;
    }

    HOEDOWN_OBJ(intern, getThis());

    if (state) {
#ifdef ZEND_ENGINE_3
        zval_ptr_dtor(state);
#endif
        ZVAL_NULL(state);
    }

    /* markdown parse */
    if (php_hoedown_parse(return_value, state, buf, buf_len,
                          &intern->options) != 0) {
        RETURN_FALSE;
    }
}

HOEDOWN_METHOD(parseFile)
{
    char *filename;
#ifdef ZEND_ENGINE_3
    size_t filename_len;
    zend_string *contents = NULL;
#else
    int filename_len;
    int len;
    char *contents = NULL;
#endif
    zval *state = NULL;
    long maxlen = PHP_STREAM_COPY_ALL;
    php_stream *stream;
    php_hoedown_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS(),
#ifdef ZEND_ENGINE_3
                              "s|z/",
#else
                              "s|z",
#endif
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

    if (state) {
#ifdef ZEND_ENGINE_3
        zval_ptr_dtor(state);
#endif
        ZVAL_NULL(state);
    }

#ifdef ZEND_ENGINE_3
    contents = php_stream_copy_to_mem(stream, maxlen, 0);
    if (contents) {
        if (ZSTR_LEN(contents) > 0) {
            /* markdown parse */
            if (php_hoedown_parse(return_value, state,
                                  ZSTR_VAL(contents), ZSTR_LEN(contents),
                                  &intern->options) != 0) {
                RETVAL_FALSE;
            }
        } else {
            RETVAL_EMPTY_STRING();
        }
        zend_string_release(contents);
    } else {
        RETVAL_FALSE;
    }
#else
    if ((len = php_stream_copy_to_mem(stream, &contents, maxlen, 0)) > 0) {
        /* markdown parse */
        if (php_hoedown_parse(return_value, state, contents, len,
                              &intern->options) != 0) {
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
#endif

    php_stream_close(stream);
}

HOEDOWN_METHOD(addRender)
{
    char *name;
#ifdef ZEND_ENGINE_3
    size_t name_len;
    zend_string *key;
#else
    int name_len;
#endif
    zval *definition;
    php_hoedown_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "sz",
                              &name, &name_len, &definition) == FAILURE) {
        RETURN_FALSE;
    }

    if (name_len == 0) {
        RETURN_FALSE;
    }

    HOEDOWN_OBJ(intern, getThis());

#ifdef ZEND_ENGINE_3
    Z_TRY_ADDREF_P(definition);

    key = zend_string_init(name, name_len, 0);

    zend_symtable_update(Z_ARRVAL_P(&intern->options.renders),
                         key, definition);

    zend_string_release(key);

#else
    if (!intern->options.renders) {
        MAKE_STD_ZVAL(intern->options.renders);
        array_init(intern->options.renders);
    }

    Z_ADDREF_P(definition);

    zend_symtable_update(Z_ARRVAL_P(intern->options.renders), name, name_len+1,
                         &definition, sizeof(zval*), NULL);

#endif

    RETVAL_TRUE;
}

HOEDOWN_METHOD(getRenders)
{
    php_hoedown_t *intern;

    if (zend_parse_parameters_none() == FAILURE) {
        RETURN_FALSE;
    }

    HOEDOWN_OBJ(intern, getThis());

#ifdef ZEND_ENGINE_3
    if (!zend_hash_num_elements(HASH_OF(&intern->options.renders))) {
        RETVAL_NULL();
    } else {
        RETVAL_ZVAL(&intern->options.renders, 1, 0);
    }
#else
    if (!intern->options.renders) {
        RETVAL_NULL();
    } else {
        RETVAL_ZVAL(intern->options.renders, 1, 0);
    }
#endif
}

HOEDOWN_METHOD(ofString)
{
    char *buf;
#ifdef ZEND_ENGINE_3
    size_t buf_len;
#else
    int buf_len;
#endif
    zval *opts = NULL, *state = NULL;
    php_hoedown_options_t options;

    if (zend_parse_parameters(ZEND_NUM_ARGS(),
#ifdef ZEND_ENGINE_3
                              "s|az/",
#else
                              "s|az",
#endif
                              &buf, &buf_len, &opts, &state) == FAILURE) {
        RETURN_FALSE;
    }

    if (buf_len == 0) {
        RETURN_FALSE;
    }

    /* setting options */
    php_hoedown_options_init(&options);
    if (opts) {
        php_hoedown_set_options(&options, opts);
    }

    if (state) {
#ifdef ZEND_ENGINE_3
        zval_ptr_dtor(state);
#endif
        ZVAL_NULL(state);
    }

    /* markdown parse */
    if (php_hoedown_parse(return_value, state, buf, buf_len,
                          &options) != 0) {
        RETVAL_FALSE;
    }

    php_hoedown_options_destroy(&options);
}

HOEDOWN_METHOD(ofFile)
{
    char *filename;
#ifdef ZEND_ENGINE_3
    size_t filename_len;
    zend_string *contents = NULL;
#else
    int filename_len;
    int len;
    char *contents = NULL;
#endif
    long maxlen = PHP_STREAM_COPY_ALL;
    php_stream *stream;
    zval *opts = NULL, *state = NULL;
    php_hoedown_options_t options;

    if (zend_parse_parameters(ZEND_NUM_ARGS(),
#ifdef ZEND_ENGINE_3
                              "s|az/",
#else
                              "s|az",
#endif
                              &filename, &filename_len,
                              &opts, &state) == FAILURE) {
        RETURN_FALSE;
    }

    if (filename_len == 0) {
        RETURN_FALSE;
    }

    /* setting options */
    php_hoedown_options_init(&options);
    if (opts) {
        php_hoedown_set_options(&options, opts);
    }

    /* read file */
    stream = php_stream_open_wrapper_ex(filename, "rb",
                                        REPORT_ERRORS, NULL, NULL);
    if (!stream) {
        php_hoedown_options_destroy(&options);
        RETURN_FALSE;
    }

    if (state) {
#ifdef ZEND_ENGINE_3
        zval_ptr_dtor(state);
#endif
        ZVAL_NULL(state);
    }

#ifdef ZEND_ENGINE_3
    contents = php_stream_copy_to_mem(stream, maxlen, 0);
    if (contents) {
        if (ZSTR_LEN(contents) > 0) {
            /* markdown parse */
            if (php_hoedown_parse(return_value, state,
                                  ZSTR_VAL(contents), ZSTR_LEN(contents),
                                  &options) != 0) {
                RETVAL_FALSE;
            }
        } else {
            RETVAL_EMPTY_STRING();
        }
        zend_string_release(contents);
    } else {
        RETVAL_FALSE;
    }
#else
    if ((len = php_stream_copy_to_mem(stream, &contents, maxlen, 0)) > 0) {
        /* markdown parse */
        if (php_hoedown_parse(return_value, state, contents, len,
                              &options) != 0) {
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
#endif

    php_stream_close(stream);
    php_hoedown_options_destroy(&options);
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

#ifdef ZEND_ENGINE_3
static void
php_hoedown_free_storage(zend_object *std)
{
    php_hoedown_t *intern;
    intern = (php_hoedown_t *)((char *)std - XtOffsetOf(php_hoedown_t, std));
    if (!intern) {
        return;
    }

    php_hoedown_options_destroy(&intern->options);

    zend_object_std_dtor(std);
}

static zend_object *
php_hoedown_new_ex(zend_class_entry *ce, php_hoedown_t **ptr)
{
    php_hoedown_t *intern;

    intern = ecalloc(1, sizeof(php_hoedown_t)+zend_object_properties_size(ce));
    if (ptr) {
        *ptr = intern;
    }

    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    rebuild_object_properties(&intern->std);

    intern->std.handlers = &php_hoedown_handlers;

    php_hoedown_options_init(&intern->options);

    return &intern->std;
}

static zend_object *
php_hoedown_new(zend_class_entry *ce)
{
    return php_hoedown_new_ex(ce, NULL);
}

#else
static void
php_hoedown_free_storage(void *object)
{
    php_hoedown_t *intern = (php_hoedown_t *)object;

    if (!intern) {
        return;
    }

    php_hoedown_options_destroy(&intern->options);

    zend_object_std_dtor(&intern->std);
    efree(object);
}

static zend_object_value
php_hoedown_new_ex(zend_class_entry *ce, php_hoedown_t **ptr)
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

    zend_object_std_init(&intern->std, ce);
#if ZEND_MODULE_API_NO >= 20100525
    object_properties_init(&intern->std, ce);
#else
    zend_hash_copy(intern->std.properties, &ce->default_properties,
                   (copy_ctor_func_t)zval_add_ref,
                   (void *)&tmp, sizeof(zval *));
#endif

    retval.handle = zend_objects_store_put(
        intern, (zend_objects_store_dtor_t)zend_objects_destroy_object,
        (zend_objects_free_object_storage_t)php_hoedown_free_storage,
        NULL);
    retval.handlers = &php_hoedown_handlers;

    php_hoedown_options_init(&intern->options);

    return retval;
}

static zend_object_value
php_hoedown_new(zend_class_entry *ce)
{
    return php_hoedown_new_ex(ce, NULL);
}
#endif

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

    php_hoedown_ce = zend_register_internal_class(&ce);
    if (php_hoedown_ce == NULL) {
        return FAILURE;
    }

    memcpy(&php_hoedown_handlers, zend_get_std_object_handlers(),
           sizeof(zend_object_handlers));

#ifdef ZEND_ENGINE_3
    php_hoedown_handlers.offset = XtOffsetOf(php_hoedown_t, std);
    php_hoedown_handlers.dtor_obj = zend_objects_destroy_object;
    php_hoedown_handlers.free_obj = php_hoedown_free_storage;
#endif
    php_hoedown_handlers.clone_obj = NULL;

    /* class constant */
    HOEDOWN_CONST_LONG(RENDERER_HTML, HOEDOWN_OPT_RENDERER_HTML);
    HOEDOWN_CONST_LONG(RENDERER_TOC, HOEDOWN_OPT_RENDERER_TOC);

    HOEDOWN_CONST_LONG(SKIP_HTML, HOEDOWN_OPT_HTML_SKIP_HTML);
    HOEDOWN_CONST_LONG(HARD_WRAP, HOEDOWN_OPT_HTML_HARD_WRAP);
    HOEDOWN_CONST_LONG(USE_XHTML, HOEDOWN_OPT_HTML_USE_XHTML);
    HOEDOWN_CONST_LONG(ESCAPE, HOEDOWN_OPT_HTML_ESCAPE);
    HOEDOWN_CONST_LONG(TASK_LIST, HOEDOWN_OPT_HTML_USE_TASK_LIST);
    HOEDOWN_CONST_LONG(LINE_CONTINUE, HOEDOWN_OPT_HTML_LINE_CONTINUE);
    HOEDOWN_CONST_LONG(HEADER_ID, HOEDOWN_OPT_HTML_HEADER_ID);
    HOEDOWN_CONST_LONG(FENCED_CODE_SCRIPT, HOEDOWN_OPT_HTML_FENCED_CODE_SCRIPT);

    HOEDOWN_CONST_LONG(TABLES, HOEDOWN_OPT_EXT_TABLES);
    HOEDOWN_CONST_LONG(FENCED_CODE, HOEDOWN_OPT_EXT_FENCED_CODE);
    HOEDOWN_CONST_LONG(FOOTNOTES, HOEDOWN_OPT_EXT_FOOTNOTES);
    HOEDOWN_CONST_LONG(AUTOLINK, HOEDOWN_OPT_EXT_AUTOLINK);
    HOEDOWN_CONST_LONG(STRIKETHROUGH, HOEDOWN_OPT_EXT_STRIKETHROUGH);
    HOEDOWN_CONST_LONG(UNDERLINE, HOEDOWN_OPT_EXT_UNDERLINE);
    HOEDOWN_CONST_LONG(HIGHLIGHT, HOEDOWN_OPT_EXT_HIGHLIGHT);
    HOEDOWN_CONST_LONG(QUOTE, HOEDOWN_OPT_EXT_QUOTE);
    HOEDOWN_CONST_LONG(SUPERSCRIPT, HOEDOWN_OPT_EXT_SUPERSCRIPT);
    HOEDOWN_CONST_LONG(MATH, HOEDOWN_OPT_EXT_MATH);
    HOEDOWN_CONST_LONG(NO_INTRA_EMPHASIS, HOEDOWN_OPT_EXT_NO_INTRA_EMPHASIS);
    HOEDOWN_CONST_LONG(SPACE_HEADERS, HOEDOWN_OPT_EXT_SPACE_HEADERS);
    HOEDOWN_CONST_LONG(MATH_EXPLICIT, HOEDOWN_OPT_EXT_MATH_EXPLICIT);
    HOEDOWN_CONST_LONG(DISABLE_INDENTED_CODE,
                       HOEDOWN_OPT_EXT_DISABLE_INDENTED_CODE);
    HOEDOWN_CONST_LONG(SPECIAL_ATTRIBUTE, HOEDOWN_OPT_EXT_SPECIAL_ATTRIBUTE);
    HOEDOWN_CONST_LONG(SCRIPT_TAGS, HOEDOWN_OPT_EXT_SCRIPT_TAGS);
    HOEDOWN_CONST_LONG(META_BLOCK, HOEDOWN_OPT_EXT_META_BLOCK);

    HOEDOWN_CONST_LONG(TOC, HOEDOWN_OPT_TOC);
    HOEDOWN_CONST_LONG(TOC_BEGIN, HOEDOWN_OPT_TOC_BEGIN);
    HOEDOWN_CONST_LONG(TOC_END, HOEDOWN_OPT_TOC_END);
    HOEDOWN_CONST_LONG(TOC_HEADER, HOEDOWN_OPT_TOC_HEADER);
    HOEDOWN_CONST_LONG(TOC_FOOTER, HOEDOWN_OPT_TOC_FOOTER);

    HOEDOWN_CONST_LONG(RENDERS, HOEDOWN_OPT_RENDERS);
    HOEDOWN_CONST_LONG(USER_BLOCK, HOEDOWN_OPT_USER_BLOCK);
    HOEDOWN_CONST_LONG(META_PARSE, HOEDOWN_OPT_META_PARSE);

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
