--TEST--
em middle word
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
as*te*risks
EOT;

$hoedown = new Hoedown;
$hoedown->setOption(Hoedown::NO_INTRA_EMPHASIS, false);
echo $hoedown->parse($text);

--EXPECTF--
<p>as<em>te</em>risks</p>
