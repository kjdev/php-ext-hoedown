--TEST--
img title
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
![HTML5](http://www.w3.org/html/logo/img/mark-word-icon.png "HTML5 logo for everyone")
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);

--EXPECTF--
<p><img src="http://www.w3.org/html/logo/img/mark-word-icon.png" alt="HTML5" title="HTML5 logo for everyone"></p>
