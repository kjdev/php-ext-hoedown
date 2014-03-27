--TEST--
paragraph hard return
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
This is a paragraph
on multiple lines
with hard return.
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p>This is a paragraph
on multiple lines
with hard return.</p>
