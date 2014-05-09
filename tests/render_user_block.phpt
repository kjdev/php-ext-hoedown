--TEST--
HTML render: user block
--SKIPIF--
--FILE--
<?php
$text =<<<EOT
{{test}}
EOT;

$hoedown = new Hoedown;

$hoedown->setOption(Hoedown::IS_USER, function($text) {
        if (preg_match('/^{{.*}}/', $text, $matches)) {
            return strlen($matches[0]);
        }
        return 0;
    });

echo "== closure: userblock ==\n";
$hoedown->addRender('userblock', function($text) {
        return 'closure:'.$text;
    });

echo $hoedown->parse($text), "\n";

echo "== function: userblock ==\n";
function user_block($text) {
    return 'function:'.$text;
}

$hoedown->addRender('userblock', 'user_block');

echo $hoedown->parse($text), "\n";

echo "== static closure: userblock ==\n";
echo Hoedown::ofString(
    $text, array(
        Hoedown::IS_USER => function($text) {
            if (preg_match('/^{{.*}}/', $text, $matches)) {
                return strlen($matches[0]);
            }
            return 0;
        },
        Hoedown::RENDERS => array(
            'userblock' => function($text) {
                return 'closure:'.$text;
            }),
    )), "\n";

echo "== static function: userblock ==\n";
echo Hoedown::ofString(
    $text, array(
        Hoedown::IS_USER => function($text) {
            if (preg_match('/^{{.*}}/', $text, $matches)) {
                return strlen($matches[0]);
            }
            return 0;
        },
        Hoedown::RENDERS => array(
            'userblock' => 'user_block'),
    )), "\n";

--EXPECTF--
== closure: userblock ==
closure:{{test}}
== function: userblock ==
function:{{test}}
== static closure: userblock ==
closure:{{test}}
== static function: userblock ==
function:{{test}}
