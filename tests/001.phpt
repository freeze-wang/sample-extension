--TEST--
Check if sample is loaded
--SKIPIF--
<?php
if (!extension_loaded('sample')) {
	echo 'skip';
}
?>
--FILE--
<?php
echo 'The extension "sample" is available';
?>
--EXPECT--
The extension "sample" is available
