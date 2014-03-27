--TEST--
render of option Hoedown::SKIP_IMAGES
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
Image: <img src="test.png">
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::SKIP_IMAGES;

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
<p>Image: <img src="test.png"></p>
== enable ==
bool(true)
<p>Image: </p>
== disable ==
bool(false)
<p>Image: <img src="test.png"></p>
