--TEST--
render of option Hoedown::TOC_END
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
# a
## b
### c
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::TOC_END;

echo "== default ==\n";
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text);

echo "== enable: 1 ==\n";
$hoedown->setOption($opt, 1);
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text);

echo "== enable: 2 ==\n";
$hoedown->setOption($opt, 2);
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text);

echo "== disable ==\n";
$hoedown->setOption($opt, 0);
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text);

--EXPECTF--
== default ==
int(0)
<h1>a</h1>

<h2>b</h2>

<h3>c</h3>
== enable: 1 ==
int(1)
<h1 id="a">a</h1>

<h2>b</h2>

<h3>c</h3>
== enable: 2 ==
int(2)
<h1 id="a">a</h1>

<h2 id="b">b</h2>

<h3>c</h3>
== disable ==
int(0)
<h1>a</h1>

<h2>b</h2>

<h3>c</h3>
