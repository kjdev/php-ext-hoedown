--TEST--
horizontal rule 3 underscores
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
___
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<hr>
