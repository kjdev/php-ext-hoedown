--TEST--
horizontal rule 7 dashes
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
-------
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<hr>
