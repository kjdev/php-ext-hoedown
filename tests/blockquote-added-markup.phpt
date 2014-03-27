--TEST--
blockquote added markup
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
> # heading level 1
> 
> paragraph
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<blockquote>
<h1>heading level 1</h1>

<p>paragraph</p>
</blockquote>
