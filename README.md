# PHP Extension for Hoedown

[![Build Status](https://secure.travis-ci.org/kjdev/php-ext-hoedown.png?branch=master)](http://travis-ci.org/kjdev/php-ext-hoedown)

This extension allows Hoedown.

## Build

```
% git clone --depth=1 https://github.com/kjdev/php-ext-hoedown.git
% git submodule init
% git submodule update
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
```

## Usage

```php
$hoedown = new Hoedown;
echo $hoedown->parse('markdown text');
```

## Runtime Configuration

Name                              | Default | Changeable
--------------------------------- | ------- | ----------
hoedown.disable\_default\_options | 0       | PHP\_INI\_ALL


* hoedown.disable\_default\_options

  It does not set the valid options for default of Hoedown class if you have to
  On.

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
Hoedown::RENDERER\_HTML          | bool   | TRUE    | Render HTML
Hoedown::RENDERER\_TOC           | bool   | FALSE   | Render the Table of Contents in HTML
Hoedown::SKIP\_HTML              | bool   | FALSE   | Strip all HTML tags
Hoedown::SKIP\_STYLE             | bool   | FALSE   | Stripg \<style\> tags
Hoedown::SKIP\_IMAGES            | bool   | FALSE   | Don't render images
Hoedown::SKIP\_LINKS             | bool   | FALSE   | Don't render links
Hoedown::SAFELINK                | bool   | FALSE   | Only allow links to safe protocols
Hoedown::TOC                     | bool   | FALSE   | Produce links to the Table of Contents
Hoedown::HARD\_WRAP              | bool   | FALSE   | Render each linebreak as \<br\>
Hoedown::USE\_XHTML              | bool   | FALSE   | Render XHTML
Hoedown::ESCAPE                  | bool   | FALSE   | Escaple all HTML
Hoedown::TASK\_LIST              | bool   | FALSE   | Render task lists
Hoedown::LINE\_CONTINUE          | bool   | FALSE   | Render line continue
Hoedown::TABLES                  | bool   | TRUE    | Parse PHP-Markdown style tables
Hoedown::FENCED\_CODE            | bool   | TRUE    | Parse fenced code blocks
Hoedown::FOOTNOTES               | bool   | FALSE   | Parse footnotes
Hoedown::AUTOLINK                | bool   | TRUE    | Automatically turn URLs into links
Hoedown::STRIKETHROUGH           | bool   | TRUE    | Parse `~~strikethrough~~` spans
Hoedown::UNDERLINE               | bool   | FALSE   | Parse `_underline_` instead of emphasis
Hoedown::HIGHLIGHT               | bool   | FALSE   | Parse `==hightlight==` spans
Hoedown::QUOTE                   | bool   | FALSE   | Render "quotes" as \<q\>
Hoedown::SUPERSCRIPT             | bool   | FALSE   | Parse super\^script
Hoedown::NO\_INTRA\_EMPHASIS     | bool   | TRUE    | Disable emphasis\_between\_words
Hoedown::SPACE\_HEADERS          | bool   | FALSE   | Requqire a space after '#' in headers
Hoedown::DISABLE\_INDENTED\_CODE | bool   | FALSE   | Don't parse indented code blocks
Hoedown::SPECIA\L\_ATTRIBUTE     | bool   | FALSE   | Parse special attributes
Hoedown::TOC\_BEGIN              | int    | 0       | Begin level for headers included in the TOC.
Hoedown::TOC\_END                | int    | 6       | End level for headers included in the TOC.
Hoedown::TOC\_ESCAPE             | bool   | TRUE    | Escape int the TOC
Hoedown::TOC\_HEADER             | string | ""      | Render header in the TOC
Hoedown::TOC\_FOOTER             | string | ""      | Render footer in the TOC
Hoedown::CLASS\_LIST             | string | ""      | Render class attribute in the list
Hoedown::CLASS\_ORDER\_LIST      | string | ""      | Render class attribute in the ordered list
Hoedown::CLASS\_TASK\_LIST       | string | ""      | Render class attribute in the task list

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
<h1 id="toc_header">header</h1>

<h2 id="toc_a">a</h2>

<h2 id="toc_b">b</h2>
-- Table of Contents --
<ul>
<li>
<a href="#toc_header">header</a>
<ul>
<li>
<a href="#toc_a">a</a>
</li>
<li>
<a href="#toc_b">b</a>
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
<a href="#toc_header">header</a>
<ul>
<li>
<a href="#toc_a">a</a>
</li>
<li>
<a href="#toc_b">b</a>
</li>
</ul>
</li>
</ul>
```
