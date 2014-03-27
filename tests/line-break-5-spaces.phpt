--TEST--
line break 5 spaces
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
A first sentence     
and a line break.
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p>A first sentence<br>
and a line break.</p>
