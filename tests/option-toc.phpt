--TEST--
render of option Hoedown::TOC
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
# a
## b
### c
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::TOC;

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
<h1>a</h1>

<h2>b</h2>

<h3>c</h3>
== enable ==
bool(true)
<h1 id="toc_a">a</h1>

<h2 id="toc_b">b</h2>

<h3 id="toc_c">c</h3>
== disable ==
bool(false)
<h1>a</h1>

<h2>b</h2>

<h3>c</h3>
