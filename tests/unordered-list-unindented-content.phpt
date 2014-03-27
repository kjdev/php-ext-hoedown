--TEST--
unordered list unindented content
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
* This a very long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long paragraph in a list.
* and yet another long long long long long long long long long long long long long long long long long long long long long long line.
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<ul>
<li>This a very long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long paragraph in a list.</li>
<li>and yet another long long long long long long long long long long long long long long long long long long long long long long line.</li>
</ul>
