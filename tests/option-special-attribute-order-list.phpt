--TEST--
render order list of option Hoedown::SPECIAL_ATTRIBUTE
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
1. a
2. b
{@list .order}
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::SPECIAL_ATTRIBUTE;

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
<ol>
<li>a</li>
<li>b
{@list .order}</li>
</ol>
== enable ==
bool(true)
<ol class="order">
<li>a</li>
<li>b</li>
</ol>
== disable ==
bool(false)
<ol>
<li>a</li>
<li>b
{@list .order}</li>
</ol>
