--TEST--
2 paragraphs hard return spaces
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
This is a first paragraph,
on multiple lines.
     
This is a second paragraph.
There are spaces in between the two.
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p>This is a first paragraph,
on multiple lines.</p>

<p>This is a second paragraph.
There are spaces in between the two.</p>
