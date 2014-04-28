--TEST--
addRender and getRenders
--SKIPIF--
--FILE--
<?php
$hoedown = new Hoedown;

echo "== add: blockcode ==\n";
$hoedown->addRender('blockcode', function($text, $lang) { return ''; });
var_dump($hoedown->getRenders());

echo "== add: blockquote ==\n";
$hoedown->addRender('blockquote', function($text) { return ''; });
var_dump($hoedown->getRenders());

echo "== add: function: blockhtml ==\n";
function blockhtml($text) {}
$hoedown->addRender('blockhtml', 'blockhtml');
var_dump($hoedown->getRenders());

echo "== add: setOption: header ==\n";
$hoedown->setOption(Hoedown::RENDERS, array(
                        'header' => function($text, $attr, $level) {
                            return '';
                        }));
var_dump($hoedown->getRenders());

echo "== add: duplicate: blockcode ==\n";
$hoedown->addRender('blockcode', function() { return ''; });
var_dump($hoedown->getRenders());

echo "== other instance ==\n";
$hoedown2 = new Hoedown;
var_dump($hoedown2->getRenders());

--EXPECTF--
== add: blockcode ==
array(1) {
  ["blockcode"]=>
  object(Closure)#%d (1) {
    ["parameter"]=>
    array(2) {
      ["$text"]=>
      string(10) "<required>"
      ["$lang"]=>
      string(10) "<required>"
    }
  }
}
== add: blockquote ==
array(2) {
  ["blockcode"]=>
  object(Closure)#%d (1) {
    ["parameter"]=>
    array(2) {
      ["$text"]=>
      string(10) "<required>"
      ["$lang"]=>
      string(10) "<required>"
    }
  }
  ["blockquote"]=>
  object(Closure)#%d (1) {
    ["parameter"]=>
    array(1) {
      ["$text"]=>
      string(10) "<required>"
    }
  }
}
== add: function: blockhtml ==
array(3) {
  ["blockcode"]=>
  object(Closure)#%d (1) {
    ["parameter"]=>
    array(2) {
      ["$text"]=>
      string(10) "<required>"
      ["$lang"]=>
      string(10) "<required>"
    }
  }
  ["blockquote"]=>
  object(Closure)#%d (1) {
    ["parameter"]=>
    array(1) {
      ["$text"]=>
      string(10) "<required>"
    }
  }
  ["blockhtml"]=>
  string(9) "blockhtml"
}
== add: setOption: header ==
array(4) {
  ["blockcode"]=>
  object(Closure)#%d (1) {
    ["parameter"]=>
    array(2) {
      ["$text"]=>
      string(10) "<required>"
      ["$lang"]=>
      string(10) "<required>"
    }
  }
  ["blockquote"]=>
  object(Closure)#%d (1) {
    ["parameter"]=>
    array(1) {
      ["$text"]=>
      string(10) "<required>"
    }
  }
  ["blockhtml"]=>
  string(9) "blockhtml"
  ["header"]=>
  object(Closure)#%d (1) {
    ["parameter"]=>
    array(3) {
      ["$text"]=>
      string(10) "<required>"
      ["$attr"]=>
      string(10) "<required>"
      ["$level"]=>
      string(10) "<required>"
    }
  }
}
== add: duplicate: blockcode ==
array(4) {
  ["blockcode"]=>
  object(Closure)#%d (0) {
  }
  ["blockquote"]=>
  object(Closure)#%d (1) {
    ["parameter"]=>
    array(1) {
      ["$text"]=>
      string(10) "<required>"
    }
  }
  ["blockhtml"]=>
  string(9) "blockhtml"
  ["header"]=>
  object(Closure)#%d (1) {
    ["parameter"]=>
    array(3) {
      ["$text"]=>
      string(10) "<required>"
      ["$attr"]=>
      string(10) "<required>"
      ["$level"]=>
      string(10) "<required>"
    }
  }
}
== other instance ==
NULL
