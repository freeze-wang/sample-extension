<?php

/*
*https://github.com/ThomasWeinert/php-extension-sample
*扩展中实现用例
*内容包括 扩展案例的C代码;运行扩展的php代码
*php -d extension=/usr/local/lib/php/extensions/debug-non-zts-20180731/sample.so . /test.php
*/
 
//echo 111;
echo "\n";
//function global
echo "\nfunction return global \n";
sample_get_global_value();
sample\getvalue();

//function return
echo "\nfunction return value \n";
var_dump(sample_answer([1]));
echo "\nfunction return class \n";
//function return class
var_dump(sample\sample_return_class()->greeting);

//function return object
$a_obj=sample\sample_getGreeting();
//$a_obj->__construct("2222\n");
echo "\nfunction_return_object \n";
$a_obj->hello();
//phpinfo(); 

//function_return_array_numeric
echo "\nfunction_return_array_numeric \n";
var_dump(sample\sample_return_array_numeric(['22','3','aaa']));

//function_return_array_assoc
echo "\nfunction_return_array_assoc \n";
var_dump(sample\sample_return_array_assoc()); 

//function_variables for the array
echo "\nfunction_argument_variadics 动态可变参数数量 \n";
sample\sample_argument_variadics(11,22,33);

//function_variables for the array
echo "\nfunction_argument_optional 动态可选参数,第二个参数可选 \n";
echo sample\sample_argument_optional(11,2);


$fp = fopen("log.txt", "rw"); 
//function_variables for the argument_mixed
echo "\nfunction_argument_mixed 动态根据输入类型打印数据 \n";
echo sample\sample_argument_mixed(1);
echo "\n";
echo sample\sample_argument_mixed($fp);
echo "\n";
fclose($fp); 


//function_argument_callback 传入参数 回调函数
echo "\nfunction_argument_callback 传入参数 回调函数 \n";
sample\sample_argument_callback(function($a,$b){
	//echo $callback;
	echo $a.$b."\n";
	echo "first_callback\n";
	return "callback return";
},111,2);

//function_argument_array
echo "\nfunction_argument_callback 传入数组 遍历打印 \n";
echo sample\sample_argument_array([11,1,2,3,3,444]);


//resource
//$aa = sample_create_resource();
//sample_use_resource($aa);

//sample_interface_test1();
//echo sample_interface_test2(123333333333);

//interface
 class sample implements SampleInterface
{
	public $name = "hello world!Interface";
	
	public function sampleMethod()
	{
		echo $this->name."\n";
	}
	public function sampleDemo()
	{
		echo $this->name."\n";
	}
	public function jsonSerialize()
	{
		return "jsonSerialize\n";
	}
	public function do($thing)
	{
		echo  $thing."\n";
	}	
}
$obj = new sample();
echo "\nInterface SampleInterface \n";
$obj->sampleMethod(); 
//$obj->jsonSerialize();
//实体类转json会自动执行jsonSerialize()方法；
echo json_encode($obj);

//sample_arginfo_object
echo "\nsample_arginfo_object 传入object \n";
echo sample\sample_arginfo_object($obj);

//sample_arginfo_object_namespace
echo "\nsample_arginfo_object_namespace 传入object \n";
echo sample\sample_arginfo_object_namespace($obj);

//sample_trigger_exception
echo "\nsample_trigger_exception \n";
//sample\sample_trigger_exception();

//sample_init_customer exception
echo "\nsample_trigger_customer_exception \n";
//sample\sample_trigger_customer_exception();

echo "\nsample constant 常量 \n";
echo sample\SAMPLE_INT."常量 \n";

//class_property_public 定义类及其成员
$b_obj=new sample\Greeting("Freeze.\n",2);
echo "\nclass_property_public 定义类及其成员\n";
$b_obj->hello();
echo $b_obj->age."\n";

echo "\nclass_constant 定义类的实体常量\n";
echo sample\Greeting::CONSTANT_ANSWER."\n";

/*
https://blog.csdn.net/u013474436/article/details/79028116
https://github.com/owenliang/php7-extension-explore/tree/master/course7-how-to-create-object
 * 调用lib创建对象和普通创建对象不同之处：
 * 需要一个结构体来保存对象与lib库资源之间的关系。如上面的helo_object。
 * 需要定义一些方法。如创建对象，消耗对象，对象转换的方法。
 * 需要在PHP_MINIT_FUNCTION中把定义的方法和类绑定起来。
*/
echo "\nclass_attach_struct 定义类关联结构体\n";
echo $b_obj->get();

sample\sample_call_method($obj);
sample\sample_call_method($obj,"do");
sample\sample_call_method($obj,"doaa");
?>
