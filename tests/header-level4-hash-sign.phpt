--TEST--
header level4 hash sign
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
#### This is an H4
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<h4>This is an H4</h4>
