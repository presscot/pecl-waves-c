--TEST--
Check for waves_generate_private_key basic behavior
--FILE--
<?php
$seed = 'some seed blah blah';

$privateKey = waves_generate_private_key($seed);
var_dump(bin2hex($privateKey));

$privateKey = WavesPrivateKey::fromSeed($seed);
var_dump(bin2hex($privateKey->key));

?>
--EXPECTF--
string(64) "%s"
string(64) "%s"
