--TEST--
render of option Hoedown::DISABLE_INDENTED_CODE
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
    echo "hoge"
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::DISABLE_INDENTED_CODE;

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
bool(false)
<pre><code>echo &quot;hoge&quot;
</code></pre>
== enable ==
bool(true)
<p>echo &quot;hoge&quot;</p>
== disable ==
bool(false)
<pre><code>echo &quot;hoge&quot;
</code></pre>