--TEST--
strong underscore
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
__double underscores__
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p><strong>double underscores</strong></p>
