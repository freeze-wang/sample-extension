/* sample extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_sample.h"
#include "Zend/zend_interfaces.h"
#include "Zend/zend_types.h"
#include <zend_exceptions.h>
#include "ext/spl/spl_exceptions.h"
#include "ext/json/php_json.h"

#define PHP_SAMPLE_CLASS_GREETING_NAME "Greeting"
#define SAMPLE_INT 42

static zend_class_entry *php_sample_interface_entry;
static zend_class_entry *php_sample_greeting_class_entry;
zend_class_entry *sample_exception;


/*
 *https://blog.csdn.net/u013474436/article/details/79028116
 *目的是通过对象调用外部 结构体的方法，成员变量等，这个方式主要用于规范
 *
 * 调用lib创建对象和普通创建对象不同之处：
 * 需要一个结构体来保存对象与lib库资源之间的关系。如上面的helo_object。
 * 需要定义一些方法。如创建对象，消耗对象，对象转换的方法。
 * 需要在PHP_MINIT_FUNCTION中把定义的方法和类绑定起来。
 *
 1. Define a variable for the object handlers
 声明一个变量，用于存储类的处理函数。增加如下代码：
 */
zend_object_handlers php_sample_greeting_handlers;

/*
 2. Define a struct that contains the original zend object and the additional data
  定义一个结构体类型，用于存储类的一些信息
*/
typedef struct _php_sample_greeting_t {
	zval who;
	zend_object std;
} php_sample_greeting_t;

/*
 3. Define templates to fetch the struct from a zend object or zval
 定义一个函数，用于从标准的zend_object转换获取php_sample_greeting_t地址
 */
#define php_sample_greeting_from(o) ((php_sample_greeting_t*) \
		((char*) o - XtOffsetOf(php_sample_greeting_t, std)))
#define php_sample_greeting_fetch(z) php_sample_greeting_from(Z_OBJ_P(z))

/*
 4. A handler function to create the object
 定义一个函数，用户创建类对象
*/
zend_object* php_sample_greeting_create(zend_class_entry *ce) {
	php_sample_greeting_t *s = (php_sample_greeting_t*) emalloc(sizeof(php_sample_greeting_t) + zend_object_properties_size(ce));
	zend_object_std_init(&s->std, ce);
	object_properties_init(&s->std, ce);
	s->std.handlers = &php_sample_greeting_handlers;
	return &s->std;
}

/*
 5. A handler function to free the object
 定义函数，用户销毁类对象
*/
void php_sample_greeting_free(zend_object *o) {
	php_sample_greeting_t *s = php_sample_greeting_from(o);
	zval_dtor(&s->who);
	zend_object_std_dtor(o);
}

/*
 3. Declare global module variables
 */

ZEND_DECLARE_MODULE_GLOBALS(sample)


//参数声明不是强制性的,但高度推荐.反射API(reflection API)使用参数声明来获取有关该功能的信息.
/*
 * ZEND_BEGIN_ARG_INFO_EX
name: 参数数组名，注册函数PHP_FE(function, arg_info)会用到
_unused: 保留值，暂时无用
return_reference: 返回值是否为引用，一般很少会用到
required_num_args: required参数数
*/
/* sample_answer参数  */
ZEND_BEGIN_ARG_INFO_EX(arginfo_sample_answer, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, arr, IS_ARRAY, 0)
ZEND_END_ARG_INFO();
/*sample_arrayNumeric */
ZEND_BEGIN_ARG_INFO_EX(arginfo_sample_arrayNumeric, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, arr, IS_ARRAY, 0)
ZEND_END_ARG_INFO();
/*
 * sample_argument_optional
 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_sample_argument_optional, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, first, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, second, IS_LONG, 0)
ZEND_END_ARG_INFO();
/*
 * sample_argument_object
 */
ZEND_BEGIN_ARG_INFO(ArgInfo_sample_object, 0)
		ZEND_ARG_OBJ_INFO(0, sample_arginfo_object, sample, 0)
ZEND_END_ARG_INFO()
/*
 * sample_argument_object namespace
 */
ZEND_BEGIN_ARG_INFO(ArgInfo_sample_object_namespace, 0)
		ZEND_ARG_OBJ_INFO(0, sample_arginfo_object_namespace, sample, 0)
ZEND_END_ARG_INFO()

PHP_FUNCTION(sample_get_global_value)
{

    long value;
    value = SAMPLE_G(sample_value);
    if(value)
	php_printf("Current value: %ld\n", value);
	SAMPLE_G(sample_value) = (value == 42) ? 21 : 42;
}

