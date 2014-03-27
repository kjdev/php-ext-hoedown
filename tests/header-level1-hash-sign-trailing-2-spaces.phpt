--TEST--
header level1 hash sign trailing 2 spaces
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
# this is an h1 with two trailing spaces  
A new paragraph.
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<h1>this is an h1 with two trailing spaces</h1>

<p>A new paragraph.</p>
