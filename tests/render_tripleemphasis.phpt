--TEST--
HTML render: tripleemphasis
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
___hoge___
EOT;

$hoedown = new Hoedown;

echo "== closure: tripleemphasis ==\n";
$hoedown->addRender('tripleemphasis', function($text) {
        return 'closure:'.$text;
    });

echo $hoedown->parse($text), "\n";

echo "== function: tripleemphasis ==\n";
function tripleemphasis($text) {
    return 'function:'.$text;
}

$hoedown->addRender('tripleemphasis', 'tripleemphasis');

echo $hoedown->parse($text), "\n";

echo "== static closure: tripleemphasis ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'tripleemphasis' => function($text) {
                                   return 'static-closure:'.$text;
                               }))), "\n";

echo "== static function: tripleemphasis ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'tripleemphasis' => 'tripleemphasis'))), "\n";

--EXPECTF--
== closure: tripleemphasis ==
<p>closure:hoge</p>

== function: tripleemphasis ==
<p>function:hoge</p>

== static closure: tripleemphasis ==
<p>static-closure:hoge</p>

== static function: tripleemphasis ==
<p>function:hoge</p>

