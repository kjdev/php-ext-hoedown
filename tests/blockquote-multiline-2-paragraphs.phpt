--TEST--
blockquote multiline 2 paragraphs
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
>A blockquote
>on multiple lines
>like this.
>
>But it has
>two paragraphs.
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<blockquote>
<p>A blockquote
on multiple lines
like this.</p>

<p>But it has
two paragraphs.</p>
</blockquote>
