--TEST--
em star
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
*single asterisks*
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p><em>single asterisks</em></p>
