--TEST--
HTML render: strikethrough
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
~~hoge~~
EOT;

$hoedown = new Hoedown;

echo "== closure: strikethrough ==\n";
$hoedown->addRender('strikethrough', function($text) {
        return 'closure:'.$text;
    });

echo $hoedown->parse($text), "\n";

echo "== function: strikethrough ==\n";
function strikethrough($text) {
    return 'function:'.$text;
}

$hoedown->addRender('strikethrough', 'strikethrough');

echo $hoedown->parse($text), "\n";

echo "== static closure: strikethrough ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'strikethrough' => function($text) {
                                   return 'static-closure:'.$text;
                               }))), "\n";

echo "== static function: strikethrough ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'strikethrough' => 'strikethrough'))), "\n";

--EXPECTF--
== closure: strikethrough ==
<p>closure:hoge</p>

== function: strikethrough ==
<p>function:hoge</p>

== static closure: strikethrough ==
<p>static-closure:hoge</p>

== static function: strikethrough ==
<p>function:hoge</p>

