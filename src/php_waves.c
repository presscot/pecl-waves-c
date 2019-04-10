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
#include "zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"
#include "priv.h"

/* And address is expected to be 26 bytes long (without terminating 0 byte) */
#define WAVES_ADDRESS_BYTES 26
/* A public key is expected to be 32 bytes in length */
#define WAVES_PUBLIC_KEY_BYTES 32
/* A private key is expected to be 32 bytes in length */
#define WAVES_PRIVATE_KEY_BYTES 32
/* Length of a secure hash, in bytes */
#define WAVES_SECURE_HASH_BYTES 32
/* Length of a random seed (passed as an argument to waves_sign_message_custom_random), in bytes */
#define WAVES_RANDOM_SEED_BYTES 64

zend_class_entry *php_waves_exception_ce;

#if 0
 /*{{{ sc_clamp
  * From libwaves_c */
static zend_always_inline void sc_clamp(unsigned char* a)
{
  a[0] &= 248;
  a[31] &= 127;
  a[31] |= 64;
}/*}}}*/
#endif

/* {{{ waves_module_entry */
zend_module_entry waves_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL,
	NULL, /* dependencies */
	"waves",
	waves_functions,
	PHP_MINIT(waves),
	PHP_MSHUTDOWN(waves),
	PHP_RINIT(waves),
	PHP_RSHUTDOWN(waves),
	PHP_MINFO(waves),
	PHP_WAVES_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_WAVES
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE();
#endif
ZEND_GET_MODULE(waves)
#endif

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(waves)
{
	zend_class_entry ce_exception;

	INIT_CLASS_ENTRY(ce_exception, "WavesException", NULL);
	php_waves_exception_ce = zend_register_internal_class_ex(&ce_exception, php_waves_get_exception_base(0));
	zend_declare_property_null(php_waves_exception_ce, "errorInfo", sizeof("errorInfo") - 1, ZEND_ACC_PUBLIC);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(waves)
{
	return SUCCESS;
}
/* }}} */

/*{{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(waves)
{
#if defined(COMPILE_DL_) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/*}}}*/

/*{{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION(waves)
{
	return SUCCESS;
}
/*}}}*/

/*{{{ PHP_MINFO_FUNCTION*/
PHP_MINFO_FUNCTION(waves)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "waves support", "enabled");
#ifdef PHP_WAVES_DEBUG
	php_info_print_table_row(2, "Debug support", "enabled");
#else
	php_info_print_table_row(2, "Debug support", "disabled");
#endif
	php_info_print_table_row(2, "Extension version", PHP_WAVES_VERSION);
	php_info_print_table_row(2, "libwaves_c headers version", "N/A");
	php_info_print_table_end();
}
/*}}}*/

/*{{{ API */

/* {{{ proto string waves_secure_hash(string message) */
PHP_FUNCTION(waves_secure_hash)
{
	char *message;
	size_t message_len;
	uint8_t hash[WAVES_SECURE_HASH_BYTES];

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s",
				&message, &message_len) == FAILURE) {
		return;
	}
	waves_secure_hash((const uint8_t *)message, message_len, hash);
	RETURN_STRINGL((const char *)hash, sizeof(hash));
}
/*}}}*/

/* {{{ proto string waves_sign_message(string $private_key, string $message);
 *
 * Returns signature on success, otherwise NULL.
 *
 * Throws InvalidArgumentException
 * Throws WavesException
 */
PHP_FUNCTION(waves_sign_message)
{
	char *message;
	char *priv_key;
	size_t message_len;
	size_t priv_key_len;
	curve25519_signature signature;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss",
				&priv_key, &priv_key_len,
				&message, &message_len) == FAILURE) {
		return;
	}

    CHECK_PRIVATE_KEY_LEN(priv_key_len);

	if (waves_sign_message((const curve25519_secret_key *)priv_key,
				(const unsigned char *)message,
				message_len, signature)) {
		RETURN_STRINGL((const char *)signature, sizeof(curve25519_signature));
	}
	zend_throw_exception_ex(php_waves_get_exception(), 0,
			"Failed to sign message '%s'", message);
}
/*}}}*/

/* {{{ proto string waves_sign_message_custom_random(string $private_key, string $message, string $random);
 *
 * $random is a 64-byte binary sequence.
 *
 * Returns signature on success, otherwise NULL.
 *
 * Throws InvalidArgumentException
 * Throws WavesException
 */
PHP_FUNCTION(waves_sign_message_custom_random)
{
	char *message;
	char *priv_key;
	char *random;
	size_t message_len;
	size_t priv_key_len;
	size_t random_len;
	curve25519_signature signature;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sss",
				&priv_key, &priv_key_len,
				&message, &message_len,
				&random, &random_len) == FAILURE) {
		return;
	}

    CHECK_PRIVATE_KEY_LEN(priv_key_len);

	if (random_len != WAVES_RANDOM_SEED_BYTES) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0,
				"Random sequence expected to be %ld bytes in length, got %d bytes",
				WAVES_RANDOM_SEED_BYTES, random_len);
		return;
	}

	if (waves_sign_message_custom_random((const curve25519_secret_key *)priv_key,
				(const unsigned char *)message,
				message_len, signature,
				(unsigned char *)random)) {
		RETURN_STRINGL((const char *)signature, sizeof(curve25519_signature));
	}
	zend_throw_exception_ex(php_waves_get_exception(), 0,
			"Failed to sign message '%s' with random sequence '%s'", message, random);
}
/*}}}*/

