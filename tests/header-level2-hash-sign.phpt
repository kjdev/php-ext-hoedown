--TEST--
header level2 hash sign
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
## This is an H2
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<h2>This is an H2</h2>
