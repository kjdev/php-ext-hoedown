--TEST--
2 paragraphs line returns
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
A first paragraph.



A second paragraph after 3 CR (carriage return).
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p>A first paragraph.</p>

<p>A second paragraph after 3 CR (carriage return).</p>
