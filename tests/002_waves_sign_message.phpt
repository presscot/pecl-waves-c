--TEST--
Check for waves_sign_message function basic behavior
--FILE--
<?php
$privkey = waves_base58_decode('1z36g6AetkWSpArw9yBxeunCVvKSyB3cEbuyv9XD2QV');
$pubkey  = waves_base58_decode('6XFxxaWniuZBpPX3xx4h2YnCGzKwL5xgpggMNAtU77by');
$message = 'hello there!';
$random = str_pad(uniqid('sign-message-', true), 64, 'X');

$signature = waves_sign_message($privkey, $message);
if (!waves_verify_message($pubkey, $message, $signature)) {
    echo "1: Generated signature is invalid\n";
    exit();
}
$signature = waves_sign_message($privkey, $message, $random);
if (!waves_verify_message($pubkey, $message, $signature)) {
    echo "2: Generated signature is invalid\n";
    exit();
}

$signature = waves_sign_message($privkey, $message, null);
if (!waves_verify_message($pubkey, $message, $signature)) {
    echo "3: Generated signature is invalid\n";
    exit();
}
echo 'ok';
?>
--EXPECT--
ok
