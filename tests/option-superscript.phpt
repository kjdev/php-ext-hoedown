--TEST--
render of option Hoedown::SUPERSCRIPT
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
hoge^(fuga)
hoge ^fuga
hoge ^fu^ga
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::SUPERSCRIPT;

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
<p>hoge^(fuga)
hoge ^fuga
hoge ^fu^ga</p>
== enable ==
bool(true)
<p>hoge<sup>fuga</sup>
hoge <sup>fuga</sup>
hoge <sup>fu<sup>ga</sup></sup></p>
== disable ==
bool(false)
<p>hoge^(fuga)
hoge ^fuga
hoge ^fu^ga</p>
