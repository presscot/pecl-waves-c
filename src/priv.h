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

extern zend_class_entry *php_waves_exception_ce;
extern const zend_function_entry waves_functions[];

#define CHECK_NETWORK_BYTE_LEN(x) do {                                                   \
	if ((x) != 1) {                                                                      \
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0,                      \
				"Network byte length expected to be one byte long, got %ld bytes", (x)); \
	}                                                                                    \
} while (0)

#define CHECK_PRIVATE_KEY_LEN(x) do {                                    \
	if ((x) != WAVES_PRIVATE_KEY_BYTES) {                                \
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0,      \
				"Private key must be %ld bytes in length, got %d bytes", \
				WAVES_PRIVATE_KEY_BYTES, (x));                           \
		return;                                                          \
	}                                                                    \
} while (0)

#define CHECK_PUBLIC_KEY_LEN(x) do {                                    \
	if ((x) != WAVES_PUBLIC_KEY_BYTES) {                                \
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0,     \
				"Public key must be %ld bytes in length, got %d bytes", \
				WAVES_PUBLIC_KEY_BYTES, (x));                           \
		return;                                                         \
	}                                                                   \
} while (0)

#define WAVES_EMALLOC(__res, __len) do {                      \
	(__res) = emalloc((__len));                               \
	if (!(__res)) {                                           \
		zend_throw_exception_ex(php_waves_get_exception(), 0, \
				"Failed to allocate %ld bytes", (__len));     \
		return;                                               \
	}                                                         \
} while (0)

#endif /* PHP_WAVES_PRIV_H */
/*
 * vim600: fdm=marker
 * vim: noet sts=4 sw=4 ts=4
 */
