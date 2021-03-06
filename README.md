# PHP Extension for Hoedown

[![Build Status](https://secure.travis-ci.org/kjdev/php-ext-hoedown.png?branch=master)](http://travis-ci.org/kjdev/php-ext-hoedown)

This extension allows Hoedown.

## Requirement

* PHP 5.3 +

## Build

```
% git clone --recursive --depth=1 https://github.com/kjdev/php-ext-hoedown.git
% cd php-ext-hoedown
% phpize
% ./configure
% make
% make test
% make install
```

## Configuration

hoedown.ini:

```
extension=hoedown.so
; hoedown.options=tables,fenced-code,autolink,strikethrough,no-intra-emphasis
```

## Usage

```php
$hoedown = new Hoedown;
echo $hoedown->parse('markdown text');
```

## Runtime Configuration

Name            | Default                                                     | Changeable
--------------- | ----------------------------------------------------------- | ----------
hoedown.options | tables,fenced-code,autolink,strikethrough,no-intra-emphasis | PHP\_INI\_ALL


* hoedown.options

  Set the options for default of Hoedown class.
  (set in the constructor before calling)

  * Hoedown::TABLES
  * Hoedown::FENCED\_CODE
  * Hoedown::AUTOLINK
  * Hoedown::STRIKETHROUGH
  * Hoedown::NO\_INTRA\_EMPHASIS

## Class synopsis

```
Hoedown {
  public __construct(array $options = [])
  public mixed getOption(int $option)
  public bool setOption(int $option, mixed $value)
  public void setOptions(array $options = [])
  public bool addRender(string $name, callable $callback)
  public array getRenders(void)
  public string parse(string $string, mixed &$state = NULL)
  public string parseString(string $string, mixed &$state = NULL)
  public string parseFile(string $filename, mixed &$state = NULL)
  static public string ofString(string $string, array $options = [], mixed &$state = NULL)
  static public string ofFile(string $filename, array $options = [], mixed &$state = NULL)
}
```

### Predefined Constants

Name                             | Type   | Default | Description
-------------------------------- | -----  | ------- | -----------
Hoedown::RENDERER\_HTML          | bool   | TRUE    | Render HTML.
Hoedown::RENDERER\_TOC           | bool   | FALSE   | Render the Table of Contents in HTML.
Hoedown::SKIP\_HTML              | bool   | FALSE   | Strip all HTML tags.
Hoedown::HARD\_WRAP              | bool   | FALSE   | Render each linebreak as \<br\>.
Hoedown::USE\_XHTML              | bool   | FALSE   | Render XHTML.
Hoedown::ESCAPE                  | bool   | FALSE   | Escaple all HTML.
Hoedown::USE\_TASK\_LIST         | bool   | FALSE   | Render task lists.
Hoedown::LINE\_CONTINUE          | bool   | FALSE   | Render line continue.
Hoedown::HEADER\_ID              | bool   | FALSE   | Render header id.
Hoedown::FENCED\_CODE\_SCRIPT    | bool   | FALSE   | Render script of fenced code blocks style.
Hoedown::TABLES                  | bool   | TRUE    | Parse PHP-Markdown style tables.
Hoedown::FENCED\_CODE            | bool   | TRUE    | Parse fenced code blocks.
Hoedown::FOOTNOTES               | bool   | FALSE   | Parse footnotes.
Hoedown::AUTOLINK                | bool   | TRUE    | Automatically turn URLs into links.
Hoedown::STRIKETHROUGH           | bool   | TRUE    | Parse `~~strikethrough~~` spans.
Hoedown::UNDERLINE               | bool   | FALSE   | Parse `_underline_` instead of emphasis.
Hoedown::HIGHLIGHT               | bool   | FALSE   | Parse `==hightlight==` spans.
Hoedown::QUOTE                   | bool   | FALSE   | Render "quotes" as \<q\>.
Hoedown::SUPERSCRIPT             | bool   | FALSE   | Parse super\^script.
Hoedown::NO\_INTRA\_EMPHASIS     | bool   | TRUE    | Disable emphasis\_between\_words.
Hoedown::SPACE\_HEADERS          | bool   | FALSE   | Requqire a space after '#' in headers.
Hoedown::DISABLE\_INDENTED\_CODE | bool   | FALSE   | Don't parse indented code blocks.
Hoedown::SPECIAL\_ATTRIBUTE      | bool   | FALSE   | Parse special attributes.
Hoedown::SCRIPT\_TAGS            | bool   | FALSE   | Parse script tags `<?..?>`.
Hoedown::META\_BLOCK             | bool   | FALSE   | Parse meta block `<!--*..*-->`.
Hoedown::TOC                     | bool   | FALSE   | Produce links to the Table of Contents.
Hoedown::TOC\_BEGIN              | int    | 0       | Begin level for headers included in the TOC.
Hoedown::TOC\_END                | int    | 6       | End level for headers included in the TOC.
Hoedown::TOC\_HEADER             | string | ""      | Render header in the TOC.
Hoedown::TOC\_FOOTER             | string | ""      | Render footer in the TOC.

### Methods

* [Hoedown::\_\_construct](#hoedown__construct) - Create a Hoedown instance
* [Hoedown::getOption](#hoedowngetoption) - Retrieve a Hoedown option value
* [Hoedown::setOption](#hoedownsetoption) - Set Hoedown option
* [Hoedown::setOptions](#hoedownsetoptions) - Set Hoedown options
* [Hoedown::parse](#hoedownparse) - retrieve html by parse string as markdown
* [Hoedown::parseString](#hoedownparsestring) - retrieve html by parse string as markdown
* [Hoedown::parseFile](#hoedownparsefile) - retrieve html by parse file as markdown
* [Hoedown::ofString](#hoedownofstring) - retrieve html by parse string as markdown
* [Hoedown::ofFile](#hoedownoffile) - retrieve html by parse file as markdown
* [Hoedown::addRender](#hoedownaddrender) - Set Hoedown renderer function
* [Hoedown::getRenders](#hoedowngetrenders) - retrieve renderer function

---

### Hoedown::\_\_construct

```php
public __construct(array $options = [])
```

Create a Hoedown instance.

**Parameters:**

* options

  An associative array of options where the key is the option to set and the
  value is the new value for the option.

**Return Values:**

Returns a new Hoedown object

---

### Hoedown::getOption

```php
public mixed getOption(int $option)
```

Retrieve a Hoedown option value.

**Parameters:**

* option

  One of the Hoedown::* constants.

**Return Values:**

Returns the value of the requested option, or FALSE on error.

---

### Hoedown::setOption

```php
public bool setOption(int $option, mixed $value)
```

Set Hoedown option.

**Parameters:**

* option

  One of the Hoedown::* constants.

* value

  Set option value.

**Return Values:**

Returns TRUE on success or FALSE on failure.

---

### Hoedown::setOptions

```php
public void setOptions(array $options = [])
```

Set Hoedown options.

**Parameters:**

* options

  An associative array of options where the key is the option to set and the
  value is the new value for the option.

---

### Hoedown::parse

```php
public string parse(string $string, mixed &$state = NULL)
```

retrieve html by parse text as markdown.

**Parameters:**

* string

  Markdown text string.

* state

  Returns the value of extend parse.

**Return Values:**

Returns the retrieve html, or FALSE on error.

---

### Hoedown::parseString

```php
public string parseString(string $string, mixed &$state = NULL)
```

retrieve html by parse string as markdown.

alias: [Hoedown::parse](#hoedownparse)

---

### Hoedown::parseFile

```php
public string parseFile(string $filename, mixed &$state = NULL)
```

retrieve html by parse file as markdown.

**Parameters:**

* filename

  Markdown file name.

* state

  Returns the value of extend parse.

**Return Values:**

Returns the retrieve html, or FALSE on error.

---

### Hoedown::ofString

```php
static public string ofString(string $string, array $options = [], mixed &$state = NULL)
```

retrieve html by parse string as markdown (static method).

**Parameters:**

* string

  Markdown text string.

* options

  An associative array of options where the key is the option to set and the
  value is the new value for the option.

* state

  Returns the value of extend parse.

**Return Values:**

Returns the retrieve html, or FALSE on error.

---

### Hoedown::ofFile

```php
static public string ofFile(string $filename, array $options = [], mixed &$state = NULL)
```

retrieve html by parse file as markdown (static method).

**Parameters:**

* filename

  Markdown file name.

* options

  An associative array of options where the key is the option to set and the
  value is the new value for the option.

* state

  Returns the value of extend parse.

**Return Values:**

Returns the retrieve html, or FALSE on error.

---

### Hoedown::addRender

```php
public bool addRender(string $name, callable $callback)
```

Set Hoedown renderer function.

**Parameters:**

* name

  Hoedown rederer function name.

* callback

  The callable to be called.

**Return Values:**

Returns TRUE on success or FALSE on failure.

---

### Hoedown::getRenders

```php
public array getRenders(void)
```

retrieve renderer function.

**Return Values:**

Returns the retrieve renderer functions, or NULL.


## Examples

* Setting a Hoedown option

```php
$hoedown = new Hoedown;
$hoedown->setOption(Hoedown::USE_XHTML, true);
$hoedown->setOption(Hoedown::HARD_WRAP, true);
// or $hoedown->setOptions([Hoedown::USE_XHTML => true, Hoedown::HARD_WRAP => true]);
// or new Hoedown([Hoedown::USE_XHTML => true, Hoedown::HARD_WRAP => true]);
echo $hoedown->parse("markdown\ntext");
```

The above example will output:

```
<p>markdown<br/>
text</p>
```

* Retrieve the Table of Contents

```php
$hoedown = new Hoedown;
$hoedown->setOption(Hoedown::TOC, true);
echo $hoedown->parse("# header\n##a\n##b", $state);
echo "-- Table of Contents --\n";
echo $state['toc'];
```

The above example will output:

```
<h1 id="header">header</h1>

<h2 id="a">a</h2>

<h2 id="b">b</h2>
-- Table of Contents --
<ul>
<li>
<a href="#header">header</a>
<ul>
<li>
<a href="#a">a</a>
</li>
<li>
<a href="#b">b</a>
</li>
</ul>
</li>
</ul>
```

Only retrieve the Table of Contents.

```php
$hoedown = new Hoedown;
$hoedown->setOption(Hoedown::TOC, true);
$hoedown->setOption(Hoedown::RENDERER_TOC, true);
echo $hoedown->parse("# header\n##a\n##b");
```

The above example will output:

```
<ul>
<li>
<a href="#header">header</a>
<ul>
<li>
<a href="#a">a</a>
</li>
<li>
<a href="#b">b</a>
</li>
</ul>
</li>
</ul>
```

## HTML render

```php
$hoedown = new Hoedown;

$hoedown->addRender('blockcode', function($text, $lang, $attr) {
        // Use Pygmentize
        return Pygmentize::highlight($text, $lang);
    });

// or
// function blockCode($text, $lang, $attr) {
//     return Pygmentize::highlight($text, $lang);
// }
// $hoedown->addRender('blockcode', 'blockCode');

// or
// $hoedown->setOption(Hoedown::RENDERS, [
//                         'blockcode' => function($text, $lang, $attr) {
//                             return Pygmentize::highlight($text, $lang);
//                         }]);

echo $hoedown->parse("...markdown string...");
```

Render functions:

* blockcode($text, $lang, $attr)
* blockquote($text)
* blockhtml($text)
* paragraph($text)
* header($text, $attr, $level)
* hrule()
* list($text, $attr, $flags)
* listitem($text, $attr, $flags)
* table($text, $attr)
* tableheader($text)
* tablebody($text)
* tablerow($text)
* tablecell($text, $flags)
* footnotes($text)
* footnotedef($text, $num)
* footnoteref($num)
* codespan($text, $attr)
* underline($text)
* highlight($text)
* quote($text)
* strikethrough($text)
* superscript($text)
* emphasis($text)
* doubleemphasis($text)
* tripleemphasis($text)
* autolink($link, $type)
* image($link, $title, $alt, $attr)
* link($content, $link, $title, $attr)
* linebreak()
* rawhtmltag($tag)
* entity($entity)
* normaltext($text)
* userblock($text)

## User block

simple php code.

```php
$hoedown = new Hoedown;

$hoedown->setOption(Hoedown::USER_BLOCK, function($text) {
        // Returns the value of user block length
        if (preg_match('/^<\?php.*\?>/is', $text, $matches)) {
            return strlen($matches[0]);
        }
        return 0;
    });

echo $hoedown->parse("<?php echo 'test'; ?>"), PHP_EOL;

$hoedown->setOption(Hoedown::RENDERS, ['userblock' => function($text) {
            ob_start();
            eval(substr($text, 5, -2));
            $retval = ob_get_contents();
            ob_end_clean();
            return $retval;
        }]);

echo $hoedown->parse("<?php echo 'test'; ?>"), PHP_EOL;
```

output:

```
<?php echo "test"; ?>
test
```

## Meta block

Add the `Hoedown::META_BLOCK` to options.

Get a meta block by running in the following program.

```php
$text = <<<EOT
<!--*
  author: user
  title: Welcom to use
  tags: [ markdown, metadata ]
*-->
test
EOT;

$hoedown = new Hoedown;

$hoedown->setOption(Hoedown::META_BLOCK, true);

echo $hoedown->parse($text, $meta), PHP_EOL;

var_dump($meta);
```

output:

```
<p>test</p>

array(1) {
  ["meta"]=>
  string(69) "  author: user
  title: Welcom to use
  tags: [ markdown, metadata ]
"
}
```

* Parse meta block

Set the `Hoedown::META_PARSE` function to options.

```php
$text = <<<EOT
<!--*
  author: user
  title: Welcom to use
  tags: [ markdown, metadata ]
*-->
test
EOT;

$hoedown = new Hoedown;

$hoedown->setOption(Hoedown::META_BLOCK, true);
$hoedown->setOption(Hoedown::META_PARSE, function($text) {
    return yaml_parse($text);
});

echo $hoedown->parse($text, $meta), PHP_EOL;

var_dump($meta);
```

output:

```
<p>test</p>

array(1) {
  ["meta"]=>
  array(3) {
    ["author"]=>
    string(4) "user"
    ["title"]=>
    string(13) "Welcom to use"
    ["tags"]=>
    array(2) {
      [0]=>
      string(8) "markdown"
      [1]=>
      string(8) "metadata"
    }
  }
}
```