PHP_FUNCTION(sample_answer)
{
	/*
	  . Use a type specific template to return a value
		方法的返回值是使用RETURN_开头的宏方法进行返回的。常用的宏方法有：
		RETURN_NULL() 返回null
		RETURN_LONG(l) 返回整型
		RETURN_DOUBLE(d) 返回浮点型
		RETURN_STR(s) 返回一个字符串。参数是一个zend_string * 指针
		RETURN_STRING(s) 返回一个字符串。参数是一个char * 指针
		RETURN_STRINGL(s, l) 返回一个字符串。第二个参数是字符串长度。
		RETURN_EMPTY_STRING() 返回一个空字符串。
		RETURN_ARR(r) 返回一个数组。参数是zend_array *指针。
		RETURN_OBJ(r) 返回一个对象。参数是zend_object *指针。
		RETURN_ZVAL(zv, copy, dtor) 返回任意类型。参数是 zval *指针。
		RETURN_FALSE 返回false
		RETURN_TRUE 返回true
	 */
	//RETURN_LONG(50);
	HashTable *arr;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &arr) == FAILURE) {
		return;
	}
    if (zend_hash_num_elements(arr) == 0) {
        //return;
    }
	RETURN_ARR(arr);
}

PHP_FUNCTION(sample_return_class)
{
	/*
	  *. initialize return_value variable as an stdClass using
		 the zend class entry
	 */
	object_init_ex(return_value, zend_standard_class_def);

	/*
	  *. update the property
	 */
	zend_update_property_stringl(
		zend_standard_class_def,
		return_value,
		ZEND_STRL("greeting"),
		ZEND_STRL("Hello World!")
	);
}
PHP_FUNCTION(sample_arrayNumeric)
{
	HashTable *names;
	zval *entry;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(names)
	ZEND_PARSE_PARAMETERS_END();

	/*
	 *. initialize the return value as an array
	*/
	array_init(return_value);

	ZEND_HASH_FOREACH_VAL(names, entry) {
		zend_string *name = zval_get_string(entry);
		zend_string *greeting = strpprintf(0, "Hello %s!", ZSTR_VAL(name));
		zend_string_release(name);

		/*
		 *add greeting to list
		 */
		add_next_index_str(return_value, greeting);
	} ZEND_HASH_FOREACH_END();
}
//function_return_array_assoc
PHP_FUNCTION(sample_returnArrayAssoc)
{
	/*
	 1. initialize return_value as array
	*/
	array_init(return_value);

	/*
	 2. add elements with named key
	*/
	add_assoc_str(return_value, "greeting", zend_string_init("Hello %s!", strlen("Hello %s!"), 0));
	add_assoc_str(return_value, "who", zend_string_init("World", strlen("World"), 0));
}
//return variables for the array 可变的数组
PHP_FUNCTION(sample_argument_variadics)
{
	/*
	1. define variables for the array
	*/
	zval *args;
	int argc, i;

	/*
	2. parse parameters using FAST ZPP Api
		https://wiki.php.net/rfc/fast_zpp
	*/
	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	/*
	3. Iterate the arguments
	*/
	for (i = 0; i < argc; i++) {
		zval *arg = args + i;

		convert_to_string(arg);
		php_printf("Hello %s!\n", Z_STRVAL_P(arg));
	}
}
//动态可选参数,第二个参数可选
PHP_FUNCTION(sample_argument_optional)
{
	/*
	 1. define/initialize variables
	 */
	zend_long first;
	zend_long second = 1;

	/*
	 2. parse parameters using FAST ZPP Api
	 https://wiki.php.net/rfc/fast_zpp
	 */
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_LONG(first)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(second)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(first * second);
}
//传入未知的复杂类型参数
PHP_FUNCTION(sample_argument_mixed)
{
	zval *value;

	/*
	 1. fetch the parameter as ZVAL (pointer)
	 */
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	/*
	 2. Z_TYPE_P allows you to get the type
	 */
	switch (Z_TYPE_P(value)) {
	/*
	 3. compare it with the IS_* macros
	 */
	case IS_NULL :
		php_printf("NULL");
		break;
	case IS_TRUE :
		php_printf("Boolean: TRUE");
		break;
	case IS_FALSE :
		php_printf("Boolean: FALSE");
		break;
	case IS_LONG :
		convert_to_string(value);
		php_printf("Integer: %s", Z_STRVAL_P(value));
		break;
	case IS_DOUBLE :
		/*
		 4b. Z_*VAL_P() allows you to read the value from the zval
		 */
		php_printf("Float: %f", Z_DVAL_P(value));
		break;
	case IS_STRING :
		php_printf("String: %s", Z_STRVAL_P(value));
		break;
	case IS_RESOURCE :
		php_printf("Resource: #%p", Z_RES_VAL_P(value));
		break;
	case IS_ARRAY :
		php_printf("Array");
		break;
	case IS_OBJECT :
		php_printf("Object");
		break;
	default :
		php_printf("UNKNOWN");
		break;
	}
}
//传入参数 回调函数
PHP_FUNCTION(sample_argument_callback)
{
	/*
	 1. define variables for function call info and cache
	 */
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;

	zval retval;

	ZEND_PARSE_PARAMETERS_START(1, -1)
	/*
	 2. read the callback parameters
	 */
	Z_PARAM_FUNC(fci, fci_cache)
	/*
	 2a. pass additional parameters to the callback
	 */
	Z_PARAM_VARIADIC('*', fci.params, fci.param_count)
	ZEND_PARSE_PARAMETERS_END();

	fci.retval = &retval;
	/*
	 3. Execute the callback
	 */
	if (zend_call_function(&fci, &fci_cache) == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
		if (Z_ISREF(retval)) {
			zend_unwrap_reference(&retval);
		}

		convert_to_string(&retval);
		//convert_to_string(&fci.params);
		php_printf("Hello %s!\n", Z_STRVAL(retval));
	}
}
//传入数组 遍历打印
PHP_FUNCTION(sample_argument_array)
{
	/*
	 1. define variables for the array
	*/
	HashTable *names;
	zval *entry;

	/*
	 2. parse parameters using FAST ZPP Api
	 https://wiki.php.net/rfc/fast_zpp
	 */
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(names)
	ZEND_PARSE_PARAMETERS_END();

	/*
	3. use ZEND_HASH_FOREACH_* template to iterate the array
	 */
	ZEND_HASH_FOREACH_VAL(names, entry) {
		zend_string *name = zval_get_string(entry);
		//php_printf("Hello %s!\n", ZSTR_VAL(name));
		php_printf("Hello ");
		PHPWRITE(ZSTR_VAL(name), ZSTR_LEN(name));
		php_printf("!\n");
		zend_string_release(name);
	} ZEND_HASH_FOREACH_END();
}
/*
 * sample_arginfo_object 传入object参数
 */
