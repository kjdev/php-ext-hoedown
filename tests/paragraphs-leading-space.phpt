--TEST--
paragraphs leading space
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
 This is a paragraph with 1 leading space.
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p>This is a paragraph with 1 leading space.</p>
