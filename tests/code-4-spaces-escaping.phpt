--TEST--
code 4 spaces escaping
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
    10 PRINT < > &
    20 GOTO 10
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<pre><code>10 PRINT &lt; &gt; &amp;
20 GOTO 10
</code></pre>
