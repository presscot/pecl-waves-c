--TEST--
Check for WavesPrivateKey::__construct basic behavior
--FILE--
<?php
try {
    new WavesPrivateKey('123');
} catch (\Throwable $e) {
    echo '1:', var_dump(get_class($e));
    echo '2:', var_dump($e->getMessage());
}

$a = new WavesPrivateKey(waves_base58_decode('7XHPhVZbY2CFgFU7furgV3cbMr9tCbDB6ULBGBLGU2Kv'));
echo '3:', var_dump(waves_base58_encode($a->key));
?>
--EXPECT--
1:string(24) "InvalidArgumentException"
2:string(51) "Private key must be 32 bytes in length, got 3 bytes"
3:string(44) "7XHPhVZbY2CFgFU7furgV3cbMr9tCbDB6ULBGBLGU2Kv"
