--TEST--
HTML render: normaltext
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
hoge
EOT;

$hoedown = new Hoedown;

echo "== closure: normaltext ==\n";
$hoedown->addRender('normaltext', function($text) {
        return 'closure:'.$text;
    });

echo $hoedown->parse($text), "\n";

echo "== function: normaltext ==\n";
function normaltext($text) {
    return 'function:'.$text;
}

$hoedown->addRender('normaltext', 'normaltext');

echo $hoedown->parse($text), "\n";

echo "== static closure: normaltext ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'normaltext' => function($text) {
                                   return 'static-closure:'.$text;
                               }))), "\n";

echo "== static function: normaltext ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'normaltext' => 'normaltext'))), "\n";

--EXPECTF--
== closure: normaltext ==
<p>closure:hoge</p>

== function: normaltext ==
<p>function:hoge</p>

== static closure: normaltext ==
<p>static-closure:hoge</p>

== static function: normaltext ==
<p>function:hoge</p>

