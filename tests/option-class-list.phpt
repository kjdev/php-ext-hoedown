--TEST--
render of option Hoedown::CLASS_LIST
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
* a
* b
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::CLASS_LIST;

echo "== default ==\n";
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text);

echo "== set: list ==\n";
$hoedown->setOption($opt, "list");
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text);

echo "== set: NULL ==\n";
$hoedown->setOption($opt, NULL);
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text);

echo "== new set: list-1 ==\n";
$hoedown1 = new Hoedown;
$hoedown1->setOption($opt, "list-1");
var_dump($hoedown1->getOption($opt));
echo $hoedown1->parse($text);

--EXPECTF--
== default ==
NULL
<ul>
<li>a</li>
<li>b</li>
</ul>
== set: list ==
string(4) "list"
<ul class="list">
<li>a</li>
<li>b</li>
</ul>
== set: NULL ==
NULL
<ul>
<li>a</li>
<li>b</li>
</ul>
== new set: list-1 ==
string(6) "list-1"
<ul class="list-1">
<li>a</li>
<li>b</li>
</ul>
