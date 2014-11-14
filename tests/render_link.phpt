--TEST--
HTML render: link
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
[hoge](http://www.php.net/ "Hoge")
EOT;

$hoedown = new Hoedown;

echo "== closure: link ==\n";
$hoedown->addRender('link', function($content, $link, $title, $attr) {
        return 'closure:'.$link.':'.$title.':'.$content.':'.$attr;
    });

echo $hoedown->parse($text), "\n";

echo "== function: link ==\n";
function _link($content, $link, $title, $attr) {
    return 'function:'.$link.':'.$title.':'.$content.':'.$attr;
}

$hoedown->addRender('link', '_link');

echo $hoedown->parse($text), "\n";

echo "== static closure: link ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'link' => function($content, $link, $title, $attr) {
                                   return 'static-closure:'.$link.':'.$title.':'.$content.':'.$attr;
                               }))), "\n";

echo "== static function: link ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'link' => '_link'))), "\n";

--EXPECTF--
== closure: link ==
<p>closure:http://www.php.net/:Hoge:hoge:</p>

== function: link ==
<p>function:http://www.php.net/:Hoge:hoge:</p>

== static closure: link ==
<p>static-closure:http://www.php.net/:Hoge:hoge:</p>

== static function: link ==
<p>function:http://www.php.net/:Hoge:hoge:</p>

