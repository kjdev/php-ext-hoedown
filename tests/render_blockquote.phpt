--TEST--
HTML render: blockquote
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
> hoge
EOT;

$hoedown = new Hoedown;

echo "== closure: blockquote ==\n";
$hoedown->addRender('blockquote', function($text) {
        return 'closure:'.$text;
    });

echo $hoedown->parse($text), "\n";

echo "== function: blockquote ==\n";
function blockquote($text) {
    return 'function:'.$text;
}

$hoedown->addRender('blockquote', 'blockquote');

echo $hoedown->parse($text), "\n";

echo "== static closure: blockquote ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'blockquote' => function($text) {
                                   return 'static-closure:'.$text;
                               }))), "\n";

echo "== static function: blockquote ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'blockquote' => 'blockquote'))), "\n";

--EXPECTF--
== closure: blockquote ==
closure:<p>hoge</p>

== function: blockquote ==
function:<p>hoge</p>

== static closure: blockquote ==
static-closure:<p>hoge</p>

== static function: blockquote ==
function:<p>hoge</p>

