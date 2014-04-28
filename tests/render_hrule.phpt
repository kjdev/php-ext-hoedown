--TEST--
HTML render: hrule
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
---
EOT;

$hoedown = new Hoedown;

echo "== closure: hrule ==\n";
$hoedown->addRender('hrule', function() {
        return 'closure:hrule';
    });

echo $hoedown->parse($text), "\n";

echo "== function: hrule ==\n";
function hrule() {
    return 'function:hrule';
}

$hoedown->addRender('hrule', 'hrule');

echo $hoedown->parse($text), "\n";

echo "== static closure: hrule ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'hrule' => function() {
                                   return 'static-closure:hrule';
                               }))), "\n";

echo "== static function: hrule ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'hrule' => 'hrule'))), "\n";

--EXPECTF--
== closure: hrule ==
closure:hrule
== function: hrule ==
function:hrule
== static closure: hrule ==
static-closure:hrule
== static function: hrule ==
function:hrule
