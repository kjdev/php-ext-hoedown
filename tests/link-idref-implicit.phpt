--TEST--
link idref implicit
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
[w3c][]

[w3c]: http://www.w3.org/
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p><a href="http://www.w3.org/">w3c</a></p>