PHP_FUNCTION(sample_arginfo_object)
{
	zval *e;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		/*
		 2. Fetch the argument and provide the zend class entry for validation
		 */
		Z_PARAM_OBJECT_OF_CLASS(e, php_sample_interface_entry)
	ZEND_PARSE_PARAMETERS_END();

	php_printf("Argument is an sample Object.\n");
}
PHP_FUNCTION(sample_getGreeting)
{
	zval name;
	zval age;
	/*
	  initialize return_value variable as an object using
		 the zend class entry
	 */
	object_init_ex(return_value, php_sample_greeting_class_entry);

	ZVAL_STRINGL(&name, "Universe", strlen("Universe"));
	ZVAL_LONG(&age, 1);
	/*
	  call constructor on that object, providing the argument
	*/
    zend_call_method_with_2_params(
		return_value,
		php_sample_greeting_class_entry,
		&php_sample_greeting_class_entry->constructor,
		"__construct",
		NULL,
		&name,
		&age
	);

	zval_ptr_dtor(&name);
}
/*
 * function_arginfo_object_namespace 带命名空间的object参数
 */
PHP_FUNCTION(sample_arginfo_object_namespace)
{
	zval *e;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		/*
			3. the ZPP API expects the zend class entry variable
		*/
		Z_PARAM_OBJECT_OF_CLASS(e, php_sample_interface_entry)
	ZEND_PARSE_PARAMETERS_END();

	php_printf("Argument is an %s\\%s.\n", PHP_SAMPLE_EXT_NS, ZSTR_VAL(php_sample_interface_entry->name));
}
//sample_trigger_exception
PHP_FUNCTION(sample_trigger_exception)
{
	zend_throw_exception_ex(
		spl_ce_LogicException, 42, "trigger!!! Sample Exception Message"
	);
}
/*
 * sample_init_exception
 */
void sample_init_exception(TSRMLS_D) {
	zend_class_entry e;

	INIT_NS_CLASS_ENTRY(e, PHP_SAMPLE_EXT_NS, "ExceptionName", NULL);
	sample_exception = zend_register_internal_class_ex(&e, (zend_class_entry*)zend_exception_get_default(TSRMLS_C));
}

