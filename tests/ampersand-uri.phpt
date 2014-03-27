--TEST--
ampersand uri
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
There is an [ampersand](http://validator.w3.org/check?uri=http://www.w3.org/&verbose=1) in the URI.
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p>There is an <a href="http://validator.w3.org/check?uri=http://www.w3.org/&amp;verbose=1">ampersand</a> in the URI.</p>
