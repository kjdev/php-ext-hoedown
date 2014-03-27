--TEST--
paragraph trailing tab
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
This is a paragraph with 1 trailing tab.	
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p>This is a paragraph with 1 trailing tab.    </p>
