--TEST--
paragraphs trailing spaces
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
This is a paragraph with a trailing space. 
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p>This is a paragraph with a trailing space. </p>
