--TEST--
HTML render: blockcode
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
```php
echo "hoge";
```
EOT;

$hoedown = new Hoedown;

echo "== closure: blockcode ==\n";
$hoedown->addRender('blockcode', function($text, $lang, $attr) {
        return 'closure:'.$text.':'.$lang;
    });

echo $hoedown->parse($text), "\n";

echo "== function: blockcode ==\n";
function blockcode($text, $lang, $attr) {
    return 'function:'.$text.':'.$lang;
}

$hoedown->addRender('blockcode', 'blockcode');

echo $hoedown->parse($text), "\n";

echo "== static closure: blockcode ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'blockcode' => function($text, $lang, $attr) {
                                   return 'static-closure:'.$text.':'.$lang;
                               }))), "\n";

echo "== static function: blockcode ==\n";
echo Hoedown::ofString($text, array(
                           Hoedown::RENDERS => array(
                               'blockcode' => 'blockcode'))), "\n";

--EXPECTF--
== closure: blockcode ==
closure:echo "hoge";
:php
== function: blockcode ==
function:echo "hoge";
:php
== static closure: blockcode ==
static-closure:echo "hoge";
:php
== static function: blockcode ==
function:echo "hoge";
:php
