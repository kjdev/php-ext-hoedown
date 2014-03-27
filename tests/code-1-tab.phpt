--TEST--
code 1 tab
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
	10 PRINT HELLO INFINITE
	20 GOTO 10
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<pre><code>10 PRINT HELLO INFINITE
20 GOTO 10
</code></pre>
