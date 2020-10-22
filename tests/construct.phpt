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
try {
    $hoedown = new Hoedown(0);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
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
Hoedown::__construct(): Argument #1 ($options) must be of type array, int given
<h1>php</h1>

<p><a href="http://www.php.net/">http://www.php.net/</a></p>
