--TEST--
Check for WavesAddress::__construct basic behavior
--FILE--
<?php
try {
    $a = new WavesAddress('123');
} catch (\InvalidArgumentException $e) {
    // For some reason, PHP 7.3.5 segfaults when using echo. It also refused to
    // accept just normal printf(). The only function that worked was fprintf()
    fprintf(STDOUT, "1:%s\n", get_class($e));
    fprintf(STDOUT, "2:%s\n", $e->getMessage());
}

$a = new WavesAddress(str_repeat('0', 13) . str_repeat("\xaf", 13));
echo '3:', var_dump(bin2hex($a->address));
?>
--EXPECT--
1:InvalidArgumentException
2:Address length expected to be 26 bytes long, got 3 bytes
3:string(52) "30303030303030303030303030afafafafafafafafafafafafaf"
