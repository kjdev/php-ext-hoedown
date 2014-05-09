--TEST--
render of option Hoedown::HEADER_ID
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
# test

## test hoge

### test
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::HEADER_ID;

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
<h1>test</h1>

<h2>test hoge</h2>

<h3>test</h3>
== enable ==
bool(true)
<h1 id="test">test</h1>

<h2 id="test-hoge">test hoge</h2>

<h3 id="test-1">test</h3>
== disable ==
bool(false)
<h1>test</h1>

<h2>test hoge</h2>

<h3>test</h3>
