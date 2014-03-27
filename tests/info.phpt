--TEST--
phpinfo() displays hoedown info
--SKIPIF--
--FILE--
<?php

phpinfo();
?>
--EXPECTF--
%a
hoedown

Hoedown support => enabled
Extension Version => %d.%d.%d
library version => %d.%d.%d
library extras => enabled
%a
