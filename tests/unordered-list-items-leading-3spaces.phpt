--TEST--
unordered list items leading 3spaces
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
   * list item 1
   * list item 2
   * list item 3
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<ul>
<li>list item 1</li>
<li>list item 2</li>
<li>list item 3</li>
</ul>
