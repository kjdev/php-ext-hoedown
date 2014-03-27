--TEST--
link idref title single quote
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
[World Wide Web Consortium][w3c]

[w3c]: http://www.w3.org/ 'Discover w3c'
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p><a href="http://www.w3.org/" title="Discover w3c">World Wide Web Consortium</a></p>
