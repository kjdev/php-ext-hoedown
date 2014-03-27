--TEST--
link bracket paranthesis
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
[W3C](http://www.w3.org/)
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p><a href="http://www.w3.org/">W3C</a></p>
