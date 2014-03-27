--TEST--
render of option Hoedown::SPACE_HEADERS
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
# hoge
#foo
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::SPACE_HEADERS;

echo "== default ==\n";
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text);

echo "== enable ==\n";
$hoedown->setOption($opt, true);
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text);

echo "== disable ==\n";
$hoedown->setOption($opt, false);
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text);

--EXPECTF--
== default ==
bool(false)
<h1>hoge</h1>

<h1>foo</h1>
== enable ==
bool(true)
<h1>hoge</h1>

<p>#foo</p>
== disable ==
bool(false)
<h1>hoge</h1>

<h1>foo</h1>