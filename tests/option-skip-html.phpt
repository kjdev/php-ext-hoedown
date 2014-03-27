--TEST--
render of option Hoedown::SKIP_HTML
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
hoge<span>foo</span>
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::SKIP_HTML;

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
<p>hoge<span>foo</span></p>
== enable ==
bool(true)
<p>hogefoo</p>
== disable ==
bool(false)
<p>hoge<span>foo</span></p>
