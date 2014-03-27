--TEST--
paragraph line
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
This a very long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long paragraph on 1 line.
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p>This a very long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long paragraph on 1 line.</p>
