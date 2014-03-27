--TEST--
header level6 hash sign closed
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
###### This is an H6  ######
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<h6>This is an H6</h6>
