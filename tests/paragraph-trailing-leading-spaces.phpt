--TEST--
paragraph trailing leading spaces
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
 This is a paragraph with a trailing and leading space. 
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p>This is a paragraph with a trailing and leading space. </p>
