--TEST--
HTML render: rawhtmltag
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
<a href="">hoge</a>
EOT;

$hoedown = new Hoedown;

echo "== closure: rawhtmltag ==\n";
$hoedown->addRender('rawhtmltag', function($tag) {
        return 'closure:'.$tag;
    });

echo $hoedown->parse($text), "\n";

echo "== function: rawhtmltag ==\n";
function rawhtmltag($tag) {
    return 'function:'.$tag;
}

$hoedown->addRender('rawhtmltag', 'rawhtmltag');

echo $hoedown->parse($text), "\n";

echo "== static closure: rawhtmltag ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'rawhtmltag' => function($tag) {
                                   return 'static-closure:'.$tag;
                               }))), "\n";

echo "== static function: rawhtmltag ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'rawhtmltag' => 'rawhtmltag'))), "\n";

--EXPECTF--
== closure: rawhtmltag ==
<p>closure:<a href="">hogeclosure:</a></p>

== function: rawhtmltag ==
<p>function:<a href="">hogefunction:</a></p>

== static closure: rawhtmltag ==
<p>static-closure:<a href="">hogestatic-closure:</a></p>

== static function: rawhtmltag ==
<p>function:<a href="">hogefunction:</a></p>

