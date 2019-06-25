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

/*{{{ proto WavesDataTransaction::__construct(string $key) */
PHP_METHOD(WavesDataTransaction, __construct)
{
	char *key;
	size_t key_len;
#if 0
	php_waves_data_tx_t *intern;
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s",
				&key, &key_len) == FAILURE) {
		return;
	}

#if 0
	intern = php_waves_data_tx_object_fetch(Z_OBJ_P(getThis()));
	PHP_WAVES_ASSERT(intern);
#endif
}/*}}}*/

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 sts=4 fdm=marker
 * vim<600: noet sw=4 ts=4 sts=4
 */
