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
#include "common.h"
#include "priv.h"

#define PHP_WAVES_PROP_REQUIRE(x) do { \
	if (UNEXPECTED(!(x))) {            \
		return NULL;                   \
	}                                  \
} while (0);

/*{{{ WavesAddress properties */
static zval *waves_address_address_prop_read(void *obj, zval *retval)/*{{{*/
{
	php_waves_address_t *a = (php_waves_address_t *)obj;
	PHP_WAVES_PROP_REQUIRE(a->address);
	ZVAL_STRINGL(retval, a->address, WAVES_ADDRESS_BYTES);
	return retval;
}/*}}}*/

const php_waves_property_entry_t waves_address_property_entries[]  = {
	{"address", sizeof("address") - 1, waves_address_address_prop_read, NULL, NULL},
	{NULL, 0, NULL, NULL, NULL}
};
/* WavesAddress properties }}}*/

/*{{{ WavesPublicKey properties */
static zval *waves_public_key_public_key_prop_read(void *obj, zval *retval)/*{{{*/
{
	php_waves_public_key_t *a = (php_waves_public_key_t *)obj;
	PHP_WAVES_PROP_REQUIRE(a->key);
	ZVAL_STRINGL(retval, a->key, WAVES_PUBLIC_KEY_BYTES);
	return retval;
}/*}}}*/

const php_waves_property_entry_t waves_public_key_property_entries[]  = {/*{{{*/
	{"key", sizeof("key") - 1, waves_public_key_public_key_prop_read, NULL, NULL},
	{NULL, 0, NULL, NULL, NULL}
};/*}}}*/
/* WavesPublicKey properties }}}*/

/*{{{ WavesSignature properties */
static zval *waves_signature_signature_prop_read(void *obj, zval *retval)/*{{{*/
{
	php_waves_signature_t *a = (php_waves_signature_t *)obj;
	PHP_WAVES_PROP_REQUIRE(a->signature);
	ZVAL_STRINGL(retval, a->signature, WAVES_SIGNATURE_BYTES);
	return retval;
}/*}}}*/

const php_waves_property_entry_t waves_signature_property_entries[]  = {/*{{{*/
	{"signature", sizeof("signature") - 1, waves_signature_signature_prop_read, NULL, NULL},
	{NULL, 0, NULL, NULL, NULL}
};/*}}}*/
/* WavesSignature properties }}}*/

/*{{{ WavesPrivateKey properties */
static zval *waves_private_key_private_key_prop_read(void *obj, zval *retval)/*{{{*/
{
	php_waves_private_key_t *a = (php_waves_private_key_t *)obj;
	PHP_WAVES_PROP_REQUIRE(a->key);
	ZVAL_STRINGL(retval, a->key, WAVES_PRIVATE_KEY_BYTES);
	return retval;
}/*}}}*/

const php_waves_property_entry_t waves_private_key_property_entries[]  = {/*{{{*/
	{"key", sizeof("key") - 1, waves_private_key_private_key_prop_read, NULL, NULL},
	{NULL, 0, NULL, NULL, NULL}
};/*}}}*/
/* WavesPrivateKey properties }}}*/

/*
 * vim600: fdm=marker
 * vim: noet sts=4 sw=4 ts=4
 */
