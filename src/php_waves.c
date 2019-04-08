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
#include "src/common.h"
#include "zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"

extern const zend_function_entry waves_functions[];

zend_class_entry *php_event_exception_ce;

static const zend_module_dep waves_deps[] = {
	ZEND_MOD_END
};

/* {{{ waves_module_entry */
zend_module_entry waves_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL,
	waves_deps,
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

/* {{{ proto waves_secure_hash(string message) */
PHP_FUNCTION(waves_secure_hash)
{
	php_printf("%s\n", __func__);
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
