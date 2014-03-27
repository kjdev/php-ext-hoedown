--TEST--
link idref title next line
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
[World Wide Web Consortium][w3c]

[w3c]: http://www.w3.org/
   "Discover W3C"
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p><a href="http://www.w3.org/" title="Discover W3C">World Wide Web Consortium</a></p>
