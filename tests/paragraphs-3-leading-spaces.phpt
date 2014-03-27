--TEST--
paragraphs 3 leading spaces
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
   This is a paragraph with 3 leading spaces.
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p>This is a paragraph with 3 leading spaces.</p>
