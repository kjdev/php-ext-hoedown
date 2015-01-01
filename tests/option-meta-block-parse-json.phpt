--TEST--
render of option Hoedown::META_BLOCK and parse json
--SKIPIF--
<?php
if (!extension_loaded('json')) {
    die("skip this test is for json support enabled");
}
?>
--FILE--
<?php
$text = <<<EOT
<!--*
{
  "author": "chun",
  "title": "Welcom to use",
  "tags": [ "markdown", "metadata" ]
}
*-->

# Header

hoge
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::META_BLOCK;

$hoedown->setOption(Hoedown::META_PARSE, function($text) {
    return json_decode($text, true);
});

echo "== default ==\n";
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text, $meta);
var_dump($meta);

echo "== enable ==\n";
$hoedown->setOption($opt, true);
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text, $meta);
var_dump($meta);

echo "== disable ==\n";
$hoedown->setOption($opt, false);
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text, $meta);
var_dump($meta);

--EXPECTF--
== default ==
bool(false)
<!--*
{
  "author": "chun",
  "title": "Welcom to use",
  "tags": [ "markdown", "metadata" ]
}
*-->

<h1>Header</h1>

<p>hoge</p>
NULL
== enable ==
bool(true)
<h1>Header</h1>

<p>hoge</p>
array(1) {
  ["meta"]=>
  array(3) {
    ["author"]=>
    string(4) "chun"
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
== disable ==
bool(false)
<!--*
{
  "author": "chun",
  "title": "Welcom to use",
  "tags": [ "markdown", "metadata" ]
}
*-->

<h1>Header</h1>

<p>hoge</p>
NULL
