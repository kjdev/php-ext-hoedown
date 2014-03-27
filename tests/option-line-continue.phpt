--TEST--
render of option Hoedown::LINE_CONTINUE
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
hoge
foo
huga
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::LINE_CONTINUE;

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
foo
huga</p>
== enable ==
bool(true)
<p>hoge foo huga</p>
== disable ==
bool(false)
<p>hoge
foo
huga</p>
