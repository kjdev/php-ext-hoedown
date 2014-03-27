--TEST--
link automatic
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
This is an automatic link <http://www.w3.org/>
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p>This is an automatic link <a href="http://www.w3.org/">http://www.w3.org/</a></p>
