--TEST--
ordered list items random number
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
1. list item 1
8. list item 2
1. list item 3
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<ol>
<li>list item 1</li>
<li>list item 2</li>
<li>list item 3</li>
</ol>
