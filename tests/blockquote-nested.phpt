--TEST--
blockquote nested
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
> This is the first level of quoting.
> > This is nested blockquote.

EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<blockquote>
<p>This is the first level of quoting.</p>

<blockquote>
<p>This is nested blockquote.</p>
</blockquote>
</blockquote>
