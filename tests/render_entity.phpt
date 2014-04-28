--TEST--
HTML render: entity
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
&amp;
EOT;

$hoedown = new Hoedown;

echo "== closure: entity ==\n";
$hoedown->addRender('entity', function($entity) {
        return 'closure:'.$entity;
    });

echo $hoedown->parse($text), "\n";

echo "== function: entity ==\n";
function entity($entity) {
    return 'function:'.$entity;
}

$hoedown->addRender('entity', 'entity');

echo $hoedown->parse($text), "\n";

echo "== static closure: entity ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'entity' => function($entity) {
                                   return 'static-closure:'.$entity;
                               }))), "\n";

echo "== static function: entity ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'entity' => 'entity'))), "\n";

--EXPECTF--
== closure: entity ==
<p>closure:&amp;</p>

== function: entity ==
<p>function:&amp;</p>

== static closure: entity ==
<p>static-closure:&amp;</p>

== static function: entity ==
<p>function:&amp;</p>

