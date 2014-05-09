--TEST--
HTML render: list, listitem
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
* a
EOT;

$hoedown = new Hoedown;

echo "== closure: list ==\n";
$hoedown->addRender('list', function($text, $attr, $flags) {
        return 'closure:'.$text;
    });

echo $hoedown->parse($text), "\n";

echo "== closure: listitem ==\n";
$hoedown->addRender('listitem', function($text, $attr, $flags) {
        return 'closure:'.$text.':'.$attr;
    });

echo $hoedown->parse($text), "\n";

echo "== function: list ==\n";
function _list($text, $attr, $flags) {
    return 'function:'.$text;
}

$hoedown->addRender('list', '_list');

echo $hoedown->parse($text), "\n";

echo "== function: listitem ==\n";
function _listitem($text, $attr, $flags) {
    return 'function:'.$text.':'.$attr;
}

$hoedown->addRender('listitem', '_listitem');

echo $hoedown->parse($text), "\n";

echo "== static closure: list ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'list' => function($text, $attr, $flags) {
                                   return 'static-closure:'.$text;
                               }))), "\n";

echo "== static function: listiteme ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'list' => function($text, $attr, $flags) {
                                   return 'static-closure:'.$text;
                               },
                               'listitem' => '_listitem'))), "\n";

--EXPECTF--
== closure: list ==
closure:<li>a</li>

== closure: listitem ==
closure:closure:a
:
== function: list ==
function:closure:a
:
== function: listitem ==
function:function:a
:
== static closure: list ==
static-closure:<li>a</li>

== static function: listiteme ==
static-closure:function:a
:
