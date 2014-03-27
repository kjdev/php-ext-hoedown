--TEST--
render of option Hoedown::ESCAPE
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
<a href="#">here</a>
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::ESCAPE;

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
<p><a href="#">here</a></p>
== enable ==
bool(true)
<p>&lt;a href=&quot;#&quot;&gt;here&lt;/a&gt;</p>
== disable ==
bool(false)
<p><a href="#">here</a></p>
