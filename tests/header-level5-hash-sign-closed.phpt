--TEST--
header level5 hash sign closed
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
##### This is an H5 #####
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<h5>This is an H5</h5>
