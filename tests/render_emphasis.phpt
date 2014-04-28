--TEST--
HTML render: emphasis
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
_hoge_
EOT;

$hoedown = new Hoedown;

echo "== closure: emphasis ==\n";
$hoedown->addRender('emphasis', function($text) {
        return 'closure:'.$text;
    });

echo $hoedown->parse($text), "\n";

echo "== function: emphasis ==\n";
function emphasis($text) {
    return 'function:'.$text;
}

$hoedown->addRender('emphasis', 'emphasis');

echo $hoedown->parse($text), "\n";

echo "== static closure: emphasis ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'emphasis' => function($text) {
                                   return 'static-closure:'.$text;
                               }))), "\n";

echo "== static function: emphasis ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'emphasis' => 'emphasis'))), "\n";

--EXPECTF--
== closure: emphasis ==
<p>closure:hoge</p>

== function: emphasis ==
<p>function:hoge</p>

== static closure: emphasis ==
<p>static-closure:hoge</p>

== static function: emphasis ==
<p>function:hoge</p>

