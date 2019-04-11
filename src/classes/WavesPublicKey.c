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
#include "php_waves.h"
#include "priv.h"
#include "zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"

/*{{{ proto WavesPublicKey::__construct(string $key)
 * Throws InvalidArgumentException */
PHP_METHOD(WavesPublicKey, __construct)
{
	char *key;
	size_t key_len;
	php_waves_public_key_t *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s",
				&key, &key_len) == FAILURE) {
		return;
	}

	CHECK_PUBLIC_KEY_LEN(key_len);

	intern = php_waves_public_key_object_fetch(Z_OBJ_P(getThis()));
	PHP_WAVES_ASSERT(intern);
  memcpy(intern->key, key, key_len);
}/*}}}*/

/*{{{ proto bool WavesPublicKey::verify(string $message, WavesSignature $signature) */
PHP_METHOD(WavesPublicKey, verify)
{
  char *message;
  size_t message_len;
  zval *signature_zval;
  php_waves_signature_t *signature_intern;
  php_waves_public_key_t *intern;

  if (zend_parse_parameters(ZEND_NUM_ARGS(), "sO",
			  &message, &message_len,
			  &signature_zval, php_waves_signature_ce) == FAILURE) {
	  return;
	}

	signature_intern = php_waves_signature_object_fetch(Z_OBJ_P(signature_zval));
	PHP_WAVES_ASSERT(signature_intern);

	intern = php_waves_public_key_object_fetch(Z_OBJ_P(getThis()));
	PHP_WAVES_ASSERT(intern);

  if (waves_verify_message((const unsigned char *)intern->key,
        (const unsigned char *)message,
        message_len, (const unsigned char *)signature_intern->signature)) {
    RETURN_TRUE;
  }
	RETVAL_FALSE;
}/*}}}*/

/*{{{ proto WavesAddress WavesPublicKey::getAddress(string $networkByte)
 * Throws InvalidArgumentException */
PHP_METHOD(WavesPublicKey, getAddress)
{
	char *network_byte;
	size_t network_byte_len;
	php_waves_public_key_t *intern;
	php_waves_address_t *address_intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s",
				&network_byte, &network_byte_len) == FAILURE) {
		return;
	}

	CHECK_NETWORK_BYTE_LEN(network_byte_len);

	intern = php_waves_public_key_object_fetch(Z_OBJ_P(getThis()));
	PHP_WAVES_ASSERT(intern);

	object_init_ex(return_value, php_waves_address_ce);
	address_intern = php_waves_address_object_fetch(Z_OBJ_P(return_value));
	PHP_WAVES_ASSERT(address_intern);

	waves_public_key_to_address((const unsigned char *)intern->key,
			(unsigned char)network_byte[0], (unsigned char *)address_intern->address);
}/*}}}*/

/*{{{ proto static WavesPublicKey WavesPublicKey::from(WavesPrivateKey $private_key) */
PHP_METHOD(WavesPublicKey, fromPrivateKey)
{
	zval *private_key_zval;
	php_waves_private_key_t *private_key_intern;
	php_waves_public_key_t *intern;
	unsigned char private_key[WAVES_PRIVATE_KEY_BYTES] = {0};

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O",
				&private_key_zval, php_waves_private_key_ce) == FAILURE) {
		return;
	}

	object_init_ex(return_value, php_waves_public_key_ce);
	intern = php_waves_public_key_object_fetch(Z_OBJ_P(return_value));
	PHP_WAVES_ASSERT(intern);

	private_key_intern = php_waves_private_key_object_fetch(Z_OBJ_P(private_key_zval));
	PHP_WAVES_ASSERT(private_key_intern);

	memcpy(private_key, private_key_intern->key, WAVES_PRIVATE_KEY_BYTES);
	php_waves_sc_clamp((unsigned char *)private_key);

	waves_gen_public_key((unsigned char *)intern->key,
			(unsigned char *)private_key);
}/*}}}*/
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 sts=4 fdm=marker
 * vim<600: noet sw=4 ts=4 sts=4
 */
