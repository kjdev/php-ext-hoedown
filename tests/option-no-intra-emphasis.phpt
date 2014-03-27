--TEST--
render of option Hoedown::NO_INTRA_EMPHASIS
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
hoge_fuga_foo
EOT;

$hoedown = new Hoedown;

echo "== default ==\n";
echo $hoedown->parse($text);

echo "== enable ==\n";
$hoedown->setOption(Hoedown::NO_INTRA_EMPHASIS, true);
echo $hoedown->parse($text);

echo "== disable ==\n";
$hoedown->setOption(Hoedown::NO_INTRA_EMPHASIS, false);
echo $hoedown->parse($text);

--EXPECTF--
== default ==
<p>hoge_fuga_foo</p>
== enable ==
<p>hoge_fuga_foo</p>
== disable ==
<p>hoge<em>fuga</em>foo</p>
