--TEST--
Check for waves_seed_to_address function basic behavior
--FILE--
<?php
$key = 'buffalo august small gallery scout moment bean priority illness lounge bulk guitar regular protect runway';
$address = waves_seed_to_address($key, 'W');
var_dump(waves_base58_encode($address));
?>
--EXPECT--
string(35) "3P4Eq1TkKGHRY8V5sWT4KK4yuqVPtfHszaU"
