--TEST--
blockquote multiline 1 space end
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
>A blockquote 
>on multiple lines 
>like this. 
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<blockquote>
<p>A blockquote 
on multiple lines 
like this. </p>
</blockquote>
