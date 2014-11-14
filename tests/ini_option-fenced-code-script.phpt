--TEST--
render of option Hoedown::FENCED_CODE_SCRIPT
--INI--
hoedown.options = "fenced-code-script"
--SKIPIF--
--FILE--
<?php
$text = <<<EOT
``` script@text/javascript
alert("Example");
```
EOT;

$hoedown = new Hoedown;
echo $hoedown->parse($text);
--EXPECTF--
<script type="text/javascript">
alert("Example");
</script>
