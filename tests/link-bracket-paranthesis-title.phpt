--TEST--
link bracket paranthesis title
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
[W3C](http://www.w3.org/ "Discover w3c")
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p><a href="http://www.w3.org/" title="Discover w3c">W3C</a></p>