PHP_FUNCTION(sample_trigger_customer_exception)
{
	zend_throw_exception_ex(
		sample_exception, 42, "trigger!!! Customer Sample Exception Message"
	);
}
PHP_FUNCTION(sample_call_method)
{
	zval *pfname,fname,params[1], *greeting;
	ZVAL_STRING(&fname,"do");
	pfname = &fname;	//指针指向地址 ， *pname获得地址内容
	ZVAL_STRING(&params[0], "World");

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJECT(greeting)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(pfname)
	ZEND_PARSE_PARAMETERS_END();

	if (
		call_user_function(
			NULL, greeting, pfname, return_value, 1, params TSRMLS_CC
		) == FAILURE
	) {
		php_error_docref(
			NULL TSRMLS_CC,
			E_ERROR,
			"Unable to call method do() on object argument"
		);
		RETVAL_FALSE;
	}

	zval_dtor(&fname);
	zval_dtor(&params[0]);
}
/*
 4. Add a request initialization function, you might need a
 request shutdown function for more complex variables.
 */
PHP_RINIT_FUNCTION(sample_global)
{
	SAMPLE_G(sample_value) = 21;
}

const zend_function_entry php_sample_functions[] = {
	ZEND_NS_NAMED_FE(PHP_SAMPLE_EXT_NS, getValue, ZEND_FN(sample_get_global_value), NULL)	//NS表示带命名空间
	PHP_FE(sample_get_global_value,NULL)
	PHP_FE(sample_answer,arginfo_sample_answer)
	ZEND_NS_NAMED_FE(PHP_SAMPLE_EXT_NS, sample_return_class, ZEND_FN(sample_return_class), NULL)
	ZEND_NS_NAMED_FE(PHP_SAMPLE_EXT_NS, sample_getGreeting, ZEND_FN(sample_getGreeting), NULL)
	ZEND_NS_NAMED_FE(PHP_SAMPLE_EXT_NS, sample_return_array_numeric, ZEND_FN(sample_arrayNumeric), arginfo_sample_arrayNumeric)
	ZEND_NS_NAMED_FE(PHP_SAMPLE_EXT_NS, sample_return_array_assoc, ZEND_FN(sample_returnArrayAssoc), NULL)
	ZEND_NS_NAMED_FE(PHP_SAMPLE_EXT_NS, sample_argument_variadics, ZEND_FN(sample_argument_variadics), NULL)
	ZEND_NS_NAMED_FE(PHP_SAMPLE_EXT_NS, sample_argument_optional, ZEND_FN(sample_argument_optional), arginfo_sample_argument_optional)
	ZEND_NS_NAMED_FE(PHP_SAMPLE_EXT_NS, sample_argument_mixed, ZEND_FN(sample_argument_mixed), NULL)
	ZEND_NS_NAMED_FE(PHP_SAMPLE_EXT_NS, sample_argument_callback, ZEND_FN(sample_argument_callback), NULL)
	ZEND_NS_NAMED_FE(PHP_SAMPLE_EXT_NS, sample_argument_array, ZEND_FN(sample_argument_array), NULL)
	ZEND_NS_NAMED_FE(PHP_SAMPLE_EXT_NS, sample_arginfo_object, ZEND_FN(sample_arginfo_object), ArgInfo_sample_object)
	ZEND_NS_NAMED_FE(PHP_SAMPLE_EXT_NS, sample_arginfo_object_namespace, ZEND_FN(sample_arginfo_object_namespace), ArgInfo_sample_object_namespace)
	ZEND_NS_NAMED_FE(PHP_SAMPLE_EXT_NS, sample_trigger_exception, ZEND_FN(sample_trigger_exception), NULL)
	ZEND_NS_NAMED_FE(PHP_SAMPLE_EXT_NS, sample_trigger_customer_exception, ZEND_FN(sample_trigger_customer_exception), NULL)
	ZEND_NS_NAMED_FE(PHP_SAMPLE_EXT_NS, sample_call_method, ZEND_FN(sample_call_method), NULL)
	PHP_FE_END
};
const zend_function_entry php_sample_interface_functions[] = {
	PHP_ABSTRACT_ME(sample_Interface, sampleMethod, NULL)
	PHP_ABSTRACT_ME(sample_Interface, sampleDemo, NULL)
	PHP_ABSTRACT_ME(sample_Interface, jsonSerialize, NULL)
	PHP_FE_END
};
//注册类的方法
PHP_METHOD(sample_Greeting, __construct) {
	zval *object;
	char *name;
	zend_long age;
	size_t name_len;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(name, name_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(age)
	ZEND_PARSE_PARAMETERS_END();

	object = getThis();
	zend_update_property_stringl(
		php_sample_greeting_class_entry,
		object,
		ZEND_STRL("name"),	//变量名
		name,
		name_len
	);
	zend_update_property_long(
		php_sample_greeting_class_entry,
		object,
		ZEND_STRL("age"),	//变量名
		age
	);
	/*
	 8. Store constructor argument in the struct
	 */
	php_sample_greeting_t *sample = php_sample_greeting_fetch(getThis());
	ZVAL_STRINGL(&sample->who, name, name_len);
	//zval_ptr_dtor(object);
}
//注册类的方法，获取成员变量
PHP_METHOD(sample_Greeting, hello) {
	zval rv, *name, *age;

	name = zend_read_property(php_sample_greeting_class_entry, getThis(), ZEND_STRL("name"), 0, &rv);
	age = zend_read_property(php_sample_greeting_class_entry, getThis(), ZEND_STRL("age"), 0, &rv);

	php_printf("Hello %s your age is %ld ", Z_STRVAL_P(name), Z_LVAL_P(age));
}
//通过对象调用外部 结构体的方法，成员变量等
PHP_METHOD(sample_Greeting, get)
{
    php_sample_greeting_t *sample = php_sample_greeting_fetch(getThis());
    RETURN_STRING(Z_STRVAL(sample->who));
}
//注册接口的方法
PHP_METHOD(sample_Interface, jsonSerialize) {

	array_init(return_value);
	//add_assoc_str关联数组采用char*作为key，zval*作为value，可以使用如下宏将已有的zval return_value加入数组或者更新已有元素：
	add_assoc_str(return_value, "greeting", zend_string_init(ZEND_STRL("HELLO jsonSerialize"), 0));
	add_assoc_long(return_value, "answer", 42);
}

const zend_function_entry php_sample_greeting_class_functions[] = {
	PHP_ME(sample_Greeting, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(sample_Greeting, hello, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(sample_Greeting, get, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

PHP_MINIT_FUNCTION(sample_minit)
{
	/*
	 4. Initialize and register the interface
	 */
	zend_class_entry ce;
	INIT_CLASS_ENTRY(
		ce, PHP_SAMPLE_INTERFACE_NAME, php_sample_interface_functions
	);
	//注册内部类
	php_sample_interface_entry = zend_register_internal_interface(&ce TSRMLS_CC);
	//php_sample_interface_entry 注册jsonSerialize函数
	zend_class_implements(php_sample_interface_entry, 1, php_json_serializable_ce);

	zend_class_entry ceg;
	//初始化类实体
	INIT_NS_CLASS_ENTRY(
			ceg, PHP_SAMPLE_EXT_NS, PHP_SAMPLE_CLASS_GREETING_NAME, php_sample_greeting_class_functions
	);

	//注册、声明一般类实体
	php_sample_greeting_class_entry = zend_register_internal_class(&ceg TSRMLS_CC);
	zend_declare_property_stringl(
		php_sample_greeting_class_entry, ZEND_STRL("name"), ZEND_STRL("World"), ZEND_ACC_PUBLIC
	);
	//自定义异常类型 In the module initialization, call the exception class init
	sample_init_exception(TSRMLS_C);


	//register the constant in the namespace 注册常量
	REGISTER_NS_LONG_CONSTANT(
		PHP_SAMPLE_EXT_NS, "SAMPLE_INT", SAMPLE_INT, CONST_CS | CONST_PERSISTENT
	);
	/*
	 1. Declare class constant 定义类的成员常量
	 */
	zend_declare_class_constant_long(php_sample_greeting_class_entry, ZEND_STRL("CONSTANT_ANSWER"), 42);
	/*
	 6. Set the create handler
	 */
	php_sample_greeting_class_entry->create_object = php_sample_greeting_create;

	/*
	 7. Copy the default handlers and add the free handler
	 */
	memcpy(&php_sample_greeting_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	php_sample_greeting_handlers.offset = XtOffsetOf(php_sample_greeting_t, std);
	php_sample_greeting_handlers.free_obj = php_sample_greeting_free;

}


zend_module_entry sample_module_entry = {
	STANDARD_MODULE_HEADER,
	PHP_SAMPLE_EXT_NAME,
	php_sample_functions, /* Functions */
	PHP_MINIT(sample_minit), /* MINIT */
	NULL, /* MSHUTDOWN */
	/*
	 5. add request initialization function to the module entry
	 */
	PHP_RINIT(sample_global), /* RINIT */
	NULL, /* RSHUTDOWN */
	NULL, /* MINFO */
	PHP_SAMPLE_EXT_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_SAMPLE
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(sample)
#endif

