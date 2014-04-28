--TEST--
HTML render: underline
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
_hoge_
EOT;

$hoedown = new Hoedown;
$hoedown->setOption(Hoedown::UNDERLINE, true);

echo "== closure: underline ==\n";
$hoedown->addRender('underline', function($text) {
        return 'closure:'.$text;
    });

echo $hoedown->parse($text), "\n";

echo "== function: underline ==\n";
function underline($text) {
    return 'function:'.$text;
}

$hoedown->addRender('underline', 'underline');

echo $hoedown->parse($text), "\n";

echo "== static closure: underline ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::UNDERLINE => true,
                           Hoedown::RENDERS => array(
                               'underline' => function($text) {
                                   return 'static-closure:'.$text;
                               }))), "\n";

echo "== static function: underline ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::UNDERLINE => true,
                           Hoedown::RENDERS => array(
                               'underline' => 'underline'))), "\n";

--EXPECTF--
== closure: underline ==
<p>closure:hoge</p>

== function: underline ==
<p>function:hoge</p>

== static closure: underline ==
<p>static-closure:hoge</p>

== static function: underline ==
<p>function:hoge</p>

