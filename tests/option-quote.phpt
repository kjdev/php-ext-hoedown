--TEST--
render of option Hoedown::QUOTE
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
this is "good" bad.
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::QUOTE;

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
<p>this is &quot;good&quot; bad.</p>
== enable ==
bool(true)
<p>this is <q>good</q> bad.</p>
== disable ==
bool(false)
<p>this is &quot;good&quot; bad.</p>
