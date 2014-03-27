--TEST--
horizontal rule 3 dashes
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
---
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<hr>
