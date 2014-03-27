--TEST--
render of option Hoedown::HARD_WRAP
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
hoge
foo
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::HARD_WRAP;

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
<p>hoge
foo</p>
== enable ==
bool(true)
<p>hoge<br>
foo</p>
== disable ==
bool(false)
<p>hoge
foo</p>
