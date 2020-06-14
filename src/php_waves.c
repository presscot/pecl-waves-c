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
#include "php_waves.h"
#include "zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"
#include "priv.h"

#include <php/ext/hash/php_hash.h>

/* Registers a PHP class */
#define PHP_WAVES_REGISTER_CLASS(name, create_func, ce, ce_functions) \
{                                                                     \
	zend_class_entry tmp_ce;                                          \
	INIT_CLASS_ENTRY(tmp_ce, name, ce_functions);                     \
	tmp_ce.create_object = create_func;                               \
	ce = zend_register_internal_class(&tmp_ce);                       \
}

/* Fills a HashTable of class properties `a` from an array `b` of type
 * php_waves_property_entry_t[]. The last item is supposed to be a sentinel
 * with all members NULL'd or zeroed out. */
#define PHP_WAVES_ADD_CLASS_PROPERTIES(a, b)                                 \
{                                                                            \
	int i = 0;                                                               \
	while (b[i].name != NULL) {                                              \
		php_waves_add_property((a), (b)[i].name, (b)[i].name_length,         \
				(php_waves_prop_read_t)(b)[i].read_func,                     \
				(php_waves_prop_write_t)(b)[i].write_func,                   \
				(php_waves_prop_get_ptr_ptr_t)(b)[i].get_ptr_ptr_func); \
		i++;                                                                 \
	}                                                                        \
}

/* Allocates memory for a PHP class object and initializes its properties. */
#define PHP_WAVES_OBJ_ALLOC(obj, ce, t)                                \
	do {                                                               \
		obj = ecalloc(1, sizeof(t) + zend_object_properties_size(ce)); \
		obj->prop_handler = find_prop_handler(&classes, ce);           \
		zend_object_std_init(&obj->zo, ce);                            \
		object_properties_init(&obj->zo, ce);                          \
	} while (0)

/* WavesException */
zend_class_entry *php_waves_exception_ce;
/* WavesAddress */
zend_class_entry *php_waves_address_ce;
/* WavesPublicKey */
zend_class_entry *php_waves_public_key_ce;
/* WavesSignature */
zend_class_entry *php_waves_signature_ce;
/* WavesPrivateKey */
zend_class_entry *php_waves_private_key_ce;

/* All PHP classes registered by this extension */
static HashTable classes;

/* Properties of WavesAddress class */
static HashTable waves_address_properties;
/* Properties of WavesPublicKey class */
static HashTable waves_public_key_properties;
/* Properties of WavesSignature class */
static HashTable waves_signature_properties;
/* Properties of WavesPrivateKey class */
static HashTable waves_private_key_properties;

/* WavesAddress class object handlers */
static zend_object_handlers waves_address_object_handlers;
/* WavesPublicKey class object handlers */
static zend_object_handlers waves_public_key_object_handlers;
/* WavesSignature class object handlers */
static zend_object_handlers waves_signature_object_handlers;
/* WavesPrivateKey class object handlers */
static zend_object_handlers waves_private_key_object_handlers;

static zend_always_inline HashTable *find_prop_handler(HashTable *classes, zend_class_entry *ce)/*{{{*/
{
	zend_class_entry *ce_parent = ce;
	while (ce_parent->type != ZEND_INTERNAL_CLASS && ce_parent->parent != NULL) {
		ce_parent = ce_parent->parent;
	}
	return zend_hash_find_ptr(classes, ce_parent->name);
} /*}}}*/

/*{{{ waves_address_object_create
 * Allocates new WavesAddress class object, its properties and handlers. */
static zend_object *waves_address_object_create(zend_class_entry *ce)
{
	php_waves_address_t *intern;
	PHP_WAVES_OBJ_ALLOC(intern, ce, php_waves_address_t);
	intern->zo.handlers = &waves_address_object_handlers;
	return &intern->zo;
}/*}}}*/

/*{{{ waves_public_key_object_create
 * Allocates new WavesPublicKey class object, its properties and handlers. */
static zend_object *waves_public_key_object_create(zend_class_entry *ce)
{
	php_waves_public_key_t *intern;
	PHP_WAVES_OBJ_ALLOC(intern, ce, php_waves_public_key_t);
	intern->zo.handlers = &waves_public_key_object_handlers;
	return &intern->zo;
}/*}}}*/

/*{{{ waves_private_key_object_create
 * Allocates new WavesPrivateKey class object, its properties and handlers. */
static zend_object *waves_private_key_object_create(zend_class_entry *ce)
{
	php_waves_private_key_t *intern;
	PHP_WAVES_OBJ_ALLOC(intern, ce, php_waves_private_key_t);
	intern->zo.handlers = &waves_private_key_object_handlers;
	return &intern->zo;
}/*}}}*/

/*{{{ waves_signature_object_create
 * Allocates new WavesSignature class object, its properties and handlers. */
