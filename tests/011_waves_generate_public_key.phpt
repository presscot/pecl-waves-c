--TEST--
Check for waves_generate_public_key basic behavior
--FILE--
<?php
$message = 'hello there!';
$privateKey = waves_base58_decode('1z36g6AetkWSpArw9yBxeunCVvKSyB3cEbuyv9XD2QV');

$publicKey = waves_generate_public_key($privateKey);
echo 'public key:'; var_dump(waves_base58_encode($publicKey));
$signature = waves_sign_message($privateKey, $message);
//echo 'signature:'; var_dump(waves_base58_encode($signature));
echo '1:'; var_dump(waves_verify_message($publicKey, $message, $signature));

$pk = new WavesPrivateKey($privateKey);
$sig = new WavesSignature($signature);
$publicKey = WavesPublicKey::fromPrivateKey($pk);
echo '2:'; var_dump($publicKey->verify($message, $sig));
?>
--EXPECT--
public key:string(44) "6XFxxaWniuZBpPX3xx4h2YnCGzKwL5xgpggMNAtU77by"
1:bool(true)
2:bool(true)
