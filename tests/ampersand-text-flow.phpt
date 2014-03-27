--TEST--
ampersand text flow
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
An ampersand & in the text flow is escaped as an html entity.
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p>An ampersand &amp; in the text flow is escaped as an html entity.</p>
