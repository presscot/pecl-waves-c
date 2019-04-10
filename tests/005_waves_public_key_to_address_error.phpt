--TEST--
Check for waves_public_key_to_address function error behavior
--FILE--
<?php
try {
     waves_public_key_to_address('1234', 'W');
} catch (\Throwable $e) {
    var_dump(get_class($e));
    var_dump($e->getMessage());
}
?>
--EXPECT--
string(24) "InvalidArgumentException"
string(50) "Public key must be 32 bytes in length, got 4 bytes"
