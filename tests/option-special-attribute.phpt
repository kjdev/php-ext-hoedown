--TEST--
render of option Hoedown::SPECIAL_ATTRIBUTE
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
# header {.head #id1}

[web](http://www.php.net){#id2 .link .php}

* item1 {#item1 .item}
* item2 {.hoge #item2 .item}
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::SPECIAL_ATTRIBUTE;

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
<h1>header {.head #id1}</h1>

<p><a href="http://www.php.net">web</a>{#id2 .link .php}</p>

<ul>
<li>item1 {#item1 .item}</li>
<li>item2 {.hoge #item2 .item}</li>
</ul>
== enable ==
bool(true)
<h1 id="id1" class="head">header</h1>

<p><a href="http://www.php.net" id="id2" class="link php">web</a></p>

<ul>
<li id="item1" class="item">item1</li>
<li id="item2" class="hoge item">item2</li>
</ul>
== disable ==
bool(false)
<h1>header {.head #id1}</h1>

<p><a href="http://www.php.net">web</a>{#id2 .link .php}</p>

<ul>
<li>item1 {#item1 .item}</li>
<li>item2 {.hoge #item2 .item}</li>
</ul>
