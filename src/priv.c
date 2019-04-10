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
#include "zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"
#include "priv.h"

zend_class_entry *php_waves_get_exception_base(int root)/*{{{*/
{
	if (!root) {
		return spl_ce_RuntimeException;
	}
	return zend_ce_exception;
}/*}}}*/

zend_class_entry *php_waves_get_exception(void)/*{{{*/
{
	return php_waves_exception_ce;
}/*}}}*/

/*
 * vim600: fdm=marker
 * vim: noet sts=4 sw=4 ts=4
 */
