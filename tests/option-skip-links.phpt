--TEST--
render of option Hoedown::SKIP_LINKS
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
Link: <a href="#">here</a>
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::SKIP_LINKS;

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
<p>Link: <a href="#">here</a></p>
== enable ==
bool(true)
<p>Link: here</p>
== disable ==
bool(false)
<p>Link: <a href="#">here</a></p>
