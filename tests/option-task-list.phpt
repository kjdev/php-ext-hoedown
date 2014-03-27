--TEST--
render of option Hoedown::TASK_LIST
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
* [ ] task1
* [x] task2
* [ ] task3
EOT;

$hoedown = new Hoedown;

$opt = Hoedown::TASK_LIST;

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
<li>[ ] task1</li>
<li>[x] task2</li>
<li>[ ] task3</li>
</ul>
== enable ==
bool(true)
<ul>
<li><input type="checkbox"> task1</li>
<li><input checked="" type="checkbox"> task2</li>
<li><input type="checkbox"> task3</li>
</ul>
== disable ==
bool(false)
<ul>
<li>[ ] task1</li>
<li>[x] task2</li>
<li>[ ] task3</li>
</ul>
