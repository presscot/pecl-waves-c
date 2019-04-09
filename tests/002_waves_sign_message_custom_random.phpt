--TEST--
Check for waves_sign_message_custom_random function basic behavior
--FILE--
<?php
$privkey = waves_base58_decode('1z36g6AetkWSpArw9yBxeunCVvKSyB3cEbuyv9XD2QV');
$pubkey = waves_base58_decode('6XFxxaWniuZBpPX3xx4h2YnCGzKwL5xgpggMNAtU77by');
//var_dump("privkey", bin2hex($privkey));
$message = 'hello there!';
$signature = waves_sign_message_custom_random(
    $privkey,
    $message,
    uniqid('waves_sign_message_custom_random-', true)
);

if (!waves_verify_message($pubkey, $message, $signature)) {
    echo "Generated signature is invalid\n";
    exit();
}
//echo 'signature:', var_dump(bin2hex($signature));
//echo 'signature base58:', var_dump(waves_base58_encode($signature));
//echo 'privkey:', var_dump($privkey);
//echo 'pubkey:', var_dump($pubkey);
//echo 'message:', var_dump($message);
echo 'ok';
?>
--EXPECT--
ok
