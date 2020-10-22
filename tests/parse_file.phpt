--TEST--
parse of file
--SKIPIF--
--FILE--
<?php
$filename = __DIR__ . '/test1.md';

$hoedown = new Hoedown;

echo "== file: test1.md ==\n";
echo $hoedown->parseFile($filename);

echo "== file: test2.md ==\n";
echo $hoedown->parseFile(__DIR__ . '/test2.md');

echo "== file: hoge.md (none) ==\n";
echo $hoedown->parseFile(__DIR__ . '/hoge.md');

echo "== file: empty ==\n";
echo $hoedown->parseFile('');

--EXPECTF--
== file: test1.md ==
<h1>a</h1>

<p><a href="http://www.php.net">http://www.php.net</a></p>

<h2>b</h2>

<p>hoge</p>
== file: test2.md ==
<h1>AA</h1>

<pre><code>echo &quot;hoge&quot;;
</code></pre>

<h2>BB</h2>

<p>foo</p>
== file: hoge.md (none) ==

Warning: Hoedown::parseFile(%s): Failed to open stream: No such file or directory in %s on line %d
== file: empty ==
