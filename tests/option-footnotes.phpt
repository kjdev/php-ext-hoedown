--TEST--
render of option Hoedown::FOOTNOTES
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
Footnotes[^1] have a label[^label] and a definition[^!DEF].

[^1]: This is a footnote
[^label]: A footnote on "label"
[^!DEF]: The definition of a footnote.
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::FOOTNOTES;

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
<p>Footnotes[^1] have a label[^label] and a definition[^!DEF].</p>

<p>[^1]: This is a footnote
[^label]: A footnote on &quot;label&quot;
[^!DEF]: The definition of a footnote.</p>
== enable ==
bool(true)
<p>Footnotes<sup id="fnref1"><a href="#fn1" rel="footnote">1</a></sup> have a label<sup id="fnref2"><a href="#fn2" rel="footnote">2</a></sup> and a definition<sup id="fnref3"><a href="#fn3" rel="footnote">3</a></sup>.</p>

<div class="footnotes">
<hr>
<ol>

<li id="fn1">
<p>This is a footnote&nbsp;<a href="#fnref1" rev="footnote">&#8617;</a></p>
</li>

<li id="fn2">
<p>A footnote on &quot;label&quot;&nbsp;<a href="#fnref2" rev="footnote">&#8617;</a></p>
</li>

<li id="fn3">
<p>The definition of a footnote.&nbsp;<a href="#fnref3" rev="footnote">&#8617;</a></p>
</li>

</ol>
</div>
== disable ==
bool(false)
<p>Footnotes[^1] have a label[^label] and a definition[^!DEF].</p>

<p>[^1]: This is a footnote
[^label]: A footnote on &quot;label&quot;
[^!DEF]: The definition of a footnote.</p>
