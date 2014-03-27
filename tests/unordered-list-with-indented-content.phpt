--TEST--
unordered list with indented content
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
* This is a list item
  with the conten on
  multiline and indented.
* And this another list item
  with the same principle.
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<ul>
<li>This is a list item
with the conten on
multiline and indented.</li>
<li>And this another list item
with the same principle.</li>
</ul>
