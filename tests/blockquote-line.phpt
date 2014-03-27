--TEST--
blockquote line
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
>This a very long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long paragraph in a blockquote.
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<blockquote>
<p>This a very long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long paragraph in a blockquote.</p>
</blockquote>
