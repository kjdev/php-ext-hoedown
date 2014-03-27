--TEST--
2 paragraphs line spaces
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
This a very long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long paragraph on 1 line.
     
A few spaces and a new long long long long long long long long long long long long long long long long paragraph on 1 line.
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p>This a very long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long paragraph on 1 line.</p>

<p>A few spaces and a new long long long long long long long long long long long long long long long long paragraph on 1 line.</p>
