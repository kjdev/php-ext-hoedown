--TEST--
render of option Hoedown::CLASS_ORDER_LIST
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
1. a
2. b
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::CLASS_ORDER_LIST;

echo "== default ==\n";
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text);

echo "== set: order ==\n";
$hoedown->setOption($opt, "order");
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text);

echo "== set: NULL ==\n";
$hoedown->setOption($opt, NULL);
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text);

echo "== new set: order-1 ==\n";
$hoedown1 = new Hoedown;
$hoedown1->setOption($opt, "order-1");
var_dump($hoedown1->getOption($opt));
echo $hoedown1->parse($text);

--EXPECTF--
== default ==
NULL
<ol>
<li>a</li>
<li>b</li>
</ol>
== set: order ==
string(5) "order"
<ol class="order">
<li>a</li>
<li>b</li>
</ol>
== set: NULL ==
NULL
<ol>
<li>a</li>
<li>b</li>
</ol>
== new set: order-1 ==
string(7) "order-1"
<ol class="order-1">
<li>a</li>
<li>b</li>
</ol>
