--TEST--
entities text flow
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
HTML entities are written using ampersand notation: &copy;
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p>HTML entities are written using ampersand notation: &copy;</p>
