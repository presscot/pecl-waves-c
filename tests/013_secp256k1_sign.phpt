--TEST--
Check for secp256k1_sign basic behavior
--FILE--
<?php
$seed = 'some seed blah blah';

$privateKey = secp256k1_sign('test','test');
var_dump(bin2hex($privateKey));

?>
--EXPECTF--
string(128) "%s"
