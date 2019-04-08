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

#ifndef PHP_WAVES_H
#define PHP_WAVES_H

#define PHP_WAVES_VERSION "1.0.0"

extern zend_module_entry waves_module_entry;
#define phpext_waves_ptr &waves_module_entry

#include "src/common.h"

zend_class_entry *php_event_get_exception_base(int root);
zend_class_entry *php_event_get_exception(void);

PHP_MINIT_FUNCTION(waves);
PHP_MSHUTDOWN_FUNCTION(waves);
PHP_RINIT_FUNCTION(waves);
PHP_RSHUTDOWN_FUNCTION(waves);
PHP_MINFO_FUNCTION(waves);

#if defined(COMPILE_DL_EVENT)
ZEND_TSRMLS_CACHE_EXTERN();
#endif

#ifdef ZTS
# define WAVES_G(v) TSRMG(event_globals_id, zend_event_globals *, v)
#else
# define WAVES_G(v) (event_globals.v)
#endif

#ifdef PHP_WAVES_DEBUG
# define PHP_WAVES_ASSERT(x) assert(x)
#else
# define PHP_WAVES_ASSERT(x)
#endif

#endif /* PHP_WAVES_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
