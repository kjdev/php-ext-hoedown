--TEST--
header level2 dash underlined
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
This is an H2
-------------
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<h2>This is an H2</h2>
