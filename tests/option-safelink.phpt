--TEST--
render of option Hoedown::SAFELINK
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
[web](http://www.php.net)
[file](file:///local.file)
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::SAFELINK;

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
<p><a href="http://www.php.net">web</a>
<a href="file:///local.file">file</a></p>
== enable ==
bool(true)
<p><a href="http://www.php.net">web</a>
[file](file:///local.file)</p>
== disable ==
bool(false)
<p><a href="http://www.php.net">web</a>
<a href="file:///local.file">file</a></p>
