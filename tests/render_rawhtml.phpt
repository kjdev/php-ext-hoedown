--TEST--
HTML render: rawhtml
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
<a href="">hoge</a>
EOT;

$hoedown = new Hoedown;

echo "== closure: rawhtml ==\n";
$hoedown->addRender('rawhtml', function($content) {
        return 'closure:'.$content;
    });

echo $hoedown->parse($text), "\n";

echo "== function: rawhtml ==\n";
function rawhtml($content) {
    return 'function:'.$content;
}

$hoedown->addRender('rawhtml', 'rawhtml');

echo $hoedown->parse($text), "\n";

echo "== static closure: rawhtml ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'rawhtml' => function($content) {
                                   return 'static-closure:'.$content;
                               }))), "\n";

echo "== static function: rawhtml ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'rawhtml' => 'rawhtml'))), "\n";

--EXPECTF--
== closure: rawhtml ==
<p>closure:<a href="">hogeclosure:</a></p>

== function: rawhtml ==
<p>function:<a href="">hogefunction:</a></p>

== static closure: rawhtml ==
<p>static-closure:<a href="">hogestatic-closure:</a></p>

== static function: rawhtml ==
<p>function:<a href="">hogefunction:</a></p>

