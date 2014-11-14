--TEST--
render of option Hoedown::FENCED_CODE_SCRIPT
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
``` script@text/javascript
alert("Example");
```
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::FENCED_CODE_SCRIPT;

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
<pre><code class="language-script@text/javascript">alert(&quot;Example&quot;);
</code></pre>
== enable ==
bool(true)
<script type="text/javascript">
alert("Example");
</script>
== disable ==
bool(false)
<pre><code class="language-script@text/javascript">alert(&quot;Example&quot;);
</code></pre>