static zend_object *waves_signature_object_create(zend_class_entry *ce)
{
	php_waves_signature_t *intern;
	PHP_WAVES_OBJ_ALLOC(intern, ce, php_waves_signature_t);
	intern->zo.handlers = &waves_signature_object_handlers;
	return &intern->zo;
}/*}}}*/

/*{{{ Common class object handlers */

static void free_prop_handler(zval *el)/*{{{*/
{
	pefree(Z_PTR_P(el), 1);
}/*}}}*/

static zval *read_property_default(void *obj, zval *retval)/*{{{*/
{
	ZVAL_NULL(retval);
	php_error_docref(NULL, E_ERROR, "Cannot read property");
	return retval;
}/*}}}*/

static int write_property_default(void *obj, zval *newval)/*{{{*/
{
	php_error_docref(NULL, E_ERROR, "Cannot write property");
	return FAILURE;
}/*}}}*/

static zval *read_property(zval *object, zval *member, int type, void **cache_slot, zval *rv, void *obj, HashTable *prop_handler)/*{{{*/
{
	zval                      tmp_member;
	zval                     *retval;
	php_waves_prop_handler_t *hnd        = NULL;

	if (Z_TYPE_P(member) != IS_STRING) {
		ZVAL_COPY(&tmp_member, member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

	if (prop_handler != NULL) {
		hnd = zend_hash_find_ptr(prop_handler, Z_STR_P(member));
	}

	if (hnd) {
		retval = hnd->read_func(obj, rv);
		if (retval == NULL) {
			retval = &EG(uninitialized_zval);
		}
	} else {
		const zend_object_handlers *std_hnd = zend_get_std_object_handlers();
		retval = std_hnd->read_property(object, member, type, cache_slot, rv);
	}

	if (member == &tmp_member) {
		zval_dtor(&tmp_member);
	}

	return retval;
}/*}}}*/

static void write_property(zval *object, zval *member, zval *value, void **cache_slot, void *obj, HashTable *prop_handler)/*{{{*/
{
	zval                      tmp_member;
	php_waves_prop_handler_t *hnd        = NULL;

	if (Z_TYPE_P(member) != IS_STRING) {
		ZVAL_COPY(&tmp_member, member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

	if (prop_handler != NULL) {
	    hnd = zend_hash_find_ptr(prop_handler, Z_STR_P(member));
	}

	if (hnd) {
		hnd->write_func(obj, value);
	} else {
		const zend_object_handlers *std_hnd = zend_get_std_object_handlers();
	    std_hnd->write_property(object, member, value, cache_slot);
	}

	if (member == &tmp_member) {
		zval_dtor(member);
	}
}/*}}}*/

static void php_waves_add_property(HashTable *h, const char *name, size_t name_len, php_waves_prop_read_t read_func, php_waves_prop_write_t write_func, php_waves_prop_get_ptr_ptr_t get_ptr_ptr_func) {/*{{{*/
	php_waves_prop_handler_t p;

	p.name             = zend_string_init(name, name_len, 1);
	p.read_func        = read_func ? read_func : read_property_default;
	p.write_func       = write_func ? write_func: write_property_default;
	p.get_ptr_ptr_func = get_ptr_ptr_func;
	zend_hash_add_mem(h, p.name, &p, sizeof(php_waves_prop_handler_t));
	zend_string_release(p.name);
}/*}}}*/

static int object_has_property(zval *object, zval *member, int has_set_exists, void **cache_slot, void *obj, HashTable *prop_handler)/*{{{*/
{
	php_waves_prop_handler_t *p;
	int                       ret = 0;

	if ((p = zend_hash_find_ptr(prop_handler, Z_STR_P(member))) != NULL) {
		switch (has_set_exists) {
			case 2:
				ret = 1;
				break;
			case 1: {
						zval rv;
						zval *value = read_property(object, member, BP_VAR_IS, cache_slot, &rv, obj, prop_handler);
						if (value != &EG(uninitialized_zval)) {
							convert_to_boolean(value);
							ret = Z_TYPE_P(value) == IS_TRUE ? 1 : 0;
						}
						break;
					}
			case 0:{
					   zval rv;
					   zval *value = read_property(object, member, BP_VAR_IS, cache_slot, &rv, obj, prop_handler);
					   if (value != &EG(uninitialized_zval)) {
						   ret = Z_TYPE_P(value) != IS_NULL ? 1 : 0;
						   zval_ptr_dtor(value);
					   }
					   break;
				   }
			default:
				   php_error_docref(NULL, E_WARNING, "Invalid value for has_set_exists");
		}
	} else {
		const zend_object_handlers *std_hnd = zend_get_std_object_handlers();
		ret = std_hnd->has_property(object, member, has_set_exists, cache_slot);
	}

	return ret;
}/*}}}*/

static HashTable *object_get_debug_info(zval *object, int *is_temp, void *obj, HashTable *prop_handler)/*{{{*/
{
	HashTable                *props = prop_handler;
	HashTable                *retval;
	php_waves_prop_handler_t *entry;

	ALLOC_HASHTABLE(retval);
	ZEND_INIT_SYMTABLE_EX(retval, zend_hash_num_elements(props) + 1, 0);

	ZEND_HASH_FOREACH_PTR(props, entry) {
		zval rv, member;
		zval *value;
		ZVAL_STR(&member, entry->name);
		value = read_property(object, &member, BP_VAR_IS, 0, &rv, obj, prop_handler);
		if (value != &EG(uninitialized_zval)) {
			zend_hash_add(retval, Z_STR(member), value);
		}
	} ZEND_HASH_FOREACH_END();

	return retval;
}/*}}}*/

static zval *get_property_ptr_ptr(zval *object, zval *member, int type, void **cache_slot, void *obj, HashTable *prop_handler)/*{{{*/
{
	zval                     *retval     = NULL;
	php_waves_prop_handler_t *hnd        = NULL;
	zval                      tmp_member;

	if (Z_TYPE_P(member) != IS_STRING) {
		ZVAL_COPY(&tmp_member, member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
		cache_slot = NULL;
	}

	if (prop_handler != NULL) {
		hnd = zend_hash_find_ptr(prop_handler, Z_STR_P(member));
	}

	if (hnd && hnd->get_ptr_ptr_func != NULL) {
		retval = hnd->get_ptr_ptr_func(obj);
	} else {
		const zend_object_handlers *std_hnd = zend_get_std_object_handlers();
		retval = std_hnd->get_property_ptr_ptr(object, member, type, cache_slot);
	}

	if (Z_ISUNDEF_P(retval)) {
		ZVAL_NULL(retval);
	}

	if (member == &tmp_member) {
		zval_dtor(member);
	}

	return retval;
}/*}}}*/

static HashTable *get_properties(zval *object, void *obj, HashTable *prop_handler)/*{{{*/
{
	HashTable                *props = zend_std_get_properties(object);
	php_waves_prop_handler_t *hnd;
	zend_string              *key;

	if (prop_handler == NULL) {
		return NULL;
	}

	ZEND_HASH_FOREACH_STR_KEY_PTR(prop_handler, key, hnd) {
		zval zret;
		if (hnd->read_func && hnd->read_func(obj, &zret)) {
			zend_hash_update(props, key, &zret);
		}
	} ZEND_HASH_FOREACH_END();

	return props;
}/*}}}*/

static HashTable *get_gc(zval *object, zval **gc_data, int *gc_count)/*{{{*/
{
	*gc_data = NULL;
	*gc_count = 0;
	return zend_std_get_properties(object);
}/*}}}*/

/* Common class objet handlers }}}*/


/*{{{ WavesAddress class object handlers */
#if 0
static void php_waves_address_free_obj(zend_object *object)/*{{{*/
{
	php_waves_address_t *intern = php_waves_address_object_fetch(object);
	PHP_WAVES_ASSERT(intern);

	zend_object_std_dtor(&intern->zo);
}/*}}}*/
#endif

static zval *php_waves_address_read_property(zval *object, zval *member, int type, void **cache_slot, zval *rv) {/*{{{*/
	php_waves_address_t *intern = php_waves_address_object_fetch(Z_OBJ_P(object));
	return (intern ? read_property(object, member, type, cache_slot, rv, (void *)intern, intern->prop_handler) : NULL);
}/*}}}*/

static void php_waves_address_write_property(zval *object, zval *member, zval *value, void **cache_slot)/*{{{*/
{
	php_waves_address_t *intern = php_waves_address_object_fetch(Z_OBJ_P(object));
	if (EXPECTED(intern)) {
		write_property(object, member, value, cache_slot, (void *)intern, intern->prop_handler);
	}
}/*}}}*/

static int php_waves_address_has_property(zval *object, zval *member, int has_set_exists, void **cache_slot)/*{{{*/
{
	php_waves_address_t *intern = php_waves_address_object_fetch(Z_OBJ_P(object));
	return (intern ? object_has_property(object, member, has_set_exists, cache_slot, (void *)intern, intern->prop_handler) : 0);
}/*}}}*/

static HashTable *php_waves_address_get_debug_info(zval *object, int *is_temp)/*{{{*/
{
	HashTable *retval;
	php_waves_address_t *intern = php_waves_address_object_fetch(Z_OBJ_P(object));
	if (EXPECTED(intern) && intern->prop_handler) {
		retval = object_get_debug_info(object, is_temp, (void *)intern, intern->prop_handler);
	} else {
		ALLOC_HASHTABLE(retval);
		ZEND_INIT_SYMTABLE_EX(retval, 1, 0);
	}
	*is_temp = 1;
	return retval;
}/*}}}*/

static zval *php_waves_address_get_property_ptr_ptr(zval *object, zval *member, int type, void **cache_slot)/*{{{*/
{
	php_waves_address_t *intern = php_waves_address_object_fetch(Z_OBJ_P(object));
	return (EXPECTED(intern) ? get_property_ptr_ptr(object, member, type, cache_slot, (void *)intern, intern->prop_handler) : NULL);
}/*}}}*/

static HashTable *php_waves_address_get_properties(zval *object)/*{{{*/
{
	HashTable *retval;
	php_waves_address_t *intern = php_waves_address_object_fetch(Z_OBJ_P(object));
	if (EXPECTED(intern)) {
		retval = get_properties(object, (void *)intern, intern->prop_handler);
	} else {
		ALLOC_HASHTABLE(retval);
	}
	return retval;
}/*}}}*/

/* WavesAddress class object handlers }}}*/
/*{{{ WavesPublicKey class object handlers */

#if 0
static void php_waves_public_key_free_obj(zend_object *object)/*{{{*/
{
	php_waves_public_key_t *intern = php_waves_public_key_object_fetch(object);
	PHP_WAVES_ASSERT(intern);

	zend_object_std_dtor(&intern->zo);
}/*}}}*/
#endif

static zval *php_waves_public_key_read_property(zval *object, zval *member, int type, void **cache_slot, zval *rv) {/*{{{*/
	php_waves_public_key_t *intern = php_waves_public_key_object_fetch(Z_OBJ_P(object));
	return (intern ? read_property(object, member, type, cache_slot, rv, (void *)intern, intern->prop_handler) : NULL);
}/*}}}*/

static void php_waves_public_key_write_property(zval *object, zval *member, zval *value, void **cache_slot)/*{{{*/
{
	php_waves_public_key_t *intern = php_waves_public_key_object_fetch(Z_OBJ_P(object));
	if (EXPECTED(intern)) {
		write_property(object, member, value, cache_slot, (void *)intern, intern->prop_handler);
	}
}/*}}}*/

static int php_waves_public_key_has_property(zval *object, zval *member, int has_set_exists, void **cache_slot)/*{{{*/
{
	php_waves_public_key_t *intern = php_waves_public_key_object_fetch(Z_OBJ_P(object));
	return (intern ? object_has_property(object, member, has_set_exists, cache_slot, (void *)intern, intern->prop_handler) : 0);
}/*}}}*/

static HashTable * php_waves_public_key_get_debug_info(zval *object, int *is_temp)/*{{{*/
{
	HashTable *retval;
	php_waves_public_key_t *intern = php_waves_public_key_object_fetch(Z_OBJ_P(object));
	if (EXPECTED(intern) && intern->prop_handler) {
		retval = object_get_debug_info(object, is_temp, (void *)intern, intern->prop_handler);
	} else {
		ALLOC_HASHTABLE(retval);
		ZEND_INIT_SYMTABLE_EX(retval, 1, 0);
	}
	*is_temp = 1;
	return retval;
}/*}}}*/

static zval *php_waves_public_key_get_property_ptr_ptr(zval *object, zval *member, int type, void **cache_slot)/*{{{*/
{
	php_waves_public_key_t *intern = php_waves_public_key_object_fetch(Z_OBJ_P(object));
	return (EXPECTED(intern) ? get_property_ptr_ptr(object, member, type, cache_slot, (void *)intern, intern->prop_handler) : NULL);
}/*}}}*/

static HashTable *php_waves_public_key_get_properties(zval *object)/*{{{*/
{
	HashTable *retval;
	php_waves_public_key_t *intern = php_waves_public_key_object_fetch(Z_OBJ_P(object));
	if (EXPECTED(intern)) {
		retval = get_properties(object, (void *)intern, intern->prop_handler);
	} else {
		ALLOC_HASHTABLE(retval);
	}
	return retval;
}/*}}}*/

/* WavesPublicKey class object handlers }}}*/
/*{{{ WavesSignature class object handlers */

#if 0
static void php_waves_signature_free_obj(zend_object *object)/*{{{*/
{
	php_waves_signature_t *intern = php_waves_signature_object_fetch(object);
	PHP_WAVES_ASSERT(intern);

	zend_object_std_dtor(&intern->zo);
}/*}}}*/
#endif

static zval *php_waves_signature_read_property(zval *object, zval *member, int type, void **cache_slot, zval *rv) {/*{{{*/
	php_waves_signature_t *intern = php_waves_signature_object_fetch(Z_OBJ_P(object));
	return (intern ? read_property(object, member, type, cache_slot, rv, (void *)intern, intern->prop_handler) : NULL);
}/*}}}*/

static void php_waves_signature_write_property(zval *object, zval *member, zval *value, void **cache_slot)/*{{{*/
{
	php_waves_signature_t *intern = php_waves_signature_object_fetch(Z_OBJ_P(object));
	if (EXPECTED(intern)) {
		write_property(object, member, value, cache_slot, (void *)intern, intern->prop_handler);
	}
}/*}}}*/

static int php_waves_signature_has_property(zval *object, zval *member, int has_set_exists, void **cache_slot)/*{{{*/
{
	php_waves_signature_t *intern = php_waves_signature_object_fetch(Z_OBJ_P(object));
	return (intern ? object_has_property(object, member, has_set_exists, cache_slot, (void *)intern, intern->prop_handler) : 0);
}/*}}}*/

static HashTable * php_waves_signature_get_debug_info(zval *object, int *is_temp)/*{{{*/
{
	HashTable *retval;
	php_waves_signature_t *intern = php_waves_signature_object_fetch(Z_OBJ_P(object));
	if (EXPECTED(intern) && intern->prop_handler) {
		retval = object_get_debug_info(object, is_temp, (void *)intern, intern->prop_handler);
	} else {
		ALLOC_HASHTABLE(retval);
		ZEND_INIT_SYMTABLE_EX(retval, 1, 0);
	}
	*is_temp = 1;
	return retval;
}/*}}}*/

static zval *php_waves_signature_get_property_ptr_ptr(zval *object, zval *member, int type, void **cache_slot)/*{{{*/
{
	php_waves_signature_t *intern = php_waves_signature_object_fetch(Z_OBJ_P(object));
	return (EXPECTED(intern) ? get_property_ptr_ptr(object, member, type, cache_slot, (void *)intern, intern->prop_handler) : NULL);
}/*}}}*/

static HashTable *php_waves_signature_get_properties(zval *object)/*{{{*/
{
	HashTable *retval;
	php_waves_signature_t *intern = php_waves_signature_object_fetch(Z_OBJ_P(object));
	if (EXPECTED(intern)) {
		retval = get_properties(object, (void *)intern, intern->prop_handler);
	} else {
		ALLOC_HASHTABLE(retval);
	}
	return retval;
}/*}}}*/

/* WavesSignature class object handlers }}}*/
/*{{{ WavesPrivateKey class object handlers */

#if 0
static void php_waves_private_key_free_obj(zend_object *object)/*{{{*/
{
	php_waves_private_key_t *intern = php_waves_private_key_object_fetch(object);
	PHP_WAVES_ASSERT(intern);

	zend_object_std_dtor(&intern->zo);
}/*}}}*/
#endif

static zval *php_waves_private_key_read_property(zval *object, zval *member, int type, void **cache_slot, zval *rv) {/*{{{*/
	php_waves_private_key_t *intern = php_waves_private_key_object_fetch(Z_OBJ_P(object));
	return (intern ? read_property(object, member, type, cache_slot, rv, (void *)intern, intern->prop_handler) : NULL);
}/*}}}*/

static void php_waves_private_key_write_property(zval *object, zval *member, zval *value, void **cache_slot)/*{{{*/
{
	php_waves_private_key_t *intern = php_waves_private_key_object_fetch(Z_OBJ_P(object));
	if (EXPECTED(intern)) {
		write_property(object, member, value, cache_slot, (void *)intern, intern->prop_handler);
	}
}/*}}}*/

static int php_waves_private_key_has_property(zval *object, zval *member, int has_set_exists, void **cache_slot)/*{{{*/
{
	php_waves_private_key_t *intern = php_waves_private_key_object_fetch(Z_OBJ_P(object));
	return (intern ? object_has_property(object, member, has_set_exists, cache_slot, (void *)intern, intern->prop_handler) : 0);
}/*}}}*/

static HashTable * php_waves_private_key_get_debug_info(zval *object, int *is_temp)/*{{{*/
{
	HashTable *retval;
	php_waves_private_key_t *intern = php_waves_private_key_object_fetch(Z_OBJ_P(object));
	if (EXPECTED(intern) && intern->prop_handler) {
		retval = object_get_debug_info(object, is_temp, (void *)intern, intern->prop_handler);
	} else {
		ALLOC_HASHTABLE(retval);
		ZEND_INIT_SYMTABLE_EX(retval, 1, 0);
	}
	*is_temp = 1;
	return retval;
}/*}}}*/

static zval *php_waves_private_key_get_property_ptr_ptr(zval *object, zval *member, int type, void **cache_slot)/*{{{*/
{
	php_waves_private_key_t *intern = php_waves_private_key_object_fetch(Z_OBJ_P(object));
	return (EXPECTED(intern) ? get_property_ptr_ptr(object, member, type, cache_slot, (void *)intern, intern->prop_handler) : NULL);
}/*}}}*/

static HashTable *php_waves_private_key_get_properties(zval *object)/*{{{*/
{
	HashTable *retval;
	php_waves_private_key_t *intern = php_waves_private_key_object_fetch(Z_OBJ_P(object));
	if (EXPECTED(intern)) {
		retval = get_properties(object, (void *)intern, intern->prop_handler);
	} else {
		ALLOC_HASHTABLE(retval);
	}
	return retval;
}/*}}}*/

/* WavesPrivateKey class object handlers }}}*/

static zend_always_inline void register_classes()/*{{{*/
{
	zend_class_entry *ce;
	zend_class_entry ce_exception;

	INIT_CLASS_ENTRY(ce_exception, "WavesException", NULL);
	php_waves_exception_ce = zend_register_internal_class_ex(&ce_exception, php_waves_get_exception_base(0));
	zend_declare_property_null(php_waves_exception_ce, "errorInfo", sizeof("errorInfo") - 1, ZEND_ACC_PUBLIC);

	zend_hash_init(&classes, 4, NULL, NULL, 1);

	PHP_WAVES_REGISTER_CLASS("WavesAddress", waves_address_object_create, php_waves_address_ce, php_waves_address_ce_functions);
	ce = php_waves_address_ce;
	zend_hash_init(&waves_address_properties, 1, NULL, free_prop_handler, 1);
	PHP_WAVES_ADD_CLASS_PROPERTIES(&waves_address_properties, waves_address_property_entries);
	zend_declare_property_null(ce, "address", sizeof("address") - 1, ZEND_ACC_PUBLIC);
	zend_hash_add_ptr(&classes, ce->name, &waves_address_properties);

	PHP_WAVES_REGISTER_CLASS("WavesPublicKey", waves_public_key_object_create, php_waves_public_key_ce, php_waves_public_key_ce_functions);
	ce = php_waves_public_key_ce;
	zend_hash_init(&waves_public_key_properties, 1, NULL, free_prop_handler, 1);
	PHP_WAVES_ADD_CLASS_PROPERTIES(&waves_public_key_properties, waves_public_key_property_entries);
	zend_declare_property_null(ce, "key", sizeof("key") - 1, ZEND_ACC_PUBLIC);
	zend_hash_add_ptr(&classes, ce->name, &waves_public_key_properties);

	PHP_WAVES_REGISTER_CLASS("WavesSignature", waves_signature_object_create, php_waves_signature_ce, php_waves_signature_ce_functions);
	ce = php_waves_signature_ce;
	zend_hash_init(&waves_signature_properties, 1, NULL, free_prop_handler, 1);
	PHP_WAVES_ADD_CLASS_PROPERTIES(&waves_signature_properties, waves_signature_property_entries);
	zend_declare_property_null(ce, "signature", sizeof("signature") - 1, ZEND_ACC_PUBLIC);
	zend_hash_add_ptr(&classes, ce->name, &waves_signature_properties);

	PHP_WAVES_REGISTER_CLASS("WavesPrivateKey", waves_private_key_object_create, php_waves_private_key_ce, php_waves_private_key_ce_functions);
	ce = php_waves_private_key_ce;
	zend_hash_init(&waves_private_key_properties, 1, NULL, free_prop_handler, 1);
	PHP_WAVES_ADD_CLASS_PROPERTIES(&waves_private_key_properties, waves_private_key_property_entries);
	zend_declare_property_null(ce, "key", sizeof("key") - 1, ZEND_ACC_PUBLIC);
	zend_hash_add_ptr(&classes, ce->name, &waves_private_key_properties);
}
/*}}}*/

/* {{{ waves_module_entry */
zend_module_entry waves_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL,
	NULL, /* dependencies */
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
	memcpy(&waves_address_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	waves_address_object_handlers.offset = XtOffsetOf(php_waves_address_t, zo);
	waves_address_object_handlers.get_gc = get_gc;
	waves_address_object_handlers.clone_obj = NULL;
	/*waves_address_object_handlers.free_obj = php_waves_address_free_obj;*/
	waves_address_object_handlers.read_property = php_waves_address_read_property;
	waves_address_object_handlers.write_property = php_waves_address_write_property;
	waves_address_object_handlers.get_property_ptr_ptr = php_waves_address_get_property_ptr_ptr;
	waves_address_object_handlers.has_property = php_waves_address_has_property;
	waves_address_object_handlers.get_debug_info = php_waves_address_get_debug_info;
	waves_address_object_handlers.get_properties = php_waves_address_get_properties;

	memcpy(&waves_public_key_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	waves_public_key_object_handlers.offset = XtOffsetOf(php_waves_public_key_t, zo);
	waves_public_key_object_handlers.get_gc = get_gc;
	waves_public_key_object_handlers.clone_obj = NULL;
	/*waves_public_key_object_handlers.free_obj = php_waves_public_key_free_obj;*/
	waves_public_key_object_handlers.read_property = php_waves_public_key_read_property;
	waves_public_key_object_handlers.write_property = php_waves_public_key_write_property;
	waves_public_key_object_handlers.get_property_ptr_ptr = php_waves_public_key_get_property_ptr_ptr;
	waves_public_key_object_handlers.has_property = php_waves_public_key_has_property;
	waves_public_key_object_handlers.get_debug_info = php_waves_public_key_get_debug_info;
	waves_public_key_object_handlers.get_properties = php_waves_public_key_get_properties;

	memcpy(&waves_signature_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	waves_signature_object_handlers.offset = XtOffsetOf(php_waves_signature_t, zo);
	waves_signature_object_handlers.get_gc = get_gc;
	waves_signature_object_handlers.clone_obj = NULL;
	/*waves_signature_object_handlers.free_obj = php_waves_signature_free_obj;*/
	waves_signature_object_handlers.read_property = php_waves_signature_read_property;
	waves_signature_object_handlers.write_property = php_waves_signature_write_property;
	waves_signature_object_handlers.get_property_ptr_ptr = php_waves_signature_get_property_ptr_ptr;
	waves_signature_object_handlers.has_property = php_waves_signature_has_property;
	waves_signature_object_handlers.get_debug_info = php_waves_signature_get_debug_info;
	waves_signature_object_handlers.get_properties = php_waves_signature_get_properties;

	memcpy(&waves_private_key_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	waves_private_key_object_handlers.offset = XtOffsetOf(php_waves_private_key_t, zo);
	waves_private_key_object_handlers.get_gc = get_gc;
	waves_private_key_object_handlers.clone_obj = NULL;
	/*waves_private_key_object_handlers.free_obj = php_waves_private_key_free_obj;*/
	waves_private_key_object_handlers.read_property = php_waves_private_key_read_property;
	waves_private_key_object_handlers.write_property = php_waves_private_key_write_property;
	waves_private_key_object_handlers.get_property_ptr_ptr = php_waves_private_key_get_property_ptr_ptr;
	waves_private_key_object_handlers.has_property = php_waves_private_key_has_property;
	waves_private_key_object_handlers.get_debug_info = php_waves_private_key_get_debug_info;
	waves_private_key_object_handlers.get_properties = php_waves_private_key_get_properties;

	register_classes();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(waves)
{
	zend_hash_destroy(&waves_address_properties);
	zend_hash_destroy(&waves_public_key_properties);
	zend_hash_destroy(&waves_signature_properties);
	zend_hash_destroy(&waves_private_key_properties);
	zend_hash_destroy(&classes);

	return SUCCESS;
}
/* }}} */

/*{{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(waves)
{
#if defined(COMPILE_DL_WAVES) && defined(ZTS)
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


/*{{{ Procedural style API */

/* {{{ proto string keccak256(string data) */
PHP_FUNCTION(keccak256)
{
	char *data;
	size_t data_len;
	//uint8_t hash[32];
	char hash[64];
	sha3_context c;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s",
				&data, &data_len) == FAILURE) {
		return;
	}

    sha3_Init256(&c);
    sha3_Update(&c, data, data_len);
    sha3_Finalize(&c);

    php_hash_bin2hex(hash, c.sb, 32);

	//memcpy(hash, c.sb, 32);

	RETURN_STRINGL((const char *)hash, sizeof(hash));
}

/* {{{ proto string waves_secure_hash(string message) */
PHP_FUNCTION(waves_secure_hash)
{
	char *message;
	size_t message_len;
	uint8_t hash[WAVES_SECURE_HASH_BYTES];

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s",
				&message, &message_len) == FAILURE) {
		return;
	}
	waves_secure_hash((const uint8_t *)message, message_len, hash);
	RETURN_STRINGL((const char *)hash, sizeof(hash));
}
/*}}}*/

/* {{{ proto string waves_sign_message(string $private_key, string $message, ?string $random = null);
 *
 * $random is a 64-byte binary sequence.
 *
 * Returns signature on success, otherwise NULL.
 *
 * Throws InvalidArgumentException
 * Throws WavesException
 */
PHP_FUNCTION(waves_sign_message)
{
	char *message;
	char *priv_key;
	char *random = NULL;
	size_t message_len;
	size_t priv_key_len;
	size_t random_len = 0;
	curve25519_signature signature;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|s!",
				&priv_key, &priv_key_len,
				&message, &message_len,
				&random, &random_len) == FAILURE) {
		return;
	}

	CHECK_PRIVATE_KEY_LEN(priv_key_len);

	if (random != NULL && random_len != 0) {
		if (random_len != WAVES_RANDOM_SEED_BYTES) {
			zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0,
					"Random sequence expected to be %ld bytes in length, got %ld bytes",
					(long)WAVES_RANDOM_SEED_BYTES, (long)random_len);
			return;
		}

		if (waves_sign_message_custom_random((const unsigned char *)priv_key,
					(const unsigned char *)message,
					message_len, signature,
					(unsigned char *)random)) {
			RETURN_STRINGL((const char *)signature, sizeof(curve25519_signature));
		}
		zend_throw_exception_ex(php_waves_get_exception(), 0,
				"Failed to sign message '%s'", message);
		return;
	}

	if (waves_sign_message((const unsigned char *)priv_key,
				(const unsigned char *)message,
				message_len, signature)) {
		RETURN_STRINGL((const char *)signature, sizeof(curve25519_signature));
	}
	zend_throw_exception_ex(php_waves_get_exception(), 0,
			"Failed to sign message '%s'", message);
}
/*}}}*/

/* {{{ proto string waves_base58_encode(string $in)
 *
 * Throws WavesException
 */
PHP_FUNCTION(waves_base58_encode)
{
	char *in;
	size_t in_len;
	char *out;
	size_t out_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s",
				&in, &in_len) == FAILURE) {
		return;
	}

	out_len = b58_length_from_bytes(in_len);
	out = emalloc(sizeof(char) * out_len);
	if (!out) {
		zend_throw_exception_ex(php_waves_get_exception(), 0,
				"Failed to allocate %ld bytes",
				(long)(sizeof(char) * out_len));
		return;
	}

	out_len = base58_encode(out, (const unsigned char *)in, in_len);
	/* out_len includes the terminating zero byte, so we decrement it */
	RETVAL_STRINGL(out, --out_len);
}
/*}}}*/

