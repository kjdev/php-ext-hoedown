--TEST--
toc render of option Hoedown::TOC_HEADER and Hoedown::TOC_FOOTER
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

$opt1 = Hoedown::TOC_HEADER;
$opt2 = Hoedown::TOC_FOOTER;

echo "== default renderer and state ==\n";
var_dump($hoedown->getOption($opt1));
var_dump($hoedown->getOption($opt2));
echo $hoedown->parse($text, $state);
if (array_key_exists('toc', $state)) {
    echo ": toc\n";
    echo $state['toc'];
}

echo "== toc renderer ==\n";
$hoedown->setOption(Hoedown::RENDERER_TOC, true);
var_dump($hoedown->getOption($opt1));
var_dump($hoedown->getOption($opt2));
echo $hoedown->parse($text);

$hoedown->setOption(Hoedown::RENDERER_HTML, true);
$hoedown->setOption($opt1, "== toc header ==");
$hoedown->setOption($opt2, "== toc footer ==");

echo "== enable header and footer: default renderer and state ==\n";
var_dump($hoedown->getOption($opt1));
var_dump($hoedown->getOption($opt2));
echo $hoedown->parse($text, $state);
if (array_key_exists('toc', $state)) {
    echo ": toc\n";
    echo $state['toc'];
}

echo "== enable header and footer: toc renderer ==\n";
$hoedown->setOption(Hoedown::RENDERER_TOC, true);
var_dump($hoedown->getOption($opt1));
var_dump($hoedown->getOption($opt2));
echo $hoedown->parse($text);

echo "== empty header and footer: default renderer and state ==\n";
$hoedown->setOption(Hoedown::RENDERER_HTML, true);
$hoedown->setOption($opt1, "");
$hoedown->setOption($opt2, "");

var_dump($hoedown->getOption($opt1));
var_dump($hoedown->getOption($opt2));
echo $hoedown->parse($text, $state);
if (array_key_exists('toc', $state)) {
    echo ": toc\n";
    echo $state['toc'];
}

echo "== new enable header and footer: toc renderer ==\n";
$hoedown1 = new Hoedown;
$hoedown1->setOption(Hoedown::TOC, true);
$hoedown1->setOption($opt1, "<div>HEADER</div>");
$hoedown1->setOption($opt2, "<div>FOOTER</div>");
$hoedown1->setOption(Hoedown::RENDERER_TOC, true);
echo $hoedown1->parse($text);

--EXPECTF--
== default renderer and state ==
NULL
NULL
<h1 id="toc_a">a</h1>

<p>hoge</p>

<h2 id="toc_b">b</h2>

<p>foo</p>

<h3 id="toc_-code-code--code-"><code>code</code></h3>

<p>huge</p>
: toc
<ul>
<li>
<a href="#toc_a">a</a>
<ul>
<li>
<a href="#toc_b">b</a>
<ul>
<li>
<a href="#toc_-code-code--code-">&lt;code&gt;code&lt;/code&gt;</a>
</li>
</ul>
</li>
</ul>
</li>
</ul>
== toc renderer ==
NULL
NULL
<ul>
<li>
<a href="#toc_a">a</a>
<ul>
<li>
<a href="#toc_b">b</a>
<ul>
<li>
<a href="#toc_-code-code--code-">&lt;code&gt;code&lt;/code&gt;</a>
</li>
</ul>
</li>
</ul>
</li>
</ul>
== enable header and footer: default renderer and state ==
string(16) "== toc header =="
string(16) "== toc footer =="
<h1 id="toc_a">a</h1>

<p>hoge</p>

<h2 id="toc_b">b</h2>

<p>foo</p>

<h3 id="toc_-code-code--code-"><code>code</code></h3>

<p>huge</p>
: toc
== toc header ==
<ul>
<li>
<a href="#toc_a">a</a>
<ul>
<li>
<a href="#toc_b">b</a>
<ul>
<li>
<a href="#toc_-code-code--code-">&lt;code&gt;code&lt;/code&gt;</a>
</li>
</ul>
</li>
</ul>
</li>
</ul>
== toc footer ==
== enable header and footer: toc renderer ==
string(16) "== toc header =="
string(16) "== toc footer =="
== toc header ==
<ul>
<li>
<a href="#toc_a">a</a>
<ul>
<li>
<a href="#toc_b">b</a>
<ul>
<li>
<a href="#toc_-code-code--code-">&lt;code&gt;code&lt;/code&gt;</a>
</li>
</ul>
</li>
</ul>
</li>
</ul>
== toc footer ==
== empty header and footer: default renderer and state ==
NULL
NULL
<h1 id="toc_a">a</h1>

<p>hoge</p>

<h2 id="toc_b">b</h2>

<p>foo</p>

<h3 id="toc_-code-code--code-"><code>code</code></h3>

<p>huge</p>
: toc
<ul>
<li>
<a href="#toc_a">a</a>
<ul>
<li>
<a href="#toc_b">b</a>
<ul>
<li>
<a href="#toc_-code-code--code-">&lt;code&gt;code&lt;/code&gt;</a>
</li>
</ul>
</li>
</ul>
</li>
</ul>
== new enable header and footer: toc renderer ==
<div>HEADER</div>
<ul>
<li>
<a href="#toc_a">a</a>
<ul>
<li>
<a href="#toc_b">b</a>
<ul>
<li>
<a href="#toc_-code-code--code-">&lt;code&gt;code&lt;/code&gt;</a>
</li>
</ul>
</li>
</ul>
</li>
</ul>
<div>FOOTER</div>
