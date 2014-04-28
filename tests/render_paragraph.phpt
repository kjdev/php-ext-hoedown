--TEST--
HTML render: paragraph
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
hoge
EOT;

$hoedown = new Hoedown;

echo "== closure: paragraph ==\n";
$hoedown->addRender('paragraph', function($text) {
        return 'closure:'.$text;
    });

echo $hoedown->parse($text), "\n";

echo "== function: paragraph ==\n";
function paragraph($text) {
    return 'function:'.$text;
}

$hoedown->addRender('paragraph', 'paragraph');

echo $hoedown->parse($text), "\n";

echo "== static closure: paragraph ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'paragraph' => function($text) {
                                   return 'static-closure:'.$text;
                               }))), "\n";

echo "== static function: paragraph ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'paragraph' => 'paragraph'))), "\n";

--EXPECTF--
== closure: paragraph ==
closure:hoge
== function: paragraph ==
function:hoge
== static closure: paragraph ==
static-closure:hoge
== static function: paragraph ==
function:hoge
