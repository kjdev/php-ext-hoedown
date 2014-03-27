--TEST--
blockquote nested return level1
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
> This is the first level of quoting.
> > This is nested blockquote.
> Back to the first level.

EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<blockquote>
<p>This is the first level of quoting.</p>

<blockquote>
<p>This is nested blockquote.
Back to the first level.</p>
</blockquote>
</blockquote>
