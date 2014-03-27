--TEST--
ordered list escaped
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
1\. ordered list escape
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p>1. ordered list escape</p>
