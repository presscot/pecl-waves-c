--TEST--
Check for waves_public_key_to_address function basic behavior
--FILE--
<?php
$pubkey = waves_base58_decode('6XFxxaWniuZBpPX3xx4h2YnCGzKwL5xgpggMNAtU77by');
$address = waves_public_key_to_address($pubkey, 'W');
var_dump(waves_base58_encode($address));
?>
--EXPECT--
string(35) "3PHBX4uXhCyaANUxccLHNXw3sqyksV7YnDz"
