--TEST--
HTML render: codespan
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
`hoge`
EOT;

$hoedown = new Hoedown;

echo "== closure: codespan ==\n";
$hoedown->addRender('codespan', function($text) {
        return 'closure:'.$text;
    });

echo $hoedown->parse($text), "\n";

echo "== function: codespan ==\n";
function codespan($text) {
    return 'function:'.$text;
}

$hoedown->addRender('codespan', 'codespan');

echo $hoedown->parse($text), "\n";

echo "== static closure: codespan ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'codespan' => function($text) {
                                   return 'static-closure:'.$text;
                               }))), "\n";

echo "== static function: codespan ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'codespan' => 'codespan'))), "\n";

--EXPECTF--
== closure: codespan ==
<p>closure:hoge</p>

== function: codespan ==
<p>function:hoge</p>

== static closure: codespan ==
<p>static-closure:hoge</p>

== static function: codespan ==
<p>function:hoge</p>

