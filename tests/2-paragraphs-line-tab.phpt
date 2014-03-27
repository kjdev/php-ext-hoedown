--TEST--
2 paragraphs line tab
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
This a very long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long paragraph on 1 line.
	
1 tab to separate them and a new long long long long long long long long long long long long long long long long paragraph on 1 line.
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p>This a very long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long paragraph on 1 line.</p>

<p>1 tab to separate them and a new long long long long long long long long long long long long long long long long paragraph on 1 line.</p>
