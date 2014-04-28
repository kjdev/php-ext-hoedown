--TEST--
HTML render: image
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
![hoge](hoge.png "Hoge")
EOT;

$hoedown = new Hoedown;

echo "== closure: image ==\n";
$hoedown->addRender('image', function($link, $title, $alt, $attr) {
        return 'closure:'.$link.':'.$title.':'.$alt.':'.$attr;
    });

echo $hoedown->parse($text), "\n";

echo "== function: image ==\n";
function image($link, $title, $alt, $attr) {
    return 'function:'.$link.':'.$title.':'.$alt.':'.$attr;
}

$hoedown->addRender('image', 'image');

echo $hoedown->parse($text), "\n";

echo "== static closure: image ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'image' => function($link, $title, $alt, $attr) {
                                   return 'static-closure:'.$link.':'.$title.':'.$alt.':'.$attr;
                               }))), "\n";

echo "== static function: image ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'image' => 'image'))), "\n";

--EXPECTF--
== closure: image ==
<p>closure:hoge.png:Hoge:hoge:</p>

== function: image ==
<p>function:hoge.png:Hoge:hoge:</p>

== static closure: image ==
<p>static-closure:hoge.png:Hoge:hoge:</p>

== static function: image ==
<p>function:hoge.png:Hoge:hoge:</p>

