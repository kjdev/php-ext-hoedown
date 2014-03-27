--TEST--
header level1 hash sign trailing 1 space
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
 # This is an H1
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p># This is an H1</p>
