--TEST--
render of option Hoedown::SCRIPT_TAGS
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
<?xml version="1.0" ?>

This is <?php echo "an example" ?> inline link.

<?php
echo "This link:";
echo "http://example.net/";
?>
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::SCRIPT_TAGS;

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
<p>&lt;?xml version=&quot;1.0&quot; ?&gt;</p>

<p>This is &lt;?php echo &quot;an example&quot; ?&gt; inline link.</p>

<p>&lt;?php
echo &quot;This link:&quot;;
echo &quot;<a href="http://example.net/">http://example.net/</a>&quot;;
?&gt;</p>
== enable ==
bool(true)
<?xml version="1.0" ?>

<p>This is <?php echo "an example" ?> inline link.</p>

<?php
echo "This link:";
echo "http://example.net/";
?>
== disable ==
bool(false)
<p>&lt;?xml version=&quot;1.0&quot; ?&gt;</p>

<p>This is &lt;?php echo &quot;an example&quot; ?&gt; inline link.</p>

<p>&lt;?php
echo &quot;This link:&quot;;
echo &quot;<a href="http://example.net/">http://example.net/</a>&quot;;
?&gt;</p>
