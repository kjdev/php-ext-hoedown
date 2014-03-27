--TEST--
2 paragraphs hard return
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
This is a first paragraph,
on multiple lines.

This is a second paragraph
which has multiple lines too.
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p>This is a first paragraph,
on multiple lines.</p>

<p>This is a second paragraph
which has multiple lines too.</p>
