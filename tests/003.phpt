--TEST--
sample_test2() Basic test
--SKIPIF--
<?php
if (!extension_loaded('sample')) {
	echo 'skip';
}
?>
--FILE--
<?php
var_dump(sample_test2());
var_dump(sample_test2('PHP'));
?>
--EXPECT--
string(11) "Hello World"
string(9) "Hello PHP"
