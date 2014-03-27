--TEST--
constructor
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
# php
http://www.php.net/
EOT;

echo "== options: none ==\n";
$hoedown = new Hoedown;
echo $hoedown->parse($text);

echo "== options: [Hoedown::AUTOLINK => false] ==\n";
$hoedown = new Hoedown(array(Hoedown::AUTOLINK => false));
echo $hoedown->parse($text);

echo "== options: [Hoedown::AUTOLINK] ==\n";
$hoedown = new Hoedown(array(Hoedown::AUTOLINK));
echo $hoedown->parse($text);

echo "== options: 0 ==\n";
$hoedown = new Hoedown(0);
echo $hoedown->parse($text);

--EXPECTF--
== options: none ==
<h1>php</h1>

<p><a href="http://www.php.net/">http://www.php.net/</a></p>
== options: [Hoedown::AUTOLINK => false] ==
<h1>php</h1>

<p>http://www.php.net/</p>
== options: [Hoedown::AUTOLINK] ==
<h1>php</h1>

<p><a href="http://www.php.net/">http://www.php.net/</a></p>
== options: 0 ==

Warning: Hoedown::__construct() expects parameter 1 to be array, integer given in %s on line %d
<h1>php</h1>

<p><a href="http://www.php.net/">http://www.php.net/</a></p>
