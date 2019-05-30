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
#ifndef PHP_WAVES_PRIV_H
#define PHP_WAVES_PRIV_H
#include "common.h"

extern zend_class_entry *php_waves_exception_ce;
extern zend_class_entry *php_waves_address_ce;
extern zend_class_entry *php_waves_public_key_ce;
extern zend_class_entry *php_waves_signature_ce;
extern zend_class_entry *php_waves_private_key_ce;

extern const zend_function_entry waves_functions[];
extern const zend_function_entry php_waves_address_ce_functions[];
extern const zend_function_entry php_waves_public_key_ce_functions[];
extern const zend_function_entry php_waves_signature_ce_functions[];
extern const zend_function_entry php_waves_private_key_ce_functions[];

#define CHECK_NETWORK_BYTE_LEN(x) do {                                                         \
	if ((x) != 1) {                                                                            \
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0,                            \
				"Network byte length expected to be one byte long, got %ld bytes", ((long)x)); \
	}                                                                                          \
} while (0)

#define CHECK_ADDRESS_LEN(x) do {                                              \
	if ((x) != WAVES_ADDRESS_BYTES) {                                          \
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0,            \
				"Address length expected to be %ld bytes long, got %ld bytes", \
				(long)WAVES_ADDRESS_BYTES, ((long)x));                         \
	}                                                                          \
} while (0)

#define CHECK_PRIVATE_KEY_LEN(x) do {                                     \
	if ((x) != WAVES_PRIVATE_KEY_BYTES) {                                 \
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0,       \
				"Private key must be %ld bytes in length, got %ld bytes", \
				(long)WAVES_PRIVATE_KEY_BYTES, ((long)x));                \
		return;                                                           \
	}                                                                     \
} while (0)

#define CHECK_PUBLIC_KEY_LEN(x) do {                                     \
	if ((x) != WAVES_PUBLIC_KEY_BYTES) {                                 \
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0,      \
				"Public key must be %ld bytes in length, got %ld bytes", \
				(long)WAVES_PUBLIC_KEY_BYTES, ((long)x));                \
		return;                                                          \
	}                                                                    \
} while (0)

#define CHECK_SIGNATURE_LEN(x) do {                                              \
	if ((x) != WAVES_SIGNATURE_BYTES) {                                          \
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0,              \
				"Signature length expected to be %ld bytes long, got %ld bytes", \
				(long)WAVES_SIGNATURE_BYTES, ((long)x));                         \
	}                                                                            \
} while (0)

#define WAVES_EMALLOC(__res, __len) do {                        \
	(__res) = emalloc((__len));                                 \
	if (!(__res)) {                                             \
		zend_throw_exception_ex(php_waves_get_exception(), 0,   \
				"Failed to allocate %ld bytes", ((long)__len)); \
		return;                                                 \
	}                                                           \
} while (0)

#define PHP_WAVES_OBJECT_TAIL \
	HashTable   *prop_handler; \
	zend_object  zo


 /*{{{ php_waves_sc_clamp
  * Sets/unsets the bits that should be set/unset in a valid private/public key.
  * From libwaves_c. */
static zend_always_inline void php_waves_sc_clamp(unsigned char* a)
{
  a[0] &= 248;
  a[31] &= 127;
  a[31] |= 64;
}/*}}}*/


/*{{{ Class internal data */

/* WavesAddress class internal data */
typedef struct _php_waves_address_t {
	char address[WAVES_ADDRESS_BYTES];
	PHP_WAVES_OBJECT_TAIL;
} php_waves_address_t;

/* WavesPublicKey class internal data */
typedef struct _php_waves_public_key_t {
	char key[WAVES_PUBLIC_KEY_BYTES];
	PHP_WAVES_OBJECT_TAIL;
} php_waves_public_key_t;

/* WavesSignature class internal data */
typedef struct _php_waves_signature_t {
	char signature[WAVES_SIGNATURE_BYTES];
	PHP_WAVES_OBJECT_TAIL;
} php_waves_signature_t;

/* WavesPrivateKey class internal data */
typedef struct _php_waves_private_key_t {
	char key[WAVES_PRIVATE_KEY_BYTES];
	PHP_WAVES_OBJECT_TAIL;
} php_waves_private_key_t;
/* Class internal data }}}*/

/*{{{ Object helpers */

/*{{{ php_waves_address_object_fetch
 * Returns internal structure of a WavesAddress class object. */
static zend_always_inline php_waves_address_t *php_waves_address_object_fetch(zend_object *obj)
{
	return (EXPECTED(obj) ? (php_waves_address_t *)((char *)obj - XtOffsetOf(php_waves_address_t, zo)) : NULL);
}/*}}}*/

/*{{{ php_waves_public_key_object_fetch
 * Returns internal structure of a WavesPublicKey class object. */
static zend_always_inline php_waves_public_key_t *php_waves_public_key_object_fetch(zend_object *obj)
{
	return (EXPECTED(obj) ? (php_waves_public_key_t *)((char *)obj - XtOffsetOf(php_waves_public_key_t, zo)) : NULL);
}/*}}}*/

/*{{{ php_waves_signature_object_fetch
 * Returns internal structure of a WavesSignature class object. */
static zend_always_inline php_waves_signature_t *php_waves_signature_object_fetch(zend_object *obj)
{
	return (EXPECTED(obj) ? (php_waves_signature_t *)((char *)obj - XtOffsetOf(php_waves_signature_t, zo)) : NULL);
}/*}}}*/

/*{{{ php_waves_private_key_object_fetch
 * Returns internal structure of a WavesPrivateKey class object. */
static zend_always_inline php_waves_private_key_t *php_waves_private_key_object_fetch(zend_object *obj)
{
	return (EXPECTED(obj) ? (php_waves_private_key_t *)((char *)obj - XtOffsetOf(php_waves_private_key_t, zo)) : NULL);
}/*}}}*/

/* Object helpers }}}*/

/* {{{Properties */

/* Property read handler */
typedef zval *(*php_waves_prop_read_t)(void *obj, zval *retval);
/* Property write handler */
typedef int (*php_waves_prop_write_t)(void *obj, zval *newval);
/* Property handler for property pointer-to-pointer retrieval */
typedef zval *(*php_waves_prop_get_ptr_ptr_t)(void *obj);

/* Property entry */
typedef struct _php_waves_property_entry_t {
	const char                   *name;
	size_t                        name_length;
	php_waves_prop_read_t         read_func;
	php_waves_prop_write_t        write_func;
	php_waves_prop_get_ptr_ptr_t  get_ptr_ptr_func;
} php_waves_property_entry_t;

/* Property entry handler */
typedef struct _php_waves_prop_handler_t {
	zend_string                  *name;
	php_waves_prop_read_t         read_func;
	php_waves_prop_write_t        write_func;
	php_waves_prop_get_ptr_ptr_t  get_ptr_ptr_func;
} php_waves_prop_handler_t;

extern const php_waves_property_entry_t waves_address_property_entries[];
extern const php_waves_property_entry_t waves_public_key_property_entries[];
extern const php_waves_property_entry_t waves_signature_property_entries[];
extern const php_waves_property_entry_t waves_private_key_property_entries[];

/*Properties }}}*/

#endif /* PHP_WAVES_PRIV_H */
/*
 * vim600: fdm=marker
 * vim: noet sts=4 sw=4 ts=4
 */
