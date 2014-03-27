--TEST--
em underscore
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
_single underscores_
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p><em>single underscores</em></p>
