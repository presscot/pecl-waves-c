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
#include "Zend/zend_portability.h"
#include "priv.h"

/* And address is expected to be 26 bytes long (without terminating 0 byte) */
#define WAVES_ADDRESS_BYTES 26

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
	uint8_t hash[32];

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s",
				&message, &message_len) == FAILURE) {
		return;
	}
	waves_secure_hash((const uint8_t *)message, message_len, hash);
	RETURN_STRINGL((const char *)hash, sizeof(hash));
}
/*}}}*/

/**
 * {{{ proto string|null waves_sign_message(string $private_key, string $message);
 *
 * Returns signature on success, otherwise NULL.
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

	if (priv_key_len != 32) {
		/* TODO: throw exception */
		php_error_docref(NULL, E_ERROR,
				"Private key must be 32 bytes in length, got %d bytes",
				priv_key_len);
		return;
	}

	if (waves_sign_message((const curve25519_secret_key *)priv_key,
				(const unsigned char *)message,
				message_len, signature)) {
		RETURN_STRINGL((const char *)signature, sizeof(curve25519_signature));
	}
	/* TODO: throw exception? */
}
/*}}}*/

/**
 * {{{ proto string|null waves_sign_message_custom_random(string $private_key, string $message, string $random);
 *
 * $random is a 64-byte binary sequence.
 *
 * Returns signature on success, otherwise NULL.
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

	if (priv_key_len != 32) {
		/* TODO: throw exception */
		php_error_docref(NULL, E_ERROR,
				"Private key must be 32 bytes in length, got %d bytes",
				priv_key_len);
		return;
	}
	if (random_len != 64) {
		/* TODO: throw exception */
		php_error_docref(NULL, E_ERROR,
				"Random sequence expected to be 64 bytes in length, got %d bytes",
				random_len);
		return;
	}

	if (waves_sign_message_custom_random((const curve25519_secret_key *)priv_key,
				(const unsigned char *)message,
				message_len, signature,
				(unsigned char *)random)) {
		RETURN_STRINGL((const char *)signature, sizeof(curve25519_signature));
	}
	/* TODO: throw exception? */
}
/*}}}*/

/**
 * {{{ proto string|null waves_base58_encode(string $in)
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
		/* TODO: Throw exception */
		php_error_docref(NULL, E_ERROR,
				"Failed to allocate %ld bytes",
				sizeof(char) * out_len);
		return;
	}

	out_len = base58_encode(out, (const unsigned char *)in, in_len);
	/* out_len includes the terminating zero byte, so we decrement it */
	RETVAL_STRINGL(out, --out_len);
}
/*}}}*/

/**
 * {{{ proto string|null waves_base58_decode(string $in)
 */
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
		/* TODO: Throw exception */
		php_error_docref(NULL, E_ERROR,
				"Failed to allocate %ld bytes",
				sizeof(char) * out_len);
		return;
	}

	out_len = base58_decode((unsigned char *)out, in);
	ZEND_ASSERT(out_len);
	RETVAL_STRINGL(out, out_len);
}
/*}}}*/

/**
 * {{{ proto bool waves_verify_message(string $public_key, string $message, string $signature)
 */
PHP_FUNCTION(waves_verify_message)
{
	char *public_key;
	char *message;
	curve25519_signature signature;
	size_t public_key_len;
	size_t message_len;
	size_t signature_len;
	bool result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sss",
				&public_key, &public_key_len,
				&message, &message_len,
				&signature, &signature_len) == FAILURE) {
		return;
	}

	result = waves_verify_message((const curve25519_public_key *)public_key,
				(const unsigned char *)message, message_len,
				signature);
	if (!result) {
		RETURN_FALSE;
	}
	RETVAL_TRUE;
}
/*}}}*/

/**
* {{{ proto string|null waves_seed_to_address(string $key, string $network_byte)
*/
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

	if (network_byte_len != 1) {
		php_error_docref(NULL, E_ERROR,
				"Network byte length expected to be one byte long, got %ld bytes",
				network_byte_len);
		return;
	}

	out = emalloc(WAVES_ADDRESS_BYTES);
	if (!out) {
		/* TODO: Throw an exception */
		php_error_docref(NULL, E_ERROR,
				"Failed to allocate %ld bytes", WAVES_ADDRESS_BYTES);
		return;
	}

	waves_seed_to_address((const unsigned char *)key, (unsigned char)network_byte[0], (unsigned char *)out);
	RETURN_STRINGL(out, WAVES_ADDRESS_BYTES);
}
/*}}}*/

/**
* {{{ proto string|null waves_public_key_to_address(string $public_key, string $network_byte)
*/
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

	if (network_byte_len != 1) {
		php_error_docref(NULL, E_ERROR,
				"Network byte length expected to be one byte long, got %ld bytes",
				network_byte_len);
		return;
	}
	if (public_key_len != 32) {
		/* TODO: throw exception */
		php_error_docref(NULL, E_ERROR,
				"Public key must be 32 bytes in length, got %d bytes",
				public_key_len);
		return;
	}

	out = emalloc(WAVES_ADDRESS_BYTES);
	if (!out) {
		/* TODO: Throw an exception */
		php_error_docref(NULL, E_ERROR,
				"Failed to allocate %ld bytes", WAVES_ADDRESS_BYTES);
		return;
	}

	waves_public_key_to_address((const unsigned char *)public_key, (unsigned char)network_byte[0], (unsigned char *)out);
	RETURN_STRINGL(out, WAVES_ADDRESS_BYTES);
}
/*}}}*/

/* API }}}*/

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 sts=4 fdm=marker
 * vim<600: noet sw=4 ts=4 sts=4
 */
