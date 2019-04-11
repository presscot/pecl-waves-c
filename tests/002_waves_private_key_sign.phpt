--TEST--
Check for WavesPrivateKey::sign method basic behavior
--FILE--
<?php
$privkey = waves_base58_decode('1z36g6AetkWSpArw9yBxeunCVvKSyB3cEbuyv9XD2QV');
$pubkey  = waves_base58_decode('6XFxxaWniuZBpPX3xx4h2YnCGzKwL5xgpggMNAtU77by');
$message = 'hello there!';
$random = str_pad(uniqid('sign-message-', true), 64, 'X');

$privateKey = new WavesPrivateKey($privkey);
$publicKey = new WavesPublicKey($pubkey);
$signature = $privateKey->sign($message);

if (!waves_verify_message($publicKey->key, $message, $signature->signature)) {
    echo "1: Generated signature is invalid\n";
    exit();
}
if (!$publicKey->verify($message, $signature)) {
    echo "1: Generated signature is invalid\n";
    exit();
}

$signature = $privateKey->sign($message, $random);
if (!waves_verify_message($publicKey->key, $message, $signature->signature)) {
    echo "2: Generated signature is invalid\n";
    exit();
}

$signature = $privateKey->sign($message, null);
if (!waves_verify_message($publicKey->key, $message, $signature->signature)) {
    echo "3: Generated signature is invalid\n";
    exit();
}
if (!$publicKey->verify($message, $signature)) {
    echo "3: Generated signature is invalid\n";
    exit();
}

try {
    $signature = $privateKey->sign($message, '123');
} catch (\Throwable $e) {
    echo '1:'; var_dump(get_class($e));
    echo '2:'; var_dump($e->getMessage());
}
echo 'ok';
?>
--EXPECT--
1:string(24) "InvalidArgumentException"
2:string(62) "Random sequence expected to be 64 bytes in length, got 3 bytes"
ok
