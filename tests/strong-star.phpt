--TEST--
strong star
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
**double asterisks**
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p><strong>double asterisks</strong></p>
