--TEST--
HTML render: highlight
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
==hoge==
EOT;

$hoedown = new Hoedown;
$hoedown->setOption(Hoedown::HIGHLIGHT, true);

echo "== closure: highlight ==\n";
$hoedown->addRender('highlight', function($text) {
        return 'closure:'.$text;
    });

echo $hoedown->parse($text), "\n";

echo "== function: highlight ==\n";
function highlight($text) {
    return 'function:'.$text;
}

$hoedown->addRender('highlight', 'highlight');

echo $hoedown->parse($text), "\n";

echo "== static closure: highlight ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::HIGHLIGHT => true,
                           Hoedown::RENDERS => array(
                               'highlight' => function($text) {
                                   return 'static-closure:'.$text;
                               }))), "\n";

echo "== static function: highlight ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::HIGHLIGHT => true,
                           Hoedown::RENDERS => array(
                               'highlight' => 'highlight'))), "\n";

--EXPECTF--
== closure: highlight ==
<p>closure:hoge</p>

== function: highlight ==
<p>function:hoge</p>

== static closure: highlight ==
<p>static-closure:hoge</p>

== static function: highlight ==
<p>function:hoge</p>

