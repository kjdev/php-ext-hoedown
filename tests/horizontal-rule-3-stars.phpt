--TEST--
horizontal rule 3 stars
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
***
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<hr>
