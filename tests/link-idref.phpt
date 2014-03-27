--TEST--
link idref
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
[World Wide Web Consortium][w3c]

[w3c]: http://www.w3.org/
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p><a href="http://www.w3.org/">World Wide Web Consortium</a></p>
