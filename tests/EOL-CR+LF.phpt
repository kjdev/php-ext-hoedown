--TEST--
EOL CR+LF
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
These lines all end with end of line (EOL) sequences.

Seriously, they really do.

If you don't believe me: HEX EDIT!


EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p>These lines all end with end of line (EOL) sequences.</p>

<p>Seriously, they really do.</p>

<p>If you don&#39;t believe me: HEX EDIT!</p>
