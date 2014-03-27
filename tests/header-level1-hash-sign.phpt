--TEST--
header level1 hash sign
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
# This is an H1
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<h1>This is an H1</h1>
