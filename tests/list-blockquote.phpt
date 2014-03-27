--TEST--
list blockquote
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
*   a list containing a blockquote

    > this the blockquote in the list
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<ul>
<li><p>a list containing a blockquote</p>

<blockquote>
<p>this the blockquote in the list</p>
</blockquote></li>
</ul>

