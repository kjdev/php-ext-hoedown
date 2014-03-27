--TEST--
parse of string
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
# a
## b
http://www.php.net/
EOT;

$hoedown = new Hoedown;

echo "== parse ==\n";
echo $hoedown->parse($text);

echo "== parseString ==\n";
echo $hoedown->parseString($text);

echo "== parse: empty ==\n";
echo $hoedown->parse('');

echo "== parseString: empty ==\n";
echo $hoedown->parseString('');

--EXPECTF--
== parse ==
<h1>a</h1>

<h2>b</h2>

<p><a href="http://www.php.net/">http://www.php.net/</a></p>
== parseString ==
<h1>a</h1>

<h2>b</h2>

<p><a href="http://www.php.net/">http://www.php.net/</a></p>
== parse: empty ==
== parseString: empty ==
