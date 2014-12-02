--TEST--
toc render of option Hoedown::TOC_BEGIN, Hoedown::TOC_END
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
# a

hoge

## b

foo

### `code`

huge
EOT;

$hoedown = new Hoedown;

$hoedown->setOption(Hoedown::TOC, true);

$opt = Hoedown::TOC_BEGIN;

echo "== default renderer and state ==\n";
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text, $state);
if (array_key_exists('toc', $state)) {
    echo ": toc\n";
    echo $state['toc'];
}

echo "== toc renderer ==\n";
$hoedown->setOption(Hoedown::RENDERER_TOC, true);
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text);

// TOC_BEGIN = 2
$hoedown->setOption(Hoedown::RENDERER_HTML, true);
$hoedown->setOption($opt, 2);

echo "== default renderer and state: 2 ==\n";
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text, $state);
if (array_key_exists('toc', $state)) {
    echo ": toc\n";
    echo $state['toc'];
}

echo "== toc renderer: 2 ==\n";
$hoedown->setOption(Hoedown::RENDERER_TOC, true);
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text);

// and TOC_END = 2
$hoedown->setOption(Hoedown::RENDERER_HTML, true);
$hoedown->setOption(Hoedown::TOC_END, 2);

echo "== default renderer and state: 2:2 ==\n";
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text, $state);
if (array_key_exists('toc', $state)) {
    echo ": toc\n";
    echo $state['toc'];
}

echo "== toc renderer: 2:2 ==\n";
$hoedown->setOption(Hoedown::RENDERER_TOC, true);
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text);
--EXPECTF--
== default renderer and state ==
int(0)
<h1 id="a">a</h1>

<p>hoge</p>

<h2 id="b">b</h2>

<p>foo</p>

<h3 id="code"><code>code</code></h3>

<p>huge</p>
: toc
<ul>
<li>
<a href="#a">a</a>
<ul>
<li>
<a href="#b">b</a>
<ul>
<li>
<a href="#code"><code>code</code></a>
</li>
</ul>
</li>
</ul>
</li>
</ul>
== toc renderer ==
int(0)
<ul>
<li>
<a href="#a">a</a>
<ul>
<li>
<a href="#b">b</a>
<ul>
<li>
<a href="#code"><code>code</code></a>
</li>
</ul>
</li>
</ul>
</li>
</ul>
== default renderer and state: 2 ==
int(2)
<h1 id="a">a</h1>

<p>hoge</p>

<h2 id="b">b</h2>

<p>foo</p>

<h3 id="code"><code>code</code></h3>

<p>huge</p>
: toc
<ul>
<li>
<a href="#b">b</a>
<ul>
<li>
<a href="#code"><code>code</code></a>
</li>
</ul>
</li>
</ul>
== toc renderer: 2 ==
int(2)
<ul>
<li>
<a href="#b">b</a>
<ul>
<li>
<a href="#code"><code>code</code></a>
</li>
</ul>
</li>
</ul>
== default renderer and state: 2:2 ==
int(2)
<h1 id="a">a</h1>

<p>hoge</p>

<h2 id="b">b</h2>

<p>foo</p>

<h3><code>code</code></h3>

<p>huge</p>
: toc
<ul>
<li>
<a href="#b">b</a>
</li>
</ul>
== toc renderer: 2:2 ==
int(2)
<ul>
<li>
<a href="#b">b</a>
</li>
</ul>
