--TEST--
HTML render: linebreak
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
ho  
ge
EOT;

$hoedown = new Hoedown;

echo "== closure: linebreak ==\n";
$hoedown->addRender('linebreak', function() {
        return 'closure:linebreak';
    });

echo $hoedown->parse($text), "\n";

echo "== function: linebreak ==\n";
function linebreak() {
    return 'function:linebreak';
}

$hoedown->addRender('linebreak', 'linebreak');

echo $hoedown->parse($text), "\n";

echo "== static closure: linebreak ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'linebreak' => function() {
                                   return 'static-closure:linebreak';
                               }))), "\n";

echo "== static function: linebreak ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'linebreak' => 'linebreak'))), "\n";

--EXPECTF--
== closure: linebreak ==
<p>hoclosure:linebreakge</p>

== function: linebreak ==
<p>hofunction:linebreakge</p>

== static closure: linebreak ==
<p>hostatic-closure:linebreakge</p>

== static function: linebreak ==
<p>hofunction:linebreakge</p>

