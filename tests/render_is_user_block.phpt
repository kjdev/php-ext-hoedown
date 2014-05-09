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

echo "== closure: is_user ==\n";
$hoedown->setOption(Hoedown::IS_USER, function($text) {
        if (preg_match('/^{closure.*}/', $text, $matches)) {
            var_dump($matches[0]);
            return strlen($matches[0]);
        }
        return 0;
    });

echo $hoedown->parse($text), "\n";

echo "== function: is_user ==\n";
function is_user($text) {
    if (preg_match('/^{function.*}/', $text, $matches)) {
        var_dump($matches[0]);
        return strlen($matches[0]);
    }
    return 0;
}

$hoedown->setOption(Hoedown::IS_USER, 'is_user');

echo $hoedown->parse($text), "\n";

echo "== static closure: is_user ==\n";
echo Hoedown::ofString(
    $text, array(
        Hoedown::IS_USER => function($text) {
            if (preg_match('/^{closure.*}/', $text, $matches)) {
                var_dump($matches[0]);
                return strlen($matches[0]);
            }
            return 0;
        })), "\n";

echo "== static function: is_user ==\n";
echo Hoedown::ofString($text, array(Hoedown::IS_USER => 'is_user')), "\n";

--EXPECTF--
== closure: is_user ==
string(25) "{closure "closure test" }"
{closure "closure test" }
<p>{function &quot;function test&quot; }</p>

== function: is_user ==
string(27) "{function "function test" }"
<p>{closure &quot;closure test&quot; }
{function "function test" }</p>

== static closure: is_user ==
string(25) "{closure "closure test" }"
{closure "closure test" }
<p>{function &quot;function test&quot; }</p>

== static function: is_user ==
string(27) "{function "function test" }"
<p>{closure &quot;closure test&quot; }
{function "function test" }</p>
