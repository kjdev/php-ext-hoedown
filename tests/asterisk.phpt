--TEST--
asterisk
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
This is * an asterisk which should stay as is.
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p>This is * an asterisk which should stay as is.</p>
