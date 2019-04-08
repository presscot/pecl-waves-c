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
#ifndef PHP_WAVES_COMMON_H
#define PHP_WAVES_COMMON_H

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <php.h>
#include <php_ini.h>
#include <ext/standard/info.h>

/* zend_fcall_info.symbol_table removed from PHP 7.1.x */
#if PHP_VERSION_ID < 70100
# define HAVE_PHP_ZEND_FCALL_INFO_SYMBOL_TABLE 1
# define HAVE_PHP_ZEND_FCALL_INFO_FUNCTION_TABLE 1
#endif

#include <waves/b58.h>
#include <waves/crypto.h>

#ifdef ZTS
# include <TSRM.h>
#endif

#endif /* PHP_WAVES_COMMON_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
