--TEST--
render of option Hoedown::FENCED_CODE
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
```php
echo "hello world";
```
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::FENCED_CODE;

echo "== default ==\n";
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text);

echo "== enable ==\n";
$hoedown->setOption($opt, true);
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text);

echo "== disable ==\n";
$hoedown->setOption($opt, false);
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text);

--EXPECTF--
== default ==
bool(true)
<pre><code class="language-php">echo &quot;hello world&quot;;
</code></pre>
== enable ==
bool(true)
<pre><code class="language-php">echo &quot;hello world&quot;;
</code></pre>
== disable ==
bool(false)
<p><code>php
echo &quot;hello world&quot;;
</code></p>