/* {{{ proto string waves_base58_encode(string $in)
 *
 * Throws WavesException
 */
PHP_FUNCTION(waves_base58_encode)
{
	char *in;
	size_t in_len;
	char *out;
	size_t out_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s",
				&in, &in_len) == FAILURE) {
		return;
	}

	out_len = b58_length_from_bytes(in_len);
	out = emalloc(sizeof(char) * out_len);
	if (!out) {
		zend_throw_exception_ex(php_waves_get_exception(), 0,
				"Failed to allocate %ld bytes",
				sizeof(char) * out_len);
		return;
	}

	out_len = base58_encode(out, (const unsigned char *)in, in_len);
	/* out_len includes the terminating zero byte, so we decrement it */
	RETVAL_STRINGL(out, --out_len);
}
/*}}}*/

/* {{{ proto string waves_base58_decode(string $in) */
PHP_FUNCTION(waves_base58_decode)
{
	char *in;
	size_t in_len;
	char *out;
	size_t out_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s",
				&in, &in_len) == FAILURE) {
		return;
	}

	if (in_len == 0) {
		RETURN_EMPTY_STRING();
	}

	out_len = bytes_length_from_b58(in_len);
	out = emalloc(sizeof(char) * out_len);
	if (!out) {
		zend_throw_exception_ex(php_waves_get_exception(), 0,
				"Failed to allocate %ld bytes",
				sizeof(char) * out_len);
		return;
	}

	out_len = base58_decode((unsigned char *)out, in);
	PHP_WAVES_ASSERT(out_len);
	RETVAL_STRINGL(out, out_len);
}
/*}}}*/

/* {{{ proto bool waves_verify_message(string $public_key, string $message, string $signature) */
PHP_FUNCTION(waves_verify_message)
{
	char *public_key;
	char *message;
	char *signature;
	size_t public_key_len;
	size_t message_len;
	size_t signature_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sss",
				&public_key, &public_key_len,
				&message, &message_len,
				&signature, &signature_len) == FAILURE) {
		return;
	}

	if (waves_verify_message((const curve25519_public_key *)public_key,
			(const unsigned char *)message,
			message_len, (const curve25519_signature *)signature)) {
		RETURN_TRUE;
	}
	RETVAL_FALSE;
}
/*}}}*/

/* {{{ proto string waves_seed_to_address(string $key, string $network_byte)
   Throws WavesException
   Throws InvalidArgumentException */
PHP_FUNCTION(waves_seed_to_address)
{
	char *key;
	char *network_byte;
	char *out;
	size_t key_len;
	size_t network_byte_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss",
				&key, &key_len,
				&network_byte, &network_byte_len) == FAILURE) {
		return;
	}

    CHECK_NETWORK_BYTE_LEN(network_byte_len);
    WAVES_EMALLOC(out, WAVES_ADDRESS_BYTES);

	waves_seed_to_address((const unsigned char *)key,
			(unsigned char)network_byte[0], (unsigned char *)out);
	RETURN_STRINGL(out, WAVES_ADDRESS_BYTES);
}
/*}}}*/

/* {{{ proto string waves_public_key_to_address(string $public_key, string $network_byte)
 * Throws WavesException
   Throws InvalidArgumentException */
PHP_FUNCTION(waves_public_key_to_address)
{
	char *public_key;
	char *network_byte;
	char *out;
	size_t public_key_len;
	size_t network_byte_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss",
				&public_key, &public_key_len,
				&network_byte, &network_byte_len) == FAILURE) {
		return;
	}

	CHECK_PUBLIC_KEY_LEN(public_key_len);
	CHECK_NETWORK_BYTE_LEN(network_byte_len);
	WAVES_EMALLOC(out, WAVES_ADDRESS_BYTES);

	waves_public_key_to_address((const unsigned char *)public_key,
			(unsigned char)network_byte[0], (unsigned char *)out);
	RETURN_STRINGL(out, WAVES_ADDRESS_BYTES);
}
/*}}}*/

#if 0
/* {{{ proto string waves_generate_public_key(string $private_key) */
PHP_FUNCTION(waves_generate_public_key)
{
	char *public_key;
	char *private_key;
	size_t private_key_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s",
				&private_key, &private_key_len) == FAILURE) {
		return;
	}

    CHECK_PRIVATE_KEY_LEN(private_key_len);

	sc_clamp((unsigned char *)private_key);

	public_key = emalloc(WAVES_PUBLIC_KEY_BYTES);
	if (!public_key) {
		zend_throw_exception_ex(php_waves_get_exception(), 0,
				"Failed to allocate %ld bytes", WAVES_PUBLIC_KEY_BYTES);
		return;
	}

	/* XXX This function is currently not exported by libwaves_c */
	curve25519_keygen((unsigned char *)public_key,
			(const unsigned char *)private_key);
	RETURN_STRINGL(public_key, WAVES_PUBLIC_KEY_BYTES);
}
/*}}}*/
#endif

/* API }}}*/

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 sts=4 fdm=marker
 * vim<600: noet sw=4 ts=4 sts=4
 */
