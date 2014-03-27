--TEST--
fenced code block
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
```
a
```
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<pre><code>a
</code></pre>