/* {{{ proto string waves_base58_decode(string $in) */
PHP_FUNCTION(waves_base58_decode)
{
	char *in;
	size_t in_len;
	char *out;
	ssize_t out_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s",
				&in, &in_len) == FAILURE) {
		return;
	}

	if (in_len == 0) {
		RETURN_EMPTY_STRING();
	}

	out_len = bytes_length_from_b58(in_len);
	out = emalloc(sizeof(char) * out_len);
	if (!out) {
		zend_throw_exception_ex(php_waves_get_exception(), 0,
				"Failed to allocate %ld bytes",
				(long)(sizeof(char) * out_len));
		return;
	}

	out_len = base58_decode((unsigned char *)out, in);
	if (out_len <= 0) {
		zend_throw_exception_ex(php_waves_get_exception(), 0, "Invalid input");
		return;
	}

	PHP_WAVES_ASSERT(out_len);
	RETVAL_STRINGL(out, out_len);
}
/*}}}*/

/* {{{ proto bool waves_verify_message(string $public_key, string $message, string $signature) */
PHP_FUNCTION(waves_verify_message)
{
	char *public_key;
	char *message;
	char *signature;
	size_t public_key_len;
	size_t message_len;
	size_t signature_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sss",
				&public_key, &public_key_len,
				&message, &message_len,
				&signature, &signature_len) == FAILURE) {
		return;
	}

	if (waves_verify_message((const unsigned char *)public_key,
			(const unsigned char *)message,
			message_len, (const unsigned char *)signature)) {
		RETURN_TRUE;
	}
	RETVAL_FALSE;
}
/*}}}*/

