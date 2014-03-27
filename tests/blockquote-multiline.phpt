--TEST--
blockquote multiline
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
>A blockquote
>on multiple lines
>like this
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<blockquote>
<p>A blockquote
on multiple lines
like this</p>
</blockquote>
