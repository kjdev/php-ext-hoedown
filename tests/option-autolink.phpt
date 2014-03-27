--TEST--
render of option Hoedown::AUTOLINK
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
http://www.php.net/
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::AUTOLINK;

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
<p><a href="http://www.php.net/">http://www.php.net/</a></p>
== enable ==
bool(true)
<p><a href="http://www.php.net/">http://www.php.net/</a></p>
== disable ==
bool(false)
<p>http://www.php.net/</p>
