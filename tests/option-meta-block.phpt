--TEST--
render of option Hoedown::META_BLOCK
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
<!--*
  author: chun
  title: Welcom to use
  tags: [ markdown, metadata ]
*-->

# Header

hoge
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::META_BLOCK;

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
  author: chun
  title: Welcom to use
  tags: [ markdown, metadata ]
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
  string(69) "  author: chun
  title: Welcom to use
  tags: [ markdown, metadata ]
"
}
== disable ==
bool(false)
<!--*
  author: chun
  title: Welcom to use
  tags: [ markdown, metadata ]
*-->

<h1>Header</h1>

<p>hoge</p>
NULL
