--TEST--
HTML render: is user block
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
{closure "closure test" }
{function "function test" }
EOT;

$hoedown = new Hoedown;

echo "== closure: user_block ==\n";
$hoedown->setOption(Hoedown::USER_BLOCK, function($text) {
        if (preg_match('/^{closure.*}/', $text, $matches)) {
            var_dump($matches[0]);
            return strlen($matches[0]);
        }
        return 0;
    });

echo $hoedown->parse($text), "\n";

echo "== function: user_block ==\n";
function user_block($text) {
    if (preg_match('/^{function.*}/', $text, $matches)) {
        var_dump($matches[0]);
        return strlen($matches[0]);
    }
    return 0;
}

$hoedown->setOption(Hoedown::USER_BLOCK, 'user_block');

echo $hoedown->parse($text), "\n";

echo "== static closure: user_block ==\n";
echo Hoedown::ofString(
    $text, array(
        Hoedown::USER_BLOCK => function($text) {
            if (preg_match('/^{closure.*}/', $text, $matches)) {
                var_dump($matches[0]);
                return strlen($matches[0]);
            }
            return 0;
        })), "\n";

echo "== static function: user_block ==\n";
echo Hoedown::ofString($text, array(Hoedown::USER_BLOCK => 'user_block')), "\n";

--EXPECTF--
== closure: user_block ==
string(25) "{closure "closure test" }"
{closure "closure test" }
<p>{function &quot;function test&quot; }</p>

== function: user_block ==
string(27) "{function "function test" }"
<p>{closure &quot;closure test&quot; }
{function "function test" }</p>

== static closure: user_block ==
string(25) "{closure "closure test" }"
{closure "closure test" }
<p>{function &quot;function test&quot; }</p>

== static function: user_block ==
string(27) "{function "function test" }"
<p>{closure &quot;closure test&quot; }
{function "function test" }</p>
