--TEST--
HTML render: doubleemphasis
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
__hoge__
EOT;

$hoedown = new Hoedown;

echo "== closure: doubleemphasis ==\n";
$hoedown->addRender('doubleemphasis', function($text) {
        return 'closure:'.$text;
    });

echo $hoedown->parse($text), "\n";

echo "== function: doubleemphasis ==\n";
function doubleemphasis($text) {
    return 'function:'.$text;
}

$hoedown->addRender('doubleemphasis', 'doubleemphasis');

echo $hoedown->parse($text), "\n";

echo "== static closure: doubleemphasis ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'doubleemphasis' => function($text) {
                                   return 'static-closure:'.$text;
                               }))), "\n";

echo "== static function: doubleemphasis ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'doubleemphasis' => 'doubleemphasis'))), "\n";

--EXPECTF--
== closure: doubleemphasis ==
<p>closure:hoge</p>

== function: doubleemphasis ==
<p>function:hoge</p>

== static closure: doubleemphasis ==
<p>static-closure:hoge</p>

== static function: doubleemphasis ==
<p>function:hoge</p>

