--TEST--
HTML render: header
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
# hoge
EOT;

$hoedown = new Hoedown;

echo "== closure: header ==\n";
$hoedown->addRender('header', function($text, $attr, $level) {
        return 'closure:'.$text.':'.$attr.':'.$level;
    });

echo $hoedown->parse($text), "\n";

echo "== function: header ==\n";
function _header($text, $attr, $level) {
    return 'function:'.$text.':'.$attr.':'.$level;
}

$hoedown->addRender('header', '_header');

echo $hoedown->parse($text), "\n";

echo "== static closure: header ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'header' => function($text, $attr, $level) {
                                   return 'static-closure:'.$text.':'.$attr.':'.$level;
                               }))), "\n";

echo "== static function: header ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'header' => '_header'))), "\n";

--EXPECTF--
== closure: header ==
closure:hoge::1
== function: header ==
function:hoge::1
== static closure: header ==
static-closure:hoge::1
== static function: header ==
function:hoge::1
