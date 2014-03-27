--TEST--
link automatic email
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
This is an automatic email link <foolspam@example.org>
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p>This is an automatic email link <a href="mailto:foolspam@example.org">foolspam@example.org</a></p>
