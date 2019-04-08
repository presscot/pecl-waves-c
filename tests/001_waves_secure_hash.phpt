--TEST--
Check for waves_secure_hash function basic behavior
--FILE--
<?php
$input = "A nice, long test to make the day great! :-)";
$expected = "\x5d\xf3\xcf\x20\x20\x5d\x75\xe0\x9a\xe4\x6d\x13\xa8\xd9\x9a\x16\x17\x4d\x71\xc8\x4f\xfc\xc0\x03\x87\xfe\xc3\xd8\x1e\x39\xdc\xbe";
$hash = waves_secure_hash($input);
var_dump(bin2hex($hash));
var_dump($hash === $expected);
var_dump(strlen($hash))
?>
--EXPECT--
string(64) "5df3cf20205d75e09ae46d13a8d99a16174d71c84ffcc00387fec3d81e39dcbe"
bool(true)
int(32)
