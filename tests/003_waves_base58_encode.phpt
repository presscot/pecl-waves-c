--TEST--
Check for waves_base58_encode function basic behavior
--FILE--
<?php
var_dump('1', waves_base58_encode("Some string \x01\x\ff\x00\x00\x00\x00"));
var_dump('2', waves_base58_encode("\x00"));
var_dump('3', waves_base58_encode("\x00\x00"));
var_dump('4', waves_base58_encode(""));
?>
--EXPECT--
string(1) "1"
string(29) "68aDe9MU5zeTm7s3gnC1c7NvXFrCo"
string(1) "2"
string(1) "1"
string(1) "3"
string(2) "11"
string(1) "4"
string(0) ""
