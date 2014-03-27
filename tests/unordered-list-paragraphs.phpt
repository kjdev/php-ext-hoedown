--TEST--
unordered list paragraphs
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
* list item in paragraph

* another list item in paragraph
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<ul>
<li><p>list item in paragraph</p></li>
<li><p>another list item in paragraph</p></li>
</ul>
