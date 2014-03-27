--TEST--
inline code with visible backtick
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
``We love `code` for everything``
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p><code>We love `code` for everything</code></p>
