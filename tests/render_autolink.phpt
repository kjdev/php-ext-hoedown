--TEST--
HTML render: autolink
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
http://www.php.net/
EOT;

$hoedown = new Hoedown;

echo "== closure: autolink ==\n";
$hoedown->addRender('autolink', function($link, $type) {
        return 'closure:'.$link.':'.$type;
    });

echo $hoedown->parse($text), "\n";

echo "== function: autolink ==\n";
function autolink($link, $type) {
    return 'function:'.$link.':'.$type;
}

$hoedown->addRender('autolink', 'autolink');

echo $hoedown->parse($text), "\n";

echo "== static closure: autolink ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'autolink' => function($link, $type) {
                                   return 'static-closure:'.$link.':'.$type;
                               }))), "\n";

echo "== static function: autolink ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'autolink' => 'autolink'))), "\n";

--EXPECTF--
== closure: autolink ==
<p>closure:http://www.php.net/:1</p>

== function: autolink ==
<p>function:http://www.php.net/:1</p>

== static closure: autolink ==
<p>static-closure:http://www.php.net/:1</p>

== static function: autolink ==
<p>function:http://www.php.net/:1</p>

