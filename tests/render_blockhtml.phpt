--TEST--
HTML render: blockhtml
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
<div>hoge</div>
EOT;

$hoedown = new Hoedown;

echo "== closure: blockhtml ==\n";
$hoedown->addRender('blockhtml', function($text) {
        return 'closure:'.$text;
    });

echo $hoedown->parse($text), "\n";

echo "== function: blockhtml ==\n";
function blockhtml($text) {
    return 'function:'.$text;
}

$hoedown->addRender('blockhtml', 'blockhtml');

echo $hoedown->parse($text), "\n";

echo "== static closure: blockhtml ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'blockhtml' => function($text) {
                                   return 'static-closure:'.$text;
                               }))), "\n";

echo "== static function: blockhtml ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'blockhtml' => 'blockhtml'))), "\n";

--EXPECTF--
== closure: blockhtml ==
closure:<div>hoge</div>

== function: blockhtml ==
function:<div>hoge</div>

== static closure: blockhtml ==
static-closure:<div>hoge</div>

== static function: blockhtml ==
function:<div>hoge</div>

