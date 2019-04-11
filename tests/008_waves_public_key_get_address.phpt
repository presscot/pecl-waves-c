--TEST--
Check for WavesPublicKey::getAddress basic behavior
--FILE--
<?php
$publicKey = new WavesPublicKey(waves_base58_decode('91ApSkoxkaVvrenXCpX7y8qTJXywQ8vrNCV1uNzE4tJo'));
echo '1:', var_dump(waves_base58_encode($publicKey->key));

$address = $publicKey->getAddress('W');
echo '2:', var_dump(waves_base58_encode($address->address));
?>
--EXPECT--
1:string(44) "91ApSkoxkaVvrenXCpX7y8qTJXywQ8vrNCV1uNzE4tJo"
2:string(35) "3PQn6QWv7zk1V5QDRirWVDSiCCLokXFP56E"
