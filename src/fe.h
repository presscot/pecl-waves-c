/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright © 2019 Waves Platform                                      |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/
#ifndef PHP_WAVES_FE_H
#define PHP_WAVES_FE_H
#include "php.h"

PHP_FUNCTION(keccak256);
PHP_FUNCTION(secp256k1_sign);
PHP_FUNCTION(secp256k1_sign2);
PHP_FUNCTION(waves_secure_hash);
PHP_FUNCTION(waves_sign_message);
PHP_FUNCTION(waves_base58_encode);
PHP_FUNCTION(waves_base58_decode);
PHP_FUNCTION(waves_verify_message);
PHP_FUNCTION(waves_public_key_to_address);
PHP_FUNCTION(waves_generate_public_key);
PHP_FUNCTION(waves_generate_private_key);

PHP_METHOD(WavesAddress, __construct);

PHP_METHOD(WavesPublicKey, __construct);
PHP_METHOD(WavesPublicKey, getAddress);
PHP_METHOD(WavesPublicKey, verify);
PHP_METHOD(WavesPublicKey, fromPrivateKey);

PHP_METHOD(WavesSignature, __construct);

PHP_METHOD(WavesPrivateKey, __construct);
PHP_METHOD(WavesPrivateKey, sign);
PHP_METHOD(WavesPrivateKey, fromSeed);
#endif /* PHP_WAVES_FE_H */
/*
 * vim600: fdm=marker
 * vim: noet sts=4 sw=4 ts=4
 */
