--TEST--
header level3 hash sign
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
### This is an H3
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<h3>This is an H3</h3>
