--TEST--
sample_test1() Basic test
--SKIPIF--
<?php
if (!extension_loaded('sample')) {
	echo 'skip';
}
?>
--FILE--
<?php
$ret = sample_test1();

var_dump($ret);
?>
--EXPECT--
The extension sample is loaded and working!
NULL
