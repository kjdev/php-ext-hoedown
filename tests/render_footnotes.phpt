--TEST--
HTML render: footnotes
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
hoge[^1]

[^1]: footnote
EOT;

$hoedown = new Hoedown;
$hoedown->setOption(Hoedown::FOOTNOTES, true);

echo "== closure: footnotes ==\n";
$hoedown->addRender('footnotes', function($text) {
        return 'closure:footnotes:'.$text;
    });

echo $hoedown->parse($text), "\n";

echo "== closure: footnotedef ==\n";
$hoedown->addRender('footnotedef', function($text, $num) {
        return 'closure:footnotedef:'.$text.':'.$num;
    });

echo $hoedown->parse($text), "\n";

echo "== closure: footnoteref ==\n";
$hoedown->addRender('footnoteref', function($num) {
        return 'closure:footnoteref:'.$num;
    });

echo $hoedown->parse($text), "\n";


function footnotes($text)
{
    return 'function:footnotes:'.$text;
}

function footnotedef($text, $num)
{
    return 'function:footnotedef:'.$text.':'.$num;
}

function footnoteref($num)
{
    return 'function:footnoteref:'.$num;
}

echo "== function: footnotes ==\n";
$hoedown->addRender('footnotes', 'footnotes');

echo $hoedown->parse($text), "\n";

echo "== function: footnoteref ==\n";
$hoedown->addRender('footnoteref', 'footnoteref');

echo $hoedown->parse($text), "\n";

echo "== function: footnotedef ==\n";
$hoedown->addRender('footnotedef', 'footnotedef');

echo $hoedown->parse($text), "\n";


echo "== static closure: footnotes ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::FOOTNOTES => true,
                           Hoedown::RENDERS => array(
                               'footnotes' => function($text) {
                                   return 'static-closure:footnotes:'.$text;
                               }))), "\n";

echo "== static function: footnoteref ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::FOOTNOTES => true,
                           Hoedown::RENDERS => array(
                               'footnotes' => function($text) {
                                   return 'static-closure:footnotes:'.$text;
                               },
                               'footnoteref' => 'footnoteref'))), "\n";

echo "== static function: footnotedef ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::FOOTNOTES => true,
                           Hoedown::RENDERS => array(
                               'footnotes' => function($text) {
                                   return 'static-closure:footnotes:'.$text;
                               },
                               'footnoteref' => 'footnoteref',
                               'footnotedef' => 'footnotedef'))), "\n";

--EXPECTF--
== closure: footnotes ==
<p>hoge<sup id="fnref1"><a href="#fn1" rel="footnote">1</a></sup></p>
closure:footnotes:
<li id="fn1">
<p>footnote&nbsp;<a href="#fnref1" rev="footnote">&#8617;</a></p>
</li>

== closure: footnotedef ==
<p>hoge<sup id="fnref1"><a href="#fn1" rel="footnote">1</a></sup></p>
closure:footnotes:closure:footnotedef:<p>footnote</p>
:1
== closure: footnoteref ==
<p>hogeclosure:footnoteref:1</p>
closure:footnotes:closure:footnotedef:<p>footnote</p>
:1
== function: footnotes ==
<p>hogeclosure:footnoteref:1</p>
function:footnotes:closure:footnotedef:<p>footnote</p>
:1
== function: footnoteref ==
<p>hogefunction:footnoteref:1</p>
function:footnotes:closure:footnotedef:<p>footnote</p>
:1
== function: footnotedef ==
<p>hogefunction:footnoteref:1</p>
function:footnotes:function:footnotedef:<p>footnote</p>
:1
== static closure: footnotes ==
<p>hoge<sup id="fnref1"><a href="#fn1" rel="footnote">1</a></sup></p>
static-closure:footnotes:
<li id="fn1">
<p>footnote&nbsp;<a href="#fnref1" rev="footnote">&#8617;</a></p>
</li>

== static function: footnoteref ==
<p>hogefunction:footnoteref:1</p>
static-closure:footnotes:
<li id="fn1">
<p>footnote&nbsp;<a href="#fnref1" rev="footnote">&#8617;</a></p>
</li>

== static function: footnotedef ==
<p>hogefunction:footnoteref:1</p>
static-closure:footnotes:function:footnotedef:<p>footnote</p>
:1
