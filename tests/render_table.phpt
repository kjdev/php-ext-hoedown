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
$hoedown->addRender('table', function($body, $attr) {
        return 'closure:table:'.$body.':'.$attr;
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


function table($body, $attr)
{
    return 'function:table:'.$body.':'.$attr;
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
                               'table' => function($body, $attr) {
                                   return 'static-closure:table:'.$body.':'.$attr;
                               }))), "\n";

echo "== static function: tablecell ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'table' => function($body, $attr) {
                                   return 'static-closure:table:'.$body.':'.$attr;
                               },
                               'tablecell' => 'tablecell'))), "\n";

echo "== static function: tablerow ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'table' => function($body, $attr) {
                                   return 'static-closure:table:'.$body.':'.$attr;
                               },
                               'tablecell' => 'tablecell',
                               'tablerow' => 'tablerow'))), "\n";

--EXPECTF--
== closure: table ==
closure:table:<thead>
<tr>
<th style="text-align: left">Left align</th>
<th style="text-align: right">Right align</th>
<th style="text-align: center">Center align</th>
</tr>
</thead>

<tbody>
<tr>
<td style="text-align: left">This</td>
<td style="text-align: right">This</td>
<td style="text-align: center">This</td>
</tr>
</tbody>
:
== closure: tablerow ==
closure:table:<thead>
closure:tablerow:<th style="text-align: left">Left align</th>
<th style="text-align: right">Right align</th>
<th style="text-align: center">Center align</th>
</thead>

<tbody>
closure:tablerow:<td style="text-align: left">This</td>
<td style="text-align: right">This</td>
<td style="text-align: center">This</td>
</tbody>
:
== closure: tablecell ==
closure:table:<thead>
closure:tablerow:closure:tablecell:Left align:5closure:tablecell:Right align:6closure:tablecell:Center align:7</thead>

<tbody>
closure:tablerow:closure:tablecell:This:1closure:tablecell:This:2closure:tablecell:This:3</tbody>
:
== function: table ==
function:table:<thead>
closure:tablerow:closure:tablecell:Left align:5closure:tablecell:Right align:6closure:tablecell:Center align:7</thead>

<tbody>
closure:tablerow:closure:tablecell:This:1closure:tablecell:This:2closure:tablecell:This:3</tbody>
:
== function: tablecell ==
function:table:<thead>
closure:tablerow:function:tablecell:Left align:5function:tablecell:Right align:6function:tablecell:Center align:7</thead>

<tbody>
closure:tablerow:function:tablecell:This:1function:tablecell:This:2function:tablecell:This:3</tbody>
:
== function: tablerow ==
function:table:<thead>
function:tablerow:function:tablecell:Left align:5function:tablecell:Right align:6function:tablecell:Center align:7</thead>

<tbody>
function:tablerow:function:tablecell:This:1function:tablecell:This:2function:tablecell:This:3</tbody>
:
== static closure: table ==
static-closure:table:<thead>
<tr>
<th style="text-align: left">Left align</th>
<th style="text-align: right">Right align</th>
<th style="text-align: center">Center align</th>
</tr>
</thead>

<tbody>
<tr>
<td style="text-align: left">This</td>
<td style="text-align: right">This</td>
<td style="text-align: center">This</td>
</tr>
</tbody>
:
== static function: tablecell ==
static-closure:table:<thead>
<tr>
function:tablecell:Left align:5function:tablecell:Right align:6function:tablecell:Center align:7</tr>
</thead>

<tbody>
<tr>
function:tablecell:This:1function:tablecell:This:2function:tablecell:This:3</tr>
</tbody>
:
== static function: tablerow ==
static-closure:table:<thead>
function:tablerow:function:tablecell:Left align:5function:tablecell:Right align:6function:tablecell:Center align:7</thead>

<tbody>
function:tablerow:function:tablecell:This:1function:tablecell:This:2function:tablecell:This:3</tbody>
:
