--TEST--
render of option Hoedown::CLASS_TASK_LIST
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
* [ ] task1
* [x] task2
EOT;

$hoedown = new Hoedown;
$hoedown->setOption(Hoedown::TASK_LIST, true);

$opt = Hoedown::CLASS_TASK_LIST;

echo "== default ==\n";
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text);

echo "== set: task ==\n";
$hoedown->setOption($opt, "task");
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text);

echo "== set: NULL ==\n";
$hoedown->setOption($opt, NULL);
var_dump($hoedown->getOption($opt));
echo $hoedown->parse($text);

echo "== new set: task-1 ==\n";
$hoedown1 = new Hoedown;
$hoedown1->setOption(Hoedown::TASK_LIST, true);
$hoedown1->setOption($opt, "task-1");
var_dump($hoedown1->getOption($opt));
echo $hoedown1->parse($text);

--EXPECTF--
== default ==
NULL
<ul>
<li><input type="checkbox"> task1</li>
<li><input checked="" type="checkbox"> task2</li>
</ul>
== set: task ==
string(4) "task"
<ul class="task">
<li><input type="checkbox"> task1</li>
<li><input checked="" type="checkbox"> task2</li>
</ul>
== set: NULL ==
NULL
<ul>
<li><input type="checkbox"> task1</li>
<li><input checked="" type="checkbox"> task2</li>
</ul>
== new set: task-1 ==
string(6) "task-1"
<ul class="task-1">
<li><input type="checkbox"> task1</li>
<li><input checked="" type="checkbox"> task2</li>
</ul>
