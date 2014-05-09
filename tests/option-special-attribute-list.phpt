--TEST--
render list of option Hoedown::SPECIAL_ATTRIBUTE
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
* a
* b
{@list .list}
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
<ul>
<li>a</li>
<li>b
{@list .list}</li>
</ul>
== enable ==
bool(true)
<ul class="list">
<li>a</li>
<li>b</li>
</ul>
== disable ==
bool(false)
<ul>
<li>a</li>
<li>b
{@list .list}</li>
</ul>
