--TEST--
Check for waves_sign_message function basic behavior
--FILE--
<?php
$privkey = waves_base58_decode('1z36g6AetkWSpArw9yBxeunCVvKSyB3cEbuyv9XD2QV');
$pubkey  = waves_base58_decode('6XFxxaWniuZBpPX3xx4h2YnCGzKwL5xgpggMNAtU77by');
$message = 'hello there!';
$signature = waves_sign_message($privkey, $message);

if (!waves_verify_message($pubkey, $message, $signature)) {
    echo "Generated signature is invalid\n";
    exit();
}
echo 'ok';
?>
--EXPECT--
ok