/* {{{ proto string waves_public_key_to_address(string $public_key, string $network_byte)
 * Throws WavesException
   Throws InvalidArgumentException */
PHP_FUNCTION(waves_public_key_to_address)
{
	char *public_key;
	char *network_byte;
	char *out;
	size_t public_key_len;
	size_t network_byte_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss",
				&public_key, &public_key_len,
				&network_byte, &network_byte_len) == FAILURE) {
		return;
	}

	CHECK_PUBLIC_KEY_LEN(public_key_len);
	CHECK_NETWORK_BYTE_LEN(network_byte_len);
	WAVES_EMALLOC(out, WAVES_ADDRESS_BYTES);

	waves_public_key_to_address((const unsigned char *)public_key,
			(unsigned char)network_byte[0], (unsigned char *)out);
	RETURN_STRINGL(out, WAVES_ADDRESS_BYTES);
}
/*}}}*/

/* {{{ proto string waves_generate_public_key(string $private_key) */
PHP_FUNCTION(waves_generate_public_key)
{
	char *public_key;
	char *private_key;
	size_t private_key_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s",
				&private_key, &private_key_len) == FAILURE) {
		return;
	}

    CHECK_PRIVATE_KEY_LEN(private_key_len);

	php_waves_sc_clamp((unsigned char *)private_key);

	public_key = emalloc(WAVES_PUBLIC_KEY_BYTES);
	if (!public_key) {
		zend_throw_exception_ex(php_waves_get_exception(), 0,
				"Failed to allocate %ld bytes", (long)WAVES_PUBLIC_KEY_BYTES);
		return;
	}

	waves_gen_public_key((unsigned char *) public_key,
			(unsigned char *) private_key);
	RETURN_STRINGL(public_key, WAVES_PUBLIC_KEY_BYTES);
}
/*}}}*/

/* {{{ proto string waves_generate_private_key(string $seed) */
PHP_FUNCTION(waves_generate_private_key)
{
	char *seed;
	char *private_key;
	size_t seed_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s",
				&seed, &seed_len) == FAILURE) {
		return;
	}

	private_key = emalloc(WAVES_PRIVATE_KEY_BYTES);
	if (!private_key) {
		zend_throw_exception_ex(php_waves_get_exception(), 0,
				"Failed to allocate %ld bytes", (long)WAVES_PRIVATE_KEY_BYTES);
		return;
	}

	waves_gen_private_key((unsigned char *)private_key, (const unsigned char *)seed);
	RETURN_STRINGL(private_key, WAVES_PRIVATE_KEY_BYTES);
}
/*}}}*/

/* Procedural style API }}}*/

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 sts=4 fdm=marker
 * vim<600: noet sw=4 ts=4 sts=4
 */
