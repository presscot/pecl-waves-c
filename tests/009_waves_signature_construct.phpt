--TEST--
Check for WavesSignature::__construct basic behavior
--FILE--
<?php
$signature = new WavesSignature(waves_base58_decode('2fjjtGJY8v2xYYXu8AwNKB4grEB3ApiGNiRrqzE1iMZBSD9u5xo15CCAW6NPWJaNCaE2B1ZRuR9g7nYKMjVrK2eV'));
echo '1:', var_dump(waves_base58_encode($signature->signature));
?>
--EXPECT--
1:string(88) "2fjjtGJY8v2xYYXu8AwNKB4grEB3ApiGNiRrqzE1iMZBSD9u5xo15CCAW6NPWJaNCaE2B1ZRuR9g7nYKMjVrK2eV"
