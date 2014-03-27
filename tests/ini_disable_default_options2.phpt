--TEST--
ini disable default options: ini On
--SKIPIF--
--INI--
hoedown.disable_default_options = On
--FILE--
<?php
$text = <<<EOT
# table

First Header  | Second Header
------------- | -------------
Content Cell  | Content Cell
Content Cell  | Content Cel

## fenced code

```php
echo "hello world";
```

### autolink

http://www.php.net/

#### strikethrough

this is ~~good~~ bad.

##### no intra emphasis

hoge_fuga_foo
EOT;

$hoedown = new Hoedown;

echo "== parse ==\n";
echo $hoedown->parse($text);

echo "== ofString ==\n";
echo Hoedown::ofString($text);

--EXPECTF--
== parse ==
<h1>table</h1>

<p>First Header  | Second Header
------------- | -------------
Content Cell  | Content Cell
Content Cell  | Content Cel</p>

<h2>fenced code</h2>

<p><code>php
echo &quot;hello world&quot;;
</code></p>

<h3>autolink</h3>

<p>http://www.php.net/</p>

<h4>strikethrough</h4>

<p>this is ~~good~~ bad.</p>

<h5>no intra emphasis</h5>

<p>hoge<em>fuga</em>foo</p>
== ofString ==
<h1>table</h1>

<p>First Header  | Second Header
------------- | -------------
Content Cell  | Content Cell
Content Cell  | Content Cel</p>

<h2>fenced code</h2>

<p><code>php
echo &quot;hello world&quot;;
</code></p>

<h3>autolink</h3>

<p>http://www.php.net/</p>

<h4>strikethrough</h4>

<p>this is ~~good~~ bad.</p>

<h5>no intra emphasis</h5>

<p>hoge<em>fuga</em>foo</p>
