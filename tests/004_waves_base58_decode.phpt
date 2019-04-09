--TEST--
Check for waves_base58_decode function basic behavior
--FILE--
<?php
var_dump('1', bin2hex(waves_base58_decode("68aDe9MU5zeTm7s3gnC1c7NvXFrCo")));
var_dump('2', bin2hex(waves_base58_decode('1')));
var_dump('3', bin2hex(waves_base58_decode('11')));
var_dump('4', bin2hex(waves_base58_decode('')));
?>
--EXPECT--
string(1) "1"
string(42) "536f6d6520737472696e6720015c780c6600000000"
string(1) "2"
string(2) "00"
string(1) "3"
string(4) "0000"
string(1) "4"
string(0) ""
