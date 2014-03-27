--TEST--
render of option Hoedown::STRIKETHROUGH
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
this is ~~good~~ bad.
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::STRIKETHROUGH;

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
bool(true)
<p>this is <del>good</del> bad.</p>
== enable ==
bool(true)
<p>this is <del>good</del> bad.</p>
== disable ==
bool(false)
<p>this is ~~good~~ bad.</p>
