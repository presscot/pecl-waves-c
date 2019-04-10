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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <php.h>
#include <php_ini.h>
#include <ext/standard/info.h>

#include "fe.h"
#include "priv.h"

/* {{{ ARG_INFO */
ZEND_BEGIN_ARG_INFO_EX(arginfo_waves_secure_hash, 0, 0, 1)
	ZEND_ARG_INFO(0, message)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_waves_sign_message, 0, 0, 2)
	ZEND_ARG_INFO(0, private_key)
	ZEND_ARG_INFO(0, message)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_waves_sign_message_custom_random, 0, 0, 3)
	ZEND_ARG_INFO(0, private_key)
	ZEND_ARG_INFO(0, message)
	ZEND_ARG_INFO(0, random)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_waves_base58_encode_decode, 0, 0, 1)
	ZEND_ARG_INFO(0, input)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_waves_verify_message, 0, 0, 3)
	ZEND_ARG_INFO(0, public_key)
	ZEND_ARG_INFO(0, message)
	ZEND_ARG_INFO(0, signature)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_waves_seed_to_address, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, network_byte)
ZEND_END_ARG_INFO()
#if 0
ZEND_BEGIN_ARG_INFO_EX(arginfo_waves_private_key_arg, 0, 0, 1)
	ZEND_ARG_INFO(0, private_key)
ZEND_END_ARG_INFO()
#endif
/* ARG_INFO }}} */

/* {{{ waves_functions[] */
const zend_function_entry waves_functions[] = {
	PHP_FE(waves_secure_hash, arginfo_waves_secure_hash)
	PHP_FE(waves_sign_message, arginfo_waves_sign_message)
	PHP_FE(waves_sign_message_custom_random, arginfo_waves_sign_message_custom_random)
	PHP_FE(waves_base58_encode, arginfo_waves_base58_encode_decode)
	PHP_FE(waves_base58_decode, arginfo_waves_base58_encode_decode)
	PHP_FE(waves_verify_message, arginfo_waves_verify_message)
	PHP_FE(waves_seed_to_address, arginfo_waves_seed_to_address)
	PHP_FE(waves_public_key_to_address, arginfo_waves_seed_to_address)
#if 0
	PHP_FE(waves_generate_public_key, arginfo_waves_private_key_arg)
#endif
	PHP_FE_END /* Must be the last line in waves_functions[] */
};
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
