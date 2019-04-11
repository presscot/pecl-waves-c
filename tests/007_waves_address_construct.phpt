--TEST--
Check for WavesAddress::__construct basic behavior
--FILE--
<?php
try {
    $a = new WavesAddress('123');
} catch (\Throwable $e) {
    echo '1:', var_dump(get_class($e));
    echo '2:', var_dump($e->getMessage());
}

$a = new WavesAddress(str_repeat('0', 13) . str_repeat("\xaf", 13));
echo '3:', var_dump(bin2hex($a->address));
?>
--EXPECT--
1:string(24) "InvalidArgumentException"
2:string(56) "Address length expected to be 26 bytes long, got 3 bytes"
3:string(52) "30303030303030303030303030afafafafafafafafafafafafaf"
