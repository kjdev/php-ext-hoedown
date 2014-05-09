--TEST--
render task list of option Hoedown::SPECIAL_ATTRIBUTE
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
* [ ] task1
* [x] task2
{@list .task}
EOT;

$hoedown = new Hoedown;
$hoedown->setOption(Hoedown::TASK_LIST, true);

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
<ul>
<li><input type="checkbox"> task1</li>
<li><input checked="" type="checkbox"> task2
{@list .task}</li>
</ul>
== enable ==
bool(true)
<ul class="task">
<li><input type="checkbox"> task1</li>
<li><input checked="" type="checkbox"> task2</li>
</ul>
== disable ==
bool(false)
<ul>
<li><input type="checkbox"> task1</li>
<li><input checked="" type="checkbox"> task2
{@list .task}</li>
</ul>
