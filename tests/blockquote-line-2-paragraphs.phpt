--TEST--
blockquote line 2 paragraphs
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
>A blockquote with a very long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long line.

>and a second very long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long line.
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<blockquote>
<p>A blockquote with a very long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long line.</p>

<p>and a second very long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long line.</p>
</blockquote>
