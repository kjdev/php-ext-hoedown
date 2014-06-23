--TEST--
Bug #7 (php_hoedown_set_option() doesn't work like expected)
--SKIPIF--
--FILE--
<?php
$hoedown = new Hoedown;

var_dump($hoedown->getOption(Hoedown::ESCAPE));

$hoedown->setOption(Hoedown::ESCAPE, false);

var_dump($hoedown->getOption(Hoedown::ESCAPE));
--EXPECTF--
bool(false)
bool(false)
