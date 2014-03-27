--TEST--
render of option Hoedown::HIGHLIGHT
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
this is ==good== bad.
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::HIGHLIGHT;

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
<p>this is ==good== bad.</p>
== enable ==
bool(true)
<p>this is <mark>good</mark> bad.</p>
== disable ==
bool(false)
<p>this is ==good== bad.</p>