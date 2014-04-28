--TEST--
HTML render: superscript
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
ho^ge
EOT;

$hoedown = new Hoedown;
$hoedown->setOption(Hoedown::SUPERSCRIPT, true);

echo "== closure: superscript ==\n";
$hoedown->addRender('superscript', function($text) {
        return 'closure:'.$text;
    });

echo $hoedown->parse($text), "\n";

echo "== function: superscript ==\n";
function superscript($text) {
    return 'function:'.$text;
}

$hoedown->addRender('superscript', 'superscript');

echo $hoedown->parse($text), "\n";

echo "== static closure: superscript ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::SUPERSCRIPT => true,
                           Hoedown::RENDERS => array(
                               'superscript' => function($text) {
                                   return 'static-closure:'.$text;
                               }))), "\n";

echo "== static function: superscript ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::SUPERSCRIPT => true,
                           Hoedown::RENDERS => array(
                               'superscript' => 'superscript'))), "\n";

--EXPECTF--
== closure: superscript ==
<p>hoclosure:ge</p>

== function: superscript ==
<p>hofunction:ge</p>

== static closure: superscript ==
<p>hostatic-closure:ge</p>

== static function: superscript ==
<p>hofunction:ge</p>

