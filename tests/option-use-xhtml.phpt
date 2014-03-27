--TEST--
render of option Hoedown::USE_XHTML
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
---
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::USE_XHTML;

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
<hr>
== enable ==
bool(true)
<hr/>
== disable ==
bool(false)
<hr>
