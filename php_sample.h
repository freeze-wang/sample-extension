/* sample extension for PHP */

#ifndef PHP_SAMPLE_H
# define PHP_SAMPLE_H

extern zend_module_entry sample_module_entry;
# define phpext_sample_ptr &sample_module_entry

extern zend_module_entry sample_interface_module_entry;
# define phpext_sample_interface_ptr &sample_interface_module_entry

#define PHP_SAMPLE_INTERFACE_NAME "SampleInterface"
# define PHP_SAMPLE_INTERFACE_VERSION "0.1.0"

#define PHP_SAMPLE_EXT_NAME  "sample"
# define PHP_SAMPLE_EXT_VERSION "0.1.0"

#define PHP_SAMPLE_EXT_NS "sample"

/*
 Define/register global module variables
 */
ZEND_BEGIN_MODULE_GLOBALS(sample)
	long sample_value;
ZEND_END_MODULE_GLOBALS(sample)

/*
 Define a template to access the global module variables, suggested
 name is {EXT_NAME}_G
 */
#define SAMPLE_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(sample, v)


#ifdef ZTS
#include "TSRM.h"
#endif
# if defined(ZTS) && defined(COMPILE_DL_SAMPLE)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_SAMPLE_H */

