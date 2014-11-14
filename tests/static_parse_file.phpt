--TEST--
static parse of file
--SKIPIF--
--FILE--
<?php
$filename = __DIR__ . '/test3.md';

echo "== ofFile ==\n";
echo Hoedown::ofFile($filename);

echo "== ofFile : without Hoedown::AUTOLINK ==\n";
echo Hoedown::ofFile($filename, array(Hoedown::AUTOLINK => false));

echo "== ofFile : with Hoedown::TOC ==\n";
echo Hoedown::ofFile($filename, array(Hoedown::TOC => true), $state);
if (is_array($state) && array_key_exists('toc', $state)) {
    echo ": toc\n";
    echo $state['toc'];
}
unset($state);

echo "== ofFile : with Hoedown::TOC and Hoedown::RENDERER_TOC ==\n";
echo Hoedown::ofFile($filename, array(Hoedown::TOC => true, Hoedown::RENDERER_TOC => true), $state);
if (is_array($state) && array_key_exists('toc', $state)) {
    echo ": toc\n";
    echo $state['toc'];
}

echo "== ofFile : none ==\n";
echo Hoedown::ofFile(__DIR__ . '/hoge.md');

echo "== ofFile : empty ==\n";
echo Hoedown::ofFile('');

--EXPECTF--
== ofFile ==
<h1>table</h1>

<table>
<thead>
<tr>
<th>First Header</th>
<th>Second Header</th>
</tr>
</thead>

<tbody>
<tr>
<td>Content Cell</td>
<td>Content Cell</td>
</tr>
<tr>
<td>Content Cell</td>
<td>Content Cel</td>
</tr>
</tbody>
</table>

<h2>fenced code</h2>

<pre><code class="language-php">echo &quot;hello world&quot;;
</code></pre>

<h3>autolink</h3>

<p><a href="http://www.php.net/">http://www.php.net/</a></p>

<h4>strikethrough</h4>

<p>this is <del>good</del> bad.</p>

<h5>no intra emphasis</h5>

<p>hoge_fuga_foo</p>
== ofFile : without Hoedown::AUTOLINK ==
<h1>table</h1>

<table>
<thead>
<tr>
<th>First Header</th>
<th>Second Header</th>
</tr>
</thead>

<tbody>
<tr>
<td>Content Cell</td>
<td>Content Cell</td>
</tr>
<tr>
<td>Content Cell</td>
<td>Content Cel</td>
</tr>
</tbody>
</table>

<h2>fenced code</h2>

<pre><code class="language-php">echo &quot;hello world&quot;;
</code></pre>

<h3>autolink</h3>

<p>http://www.php.net/</p>

<h4>strikethrough</h4>

<p>this is <del>good</del> bad.</p>

<h5>no intra emphasis</h5>

<p>hoge_fuga_foo</p>
== ofFile : with Hoedown::TOC ==
<h1 id="table">table</h1>

<table>
<thead>
<tr>
<th>First Header</th>
<th>Second Header</th>
</tr>
</thead>

<tbody>
<tr>
<td>Content Cell</td>
<td>Content Cell</td>
</tr>
<tr>
<td>Content Cell</td>
<td>Content Cel</td>
</tr>
</tbody>
</table>

<h2 id="fenced-code">fenced code</h2>

<pre><code class="language-php">echo &quot;hello world&quot;;
</code></pre>

<h3 id="autolink">autolink</h3>

<p><a href="http://www.php.net/">http://www.php.net/</a></p>

<h4 id="strikethrough">strikethrough</h4>

<p>this is <del>good</del> bad.</p>

<h5 id="no-intra-emphasis">no intra emphasis</h5>

<p>hoge_fuga_foo</p>
: toc
<ul>
<li>
<a href="#table">table</a>
<ul>
<li>
<a href="#fenced-code">fenced code</a>
<ul>
<li>
<a href="#autolink">autolink</a>
<ul>
<li>
<a href="#strikethrough">strikethrough</a>
<ul>
<li>
<a href="#no-intra-emphasis">no intra emphasis</a>
</li>
</ul>
</li>
</ul>
</li>
</ul>
</li>
</ul>
</li>
</ul>
== ofFile : with Hoedown::TOC and Hoedown::RENDERER_TOC ==
<ul>
<li>
<a href="#table">table</a>
<ul>
<li>
<a href="#fenced-code">fenced code</a>
<ul>
<li>
<a href="#autolink">autolink</a>
<ul>
<li>
<a href="#strikethrough">strikethrough</a>
<ul>
<li>
<a href="#no-intra-emphasis">no intra emphasis</a>
</li>
</ul>
</li>
</ul>
</li>
</ul>
</li>
</ul>
</li>
</ul>
== ofFile : none ==

Warning: Hoedown::ofFile(%s): failed to open stream: No such file or directory in %s on line %d
== ofFile : empty ==
