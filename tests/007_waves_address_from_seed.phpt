--TEST--
Check for WavesAddress::fromSeed method basic behavior
--FILE--
<?php
try {
    WavesAddress::fromSeed('seed', 'network_byte');
} catch (\Throwable $e) {
    echo '1:', var_dump(get_class($e));
    echo '2:', var_dump($e->getMessage());
}

$a = WavesAddress::fromSeed('seed blah blah blah', 'A');
echo '3:', var_dump(bin2hex($a->address));
?>
--EXPECT--
1:string(24) "InvalidArgumentException"
2:string(62) "Network byte length expected to be one byte long, got 12 bytes"
3:string(52) "014141b0ed41d0ef54d76112545f7de2a33f3d2bd2489a520f00"
