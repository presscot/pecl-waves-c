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

/*{{{ proto WavesAddress::__construct(string $address)
 * Throws InvalidArgumentException */
PHP_METHOD(WavesAddress, __construct)
{
	char *address;
	size_t address_len;
	php_waves_address_t *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s",
				&address, &address_len) == FAILURE) {
		return;
	}

	CHECK_ADDRESS_LEN(address_len);

	php_waves_sc_clamp((unsigned char *)address);

	intern = php_waves_address_object_fetch(Z_OBJ_P(getThis()));
	PHP_WAVES_ASSERT(intern);
	memcpy(intern->address, address, address_len);
	if (address_len < WAVES_ADDRESS_BYTES) {
		memset(intern->address + address_len, 0, WAVES_ADDRESS_BYTES - address_len);
	}
}/*}}}*/

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 sts=4 fdm=marker
 * vim<600: noet sw=4 ts=4 sts=4
 */
