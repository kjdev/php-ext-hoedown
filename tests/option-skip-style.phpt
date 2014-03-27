--TEST--
render of option Hoedown::SKIP_STYLE
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
hoge<style>foo</style>
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::SKIP_STYLE;

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
<p>hoge<style>foo</style></p>
== enable ==
bool(true)
<p>hogefoo</p>
== disable ==
bool(false)
<p>hoge<style>foo</style></p>
