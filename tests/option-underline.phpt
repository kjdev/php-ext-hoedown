--TEST--
render of option Hoedown::UNDERLINE
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
this is _good_ bad.
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::UNDERLINE;

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
<p>this is <em>good</em> bad.</p>
== enable ==
bool(true)
<p>this is <u>good</u> bad.</p>
== disable ==
bool(false)
<p>this is <em>good</em> bad.</p>
