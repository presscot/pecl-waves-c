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
/*{{{ Procedural style API */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_waves_secure_hash, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_waves_sign_message, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, private_key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_INFO(0, random)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_waves_base58_encode_decode, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, input, IS_STRING, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_waves_verify_message, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, public_key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, signature, IS_STRING, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_waves_public_key_to_address, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, network_byte, IS_STRING, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_waves_private_key_arg, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, private_key, IS_STRING, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_waves_generate_private_key, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, seed, IS_STRING, 0)
ZEND_END_ARG_INFO()
/* Procedural style API }}}*/
/*{{{ WavesAddress ARGINFO */
ZEND_BEGIN_ARG_INFO_EX(arginfo_waves_address__construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, address, IS_STRING, 0)
ZEND_END_ARG_INFO();
/* WavesAddress ARGINFO }}}*/
/*{{{ WavesPublicKey ARGINFO */
ZEND_BEGIN_ARG_INFO_EX(arginfo_waves_public_key__construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, public_key, IS_STRING, 0)
ZEND_END_ARG_INFO();
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_waves_public_key_get_address, 0, 1, WavesAddress, 0)
	ZEND_ARG_TYPE_INFO(0, network_byte, IS_STRING, 0)
ZEND_END_ARG_INFO();
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_waves_public_key_verify, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, signature, WavesSignature, 0)
ZEND_END_ARG_INFO();
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_waves_public_key_from_private_key, 0, 1, WavesPublicKey, 0)
	ZEND_ARG_OBJ_INFO(0, private_key, WavesPrivateKey, 0)
ZEND_END_ARG_INFO();
/* WavesPublicKey ARGINFO }}}*/
/*{{{ WavesSignature ARGINFO */
ZEND_BEGIN_ARG_INFO_EX(arginfo_waves_signature__construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, signature, IS_STRING, 0)
ZEND_END_ARG_INFO();
/* WavesSignature ARGINFO }}}*/
/*{{{ WavesPrivateKey ARGINFO */
ZEND_BEGIN_ARG_INFO_EX(arginfo_waves_private_key__construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, private_key, IS_STRING, 0)
ZEND_END_ARG_INFO();
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_waves_private_key_sign, 0, 1, WavesSignature, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_INFO(0, random)
ZEND_END_ARG_INFO();
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_waves_private_key_from_seed, 0, 1, WavesPrivateKey, 0)
	ZEND_ARG_TYPE_INFO(0, seed, IS_STRING, 0)
ZEND_END_ARG_INFO();
/* WavesPrivateKey ARGINFO }}}*/
/*{{{ WavesDataTransaction ARGINFO */
ZEND_BEGIN_ARG_INFO_EX(arginfo_waves_data_tx__construct, 0, 0, 0)
ZEND_END_ARG_INFO();
/* WavesDataTransaction ARGINFO }}}*/
/* ARG_INFO }}} */

/* {{{ waves_functions[] */
const zend_function_entry waves_functions[] = {
	PHP_FE(waves_secure_hash, arginfo_waves_secure_hash)
	PHP_FE(waves_sign_message, arginfo_waves_sign_message)
	PHP_FE(waves_base58_encode, arginfo_waves_base58_encode_decode)
	PHP_FE(waves_base58_decode, arginfo_waves_base58_encode_decode)
	PHP_FE(waves_verify_message, arginfo_waves_verify_message)
	PHP_FE(waves_public_key_to_address, arginfo_waves_public_key_to_address)
	PHP_FE(waves_generate_public_key, arginfo_waves_private_key_arg)
	PHP_FE(waves_generate_private_key, arginfo_waves_generate_private_key)
	PHP_FE_END
};
/* }}} */
const zend_function_entry php_waves_address_ce_functions[] = {/*{{{*/
	PHP_ME(WavesAddress, __construct, arginfo_waves_address__construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_FE_END
};/*}}}*/
const zend_function_entry php_waves_public_key_ce_functions[] = {/*{{{*/
	PHP_ME(WavesPublicKey, __construct, arginfo_waves_public_key__construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(WavesPublicKey, getAddress, arginfo_waves_public_key_get_address, ZEND_ACC_PUBLIC)
	PHP_ME(WavesPublicKey, verify, arginfo_waves_public_key_verify, ZEND_ACC_PUBLIC)
	PHP_ME(WavesPublicKey, fromPrivateKey, arginfo_waves_public_key_from_private_key, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_FE_END
};/*}}}*/
const zend_function_entry php_waves_signature_ce_functions[] = {/*{{{*/
	PHP_ME(WavesSignature, __construct, arginfo_waves_signature__construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_FE_END
};/*}}}*/
const zend_function_entry php_waves_private_key_ce_functions[] = {/*{{{*/
	PHP_ME(WavesPrivateKey, __construct, arginfo_waves_private_key__construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(WavesPrivateKey, sign, arginfo_waves_private_key_sign, ZEND_ACC_PUBLIC)
	PHP_ME(WavesPrivateKey, fromSeed, arginfo_waves_private_key_from_seed, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_FE_END
};/*}}}*/
const zend_function_entry php_waves_data_tx_ce_functions[] = {/*{{{*/
	PHP_ME(WavesPrivateKey, __construct, arginfo_waves_data_tx__construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_FE_END
};/*}}}*/

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
