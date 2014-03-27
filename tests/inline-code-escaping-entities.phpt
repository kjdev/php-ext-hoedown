--TEST--
inline code escaping entities
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
We love `<code> and &` for everything
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p>We love <code>&lt;code&gt; and &amp;</code> for everything</p>
