--TEST--
img idref
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
![HTML5][h5]

[h5]: http://www.w3.org/html/logo/img/mark-word-icon.png
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p><img src="http://www.w3.org/html/logo/img/mark-word-icon.png" alt="HTML5"></p>
