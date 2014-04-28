--TEST--
HTML render: table
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
| Left align | Right align | Center align |
|:-----------|------------:|:------------:|
| This       |        This |     This     |
EOT;

$hoedown = new Hoedown;

echo "== closure: table ==\n";
$hoedown->addRender('table', function($header, $body, $attr) {
        return 'closure:table:'.$header.':'.$body.':'.$attr;
    });

echo $hoedown->parse($text), "\n";

echo "== closure: tablerow ==\n";
$hoedown->addRender('tablerow', function($text) {
        return 'closure:tablerow:'.$text;
    });

echo $hoedown->parse($text), "\n";

echo "== closure: tablecell ==\n";
$hoedown->addRender('tablecell', function($text, $flags) {
        return 'closure:tablecell:'.$text.':'.$flags;
    });

echo $hoedown->parse($text), "\n";


function table($header, $body, $attr)
{
    return 'function:table:'.$header.':'.$body.':'.$attr;
}

function tablerow($text)
{
    return 'function:tablerow:'.$text;
}

function tablecell($text, $flags)
{
    return 'function:tablecell:'.$text.':'.$flags;
}

echo "== function: table ==\n";
$hoedown->addRender('table', 'table');

echo $hoedown->parse($text), "\n";

echo "== function: tablecell ==\n";
$hoedown->addRender('tablecell', 'tablecell');

echo $hoedown->parse($text), "\n";

echo "== function: tablerow ==\n";
$hoedown->addRender('tablerow', 'tablerow');

echo $hoedown->parse($text), "\n";


echo "== static closure: table ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'table' => function($header, $body, $attr) {
                                   return 'static-closure:table:'.$header.':'.$body.':'.$attr;
                               }))), "\n";

echo "== static function: tablecell ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'table' => function($header, $body, $attr) {
                                   return 'static-closure:table:'.$header.':'.$body.':'.$attr;
                               },
                               'tablecell' => 'tablecell'))), "\n";

echo "== static function: tablerow ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'table' => function($header, $body, $attr) {
                                   return 'static-closure:table:'.$header.':'.$body.':'.$attr;
                               },
                               'tablecell' => 'tablecell',
                               'tablerow' => 'tablerow'))), "\n";

--EXPECTF--
== closure: table ==
closure:table:<tr>
<th style="text-align: left">Left align</th>
<th style="text-align: right">Right align</th>
<th style="text-align: center">Center align</th>
</tr>
:<tr>
<td style="text-align: left">This</td>
<td style="text-align: right">This</td>
<td style="text-align: center">This</td>
</tr>
:
== closure: tablerow ==
closure:table:closure:tablerow:<th style="text-align: left">Left align</th>
<th style="text-align: right">Right align</th>
<th style="text-align: center">Center align</th>
:closure:tablerow:<td style="text-align: left">This</td>
<td style="text-align: right">This</td>
<td style="text-align: center">This</td>
:
== closure: tablecell ==
closure:table:closure:tablerow:closure:tablecell:Left align:5closure:tablecell:Right align:6closure:tablecell:Center align:7:closure:tablerow:closure:tablecell:This:1closure:tablecell:This:2closure:tablecell:This:3:
== function: table ==
function:table:closure:tablerow:closure:tablecell:Left align:5closure:tablecell:Right align:6closure:tablecell:Center align:7:closure:tablerow:closure:tablecell:This:1closure:tablecell:This:2closure:tablecell:This:3:
== function: tablecell ==
function:table:closure:tablerow:function:tablecell:Left align:5function:tablecell:Right align:6function:tablecell:Center align:7:closure:tablerow:function:tablecell:This:1function:tablecell:This:2function:tablecell:This:3:
== function: tablerow ==
function:table:function:tablerow:function:tablecell:Left align:5function:tablecell:Right align:6function:tablecell:Center align:7:function:tablerow:function:tablecell:This:1function:tablecell:This:2function:tablecell:This:3:
== static closure: table ==
static-closure:table:<tr>
<th style="text-align: left">Left align</th>
<th style="text-align: right">Right align</th>
<th style="text-align: center">Center align</th>
</tr>
:<tr>
<td style="text-align: left">This</td>
<td style="text-align: right">This</td>
<td style="text-align: center">This</td>
</tr>
:
== static function: tablecell ==
static-closure:table:<tr>
function:tablecell:Left align:5function:tablecell:Right align:6function:tablecell:Center align:7</tr>
:<tr>
function:tablecell:This:1function:tablecell:This:2function:tablecell:This:3</tr>
:
== static function: tablerow ==
static-closure:table:function:tablerow:function:tablecell:Left align:5function:tablecell:Right align:6function:tablecell:Center align:7:function:tablerow:function:tablecell:This:1function:tablecell:This:2function:tablecell:This:3:
