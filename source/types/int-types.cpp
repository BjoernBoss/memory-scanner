#include "int_types.h"

//implementation of the acquire-function - int8
datatype typeint8::acquire()
{
	//create the new object
	datatype type = datatype();

	//set the attributes used to describe the datatype
	type.size = sizeof(int8_t);
	type.addr_alignment = sizeof(int8_t);
	type.name = "int8";
	type.restricted = false;
	type.tostring = typeint8::tostring;

	//set the function to read input
	type.readinput = typeint8::readinput;

	//set the test-functions
	type.validate = typeint8::validate;
	type.test_equal = typeint8::test_equal;
	type.test_unequal = typeint8::test_unequal;
	type.test_less = typeint8::test_less;
	type.test_less_equal = typeint8::test_less_equal;
	type.test_greater_equal = typeint8::test_greater_equal;
	type.test_greater = typeint8::test_greater;

	//return the type
	return type;
}

//implementation of the functions used to describe the datatype - int8
std::string typeint8::tostring(uint8_t* value)
{
	//convert the value to a string
	std::stringstream sstr;
	sstr << (int16_t)*(int8_t*)value;
	return sstr.str();
}

//implementation of the function to read input - int8
bool typeint8::readinput(uint8_t* buffer)
{
	//read the input
	std::cout << "enter a value: ";
	std::string str;
	std::getline(std::cin, str);
	if (str.size() == 0)
		return false;

	//convert the input to a number
	std::stringstream sstr(str);
	int64_t value;
	sstr >> value;
	if (!sstr.eof())
		return false;

	//set the value
	*(int8_t*)buffer = (int8_t)value;
	return true;
}

//implementation of the test-functions - int8
bool typeint8::validate(uint8_t* value)
{
	return true;
}
bool typeint8::test_equal(uint8_t* value, uint8_t* compareto)
{
	if (*(int8_t*)compareto == *(int8_t*)value)
		return true;
	return false;
}
bool typeint8::test_unequal(uint8_t* value, uint8_t* compareto)
{
	if (*(int8_t*)compareto != *(int8_t*)value)
		return true;
	return false;
}
bool typeint8::test_less(uint8_t* value, uint8_t* compareto)
{
	if (*(int8_t*)value < *(int8_t*)compareto)
		return true;
	return false;
}
bool typeint8::test_less_equal(uint8_t* value, uint8_t* compareto)
{
	if (*(int8_t*)value <= *(int8_t*)compareto)
		return true;
	return false;
}
bool typeint8::test_greater_equal(uint8_t* value, uint8_t* compareto)
{
	if (*(int8_t*)value >= *(int8_t*)compareto)
		return true;
	return false;
}
bool typeint8::test_greater(uint8_t* value, uint8_t* compareto)
{
	if (*(int8_t*)value > *(int8_t*)compareto)
		return true;
	return false;
}



//implementation of the acquire-function - int16
datatype typeint16::acquire()
{
	//create the new object
	datatype type = datatype();

	//set the attributes used to describe the datatype
	type.size = sizeof(int16_t);
	type.addr_alignment = sizeof(int16_t);
	type.name = "int16";
	type.restricted = false;
	type.tostring = typeint16::tostring;

	//set the function to read input
	type.readinput = typeint16::readinput;

	//set the test-functions
	type.validate = typeint16::validate;
	type.test_equal = typeint16::test_equal;
	type.test_unequal = typeint16::test_unequal;
	type.test_less = typeint16::test_less;
	type.test_less_equal = typeint16::test_less_equal;
	type.test_greater_equal = typeint16::test_greater_equal;
	type.test_greater = typeint16::test_greater;

	//return the type
	return type;
}

//implementation of the functions used to describe the datatype - int16
std::string typeint16::tostring(uint8_t* value)
{
	//convert the value to a string
	std::stringstream sstr;
	sstr << *(int16_t*)value;
	return sstr.str();
}

//implementation of the function to read input - int16
bool typeint16::readinput(uint8_t* buffer)
{
	//read the input
	std::cout << "enter a value: ";
	std::string str;
	std::getline(std::cin, str);
	if (str.size() == 0)
		return false;

	//convert the input to a number
	std::stringstream sstr(str);
	int64_t value;
	sstr >> value;
	if (!sstr.eof())
		return false;

	//set the value
	*(int16_t*)buffer = (int16_t)value;
	return true;
}

//implementation of the test-functions - int16
bool typeint16::validate(uint8_t* value)
{
	return true;
}
bool typeint16::test_equal(uint8_t* value, uint8_t* compareto)
{
	if (*(int16_t*)compareto == *(int16_t*)value)
		return true;
	return false;
}
bool typeint16::test_unequal(uint8_t* value, uint8_t* compareto)
{
	if (*(int16_t*)compareto != *(int16_t*)value)
		return true;
	return false;
}
bool typeint16::test_less(uint8_t* value, uint8_t* compareto)
{
	if (*(int16_t*)value < *(int16_t*)compareto)
		return true;
	return false;
}
bool typeint16::test_less_equal(uint8_t* value, uint8_t* compareto)
{
	if (*(int16_t*)value <= *(int16_t*)compareto)
		return true;
	return false;
}
bool typeint16::test_greater_equal(uint8_t* value, uint8_t* compareto)
{
	if (*(int16_t*)value >= *(int16_t*)compareto)
		return true;
	return false;
}
bool typeint16::test_greater(uint8_t* value, uint8_t* compareto)
{
	if (*(int16_t*)value > *(int16_t*)compareto)
		return true;
	return false;
}



//implementation of the acquire-function - int32
datatype typeint32::acquire()
{
	//create the new object
	datatype type = datatype();

	//set the attributes used to describe the datatype
	type.size = sizeof(int32_t);
	type.addr_alignment = sizeof(int32_t);
	type.name = "int32";
	type.restricted = false;
	type.tostring = typeint32::tostring;

	//set the function to read input
	type.readinput = typeint32::readinput;

	//set the test-functions
	type.validate = typeint32::validate;
	type.test_equal = typeint32::test_equal;
	type.test_unequal = typeint32::test_unequal;
	type.test_less = typeint32::test_less;
	type.test_less_equal = typeint32::test_less_equal;
	type.test_greater_equal = typeint32::test_greater_equal;
	type.test_greater = typeint32::test_greater;

	//return the type
	return type;
}

//implementation of the functions used to describe the datatype - int32
std::string typeint32::tostring(uint8_t* value)
{
	//convert the value to a string
	std::stringstream sstr;
	sstr << *(int32_t*)value;
	return sstr.str();
}

//implementation of the function to read input - int32
bool typeint32::readinput(uint8_t* buffer)
{
	//read the input
	std::cout << "enter a value: ";
	std::string str;
	std::getline(std::cin, str);
	if (str.size() == 0)
		return false;

	//convert the input to a number
	std::stringstream sstr(str);
	int64_t value;
	sstr >> value;
	if (!sstr.eof())
		return false;

	//set the value
	*(int32_t*)buffer = (int32_t)value;
	return true;
}

//implementation of the test-functions - int32
bool typeint32::validate(uint8_t* value)
{
	return true;
}
bool typeint32::test_equal(uint8_t* value, uint8_t* compareto)
{
	if (*(int32_t*)compareto == *(int32_t*)value)
		return true;
	return false;
}
bool typeint32::test_unequal(uint8_t* value, uint8_t* compareto)
{
	if (*(int32_t*)compareto != *(int32_t*)value)
		return true;
	return false;
}
bool typeint32::test_less(uint8_t* value, uint8_t* compareto)
{
	if (*(int32_t*)value < *(int32_t*)compareto)
		return true;
	return false;
}
bool typeint32::test_less_equal(uint8_t* value, uint8_t* compareto)
{
	if (*(int32_t*)value <= *(int32_t*)compareto)
		return true;
	return false;
}
bool typeint32::test_greater_equal(uint8_t* value, uint8_t* compareto)
{
	if (*(int32_t*)value >= *(int32_t*)compareto)
		return true;
	return false;
}
bool typeint32::test_greater(uint8_t* value, uint8_t* compareto)
{
	if (*(int32_t*)value > *(int32_t*)compareto)
		return true;
	return false;
}



//implementation of the acquire-function - int64
datatype typeint64::acquire()
{
	//create the new object
	datatype type = datatype();

	//set the attributes used to describe the datatype
	type.size = sizeof(int64_t);
	type.addr_alignment = sizeof(int64_t);
	type.name = "int64";
	type.restricted = false;
	type.tostring = typeint64::tostring;

	//set the function to read input
	type.readinput = typeint64::readinput;

	//set the test-functions
	type.validate = typeint64::validate;
	type.test_equal = typeint64::test_equal;
	type.test_unequal = typeint64::test_unequal;
	type.test_less = typeint64::test_less;
	type.test_less_equal = typeint64::test_less_equal;
	type.test_greater_equal = typeint64::test_greater_equal;
	type.test_greater = typeint64::test_greater;

	//return the type
	return type;
}

//implementation of the functions used to describe the datatype - int64
std::string typeint64::tostring(uint8_t* value)
{
	//convert the value to a string
	std::stringstream sstr;
	sstr << *(int64_t*)value;
	return sstr.str();
}

//implementation of the function to read input - int64
bool typeint64::readinput(uint8_t* buffer)
{
	//read the input
	std::cout << "enter a value: ";
	std::string str;
	std::getline(std::cin, str);
	if (str.size() == 0)
		return false;

	//convert the input to a number
	std::stringstream sstr(str);
	int64_t value;
	sstr >> value;
	if (!sstr.eof())
		return false;

	//set the value
	*(int64_t*)buffer = (int64_t)value;
	return true;
}

//implementation of the test-functions - int64
bool typeint64::validate(uint8_t* value)
{
	return true;
}
bool typeint64::test_equal(uint8_t* value, uint8_t* compareto)
{
	if (*(int64_t*)compareto == *(int64_t*)value)
		return true;
	return false;
}
bool typeint64::test_unequal(uint8_t* value, uint8_t* compareto)
{
	if (*(int64_t*)compareto != *(int64_t*)value)
		return true;
	return false;
}
bool typeint64::test_less(uint8_t* value, uint8_t* compareto)
{
	if (*(int64_t*)value < *(int64_t*)compareto)
		return true;
	return false;
}
bool typeint64::test_less_equal(uint8_t* value, uint8_t* compareto)
{
	if (*(int64_t*)value <= *(int64_t*)compareto)
		return true;
	return false;
}
bool typeint64::test_greater_equal(uint8_t* value, uint8_t* compareto)
{
	if (*(int64_t*)value >= *(int64_t*)compareto)
		return true;
	return false;
}
bool typeint64::test_greater(uint8_t* value, uint8_t* compareto)
{
	if (*(int64_t*)value > *(int64_t*)compareto)
		return true;
	return false;
}