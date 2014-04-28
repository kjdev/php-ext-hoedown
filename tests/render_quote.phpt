--TEST--
HTML render: quote
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
"hoge"
EOT;

$hoedown = new Hoedown;
$hoedown->setOption(Hoedown::QUOTE, true);

echo "== closure: quote ==\n";
$hoedown->addRender('quote', function($text) {
        return 'closure:'.$text;
    });

echo $hoedown->parse($text), "\n";

echo "== function: quote ==\n";
function quote($text) {
    return 'function:'.$text;
}

$hoedown->addRender('quote', 'quote');

echo $hoedown->parse($text), "\n";

echo "== static closure: quote ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::QUOTE => true,
                           Hoedown::RENDERS => array(
                               'quote' => function($text) {
                                   return 'static-closure:'.$text;
                               }))), "\n";

echo "== static function: quote ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::QUOTE => true,
                           Hoedown::RENDERS => array(
                               'quote' => 'quote'))), "\n";

--EXPECTF--
== closure: quote ==
<p>closure:hoge</p>

== function: quote ==
<p>function:hoge</p>

== static closure: quote ==
<p>static-closure:hoge</p>

== static function: quote ==
<p>function:hoge</p>

