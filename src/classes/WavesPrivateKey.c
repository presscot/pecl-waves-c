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
#include "../php_waves.h"
#include "../priv.h"
#include "zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"

/*{{{ proto WavesPrivateKey::__construct(string $key)
 * Throws InvalidArgumentException */
PHP_METHOD(WavesPrivateKey, __construct)
{
	char *key;
	size_t key_len;
	php_waves_private_key_t *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s",
				&key, &key_len) == FAILURE) {
		return;
	}

	CHECK_PRIVATE_KEY_LEN(key_len);

	intern = php_waves_private_key_object_fetch(Z_OBJ_P(getThis()));
	PHP_WAVES_ASSERT(intern);
	memcpy(intern->key, key, key_len);
}/*}}}*/

/*{{{ proto WavesSignature WavesPrivateKey::sign(string $message, ?string $random = null)
 * Throws InvalidArgumentException
 * Throws WavesException */
PHP_METHOD(WavesPrivateKey, sign)
{
	char *message;
	char *random = NULL;
	size_t message_len;
	size_t random_len = 0;
	php_waves_private_key_t *intern;
	php_waves_signature_t *signature_intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|s!",
				&message, &message_len,
				&random, &random_len) == FAILURE) {
		return;
	}

	object_init_ex(return_value, php_waves_signature_ce);
	signature_intern = php_waves_signature_object_fetch(Z_OBJ_P(return_value));
	PHP_WAVES_ASSERT(signature_intern);

	intern = php_waves_private_key_object_fetch(Z_OBJ_P(getThis()));
	PHP_WAVES_ASSERT(intern);

	if (random != NULL && random_len != 0) {
		if (random_len != WAVES_RANDOM_SEED_BYTES) {
			zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0,
					"Random sequence expected to be %ld bytes in length, got %ld bytes",
					(long)WAVES_RANDOM_SEED_BYTES, (long)random_len);
			return;
		}

		if (waves_sign_message_custom_random(
					(const unsigned char *)intern->key,
					(const unsigned char *)message,
					message_len,
					(unsigned char *)signature_intern->signature,
					(unsigned char *)random)) {
			return;
		}

		zend_throw_exception_ex(php_waves_get_exception(), 0,
				"Failed to sign message '%s'", message);
		return;
	}

	if (waves_sign_message((const unsigned char *)intern->key,
				(const unsigned char *)message,
				message_len, (unsigned char *)signature_intern->signature)) {
		return;
	}
	zend_throw_exception_ex(php_waves_get_exception(), 0,
			"Failed to sign message '%s'", message);
}/*}}}*/

/*{{{ proto static WavesPrivateKey WavesPrivateKey::fromSeed(string $seed) */
PHP_METHOD(WavesPrivateKey, fromSeed)
{
	char *seed;
	size_t seed_len;
	php_waves_private_key_t *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s",
				&seed, &seed_len) == FAILURE) {
		return;
	}

	object_init_ex(return_value, php_waves_private_key_ce);
	intern = php_waves_private_key_object_fetch(Z_OBJ_P(return_value));
	PHP_WAVES_ASSERT(intern);

	waves_gen_private_key((unsigned char *)intern->key, (const unsigned char *)seed);
}/*}}}*/
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 sts=4 fdm=marker
 * vim<600: noet sw=4 ts=4 sts=4
 */
