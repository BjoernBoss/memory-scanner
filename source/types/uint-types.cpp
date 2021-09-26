#include "uint_types.h"

//implementation of the acquire-function - uint8
datatype typeuint8::acquire()
{
	//create the new object
	datatype type = datatype();

	//set the attributes used to describe the datatype
	type.size = sizeof(uint8_t);
	type.addr_alignment = sizeof(uint8_t);
	type.name = "uint8";
	type.restricted = false;
	type.tostring = typeuint8::tostring;

	//set the function to read input
	type.readinput = typeuint8::readinput;

	//set the test-functions
	type.validate = typeuint8::validate;
	type.test_equal = typeuint8::test_equal;
	type.test_unequal = typeuint8::test_unequal;
	type.test_less = typeuint8::test_less;
	type.test_less_equal = typeuint8::test_less_equal;
	type.test_greater_equal = typeuint8::test_greater_equal;
	type.test_greater = typeuint8::test_greater;

	//return the type
	return type;
}

//implementation of the functions used to describe the datatype - uint8
std::string typeuint8::tostring(uint8_t* value)
{
	//convert the value to a string
	std::stringstream sstr;
	sstr << (uint16_t)*(uint8_t*)value;
	return sstr.str();
}

//implementation of the function to read input - uint8
bool typeuint8::readinput(uint8_t* buffer)
{
	//read the input
	std::cout << "enter a value: ";
	std::string str;
	std::getline(std::cin, str);
	if (str.size() == 0)
		return false;

	//convert the input to a number
	std::stringstream sstr(str);
	uint64_t value;
	sstr >> value;
	if (!sstr.eof())
		return false;

	//set the value
	*(uint8_t*)buffer = (uint8_t)value;
	return true;
}

//implementation of the test-functions - uint8
bool typeuint8::validate(uint8_t* value)
{
	return true;
}
bool typeuint8::test_equal(uint8_t* value, uint8_t* compareto)
{
	if (*(uint8_t*)compareto == *(uint8_t*)value)
		return true;
	return false;
}
bool typeuint8::test_unequal(uint8_t* value, uint8_t* compareto)
{
	if (*(uint8_t*)compareto != *(uint8_t*)value)
		return true;
	return false;
}
bool typeuint8::test_less(uint8_t* value, uint8_t* compareto)
{
	if (*(uint8_t*)value < *(uint8_t*)compareto)
		return true;
	return false;
}
bool typeuint8::test_less_equal(uint8_t* value, uint8_t* compareto)
{
	if (*(uint8_t*)value <= *(uint8_t*)compareto)
		return true;
	return false;
}
bool typeuint8::test_greater_equal(uint8_t* value, uint8_t* compareto)
{
	if (*(uint8_t*)value >= *(uint8_t*)compareto)
		return true;
	return false;
}
bool typeuint8::test_greater(uint8_t* value, uint8_t* compareto)
{
	if (*(uint8_t*)value > *(uint8_t*)compareto)
		return true;
	return false;
}



//implementation of the acquire-function - uint16
datatype typeuint16::acquire()
{
	//create the new object
	datatype type = datatype();

	//set the attributes used to describe the datatype
	type.size = sizeof(uint16_t);
	type.addr_alignment = sizeof(uint16_t);
	type.name = "uint16";
	type.restricted = false;
	type.tostring = typeuint16::tostring;

	//set the function to read input
	type.readinput = typeuint16::readinput;

	//set the test-functions
	type.validate = typeuint16::validate;
	type.test_equal = typeuint16::test_equal;
	type.test_unequal = typeuint16::test_unequal;
	type.test_less = typeuint16::test_less;
	type.test_less_equal = typeuint16::test_less_equal;
	type.test_greater_equal = typeuint16::test_greater_equal;
	type.test_greater = typeuint16::test_greater;

	//return the type
	return type;
}

//implementation of the functions used to describe the datatype - uint16
std::string typeuint16::tostring(uint8_t* value)
{
	//convert the value to a string
	std::stringstream sstr;
	sstr << *(uint16_t*)value;
	return sstr.str();
}

//implementation of the function to read input - uint16
bool typeuint16::readinput(uint8_t* buffer)
{
	//read the input
	std::cout << "enter a value: ";
	std::string str;
	std::getline(std::cin, str);
	if (str.size() == 0)
		return false;

	//convert the input to a number
	std::stringstream sstr(str);
	uint64_t value;
	sstr >> value;
	if (!sstr.eof())
		return false;

	//set the value
	*(uint16_t*)buffer = (uint16_t)value;
	return true;
}

//implementation of the test-functions - uint16
bool typeuint16::validate(uint8_t* value)
{
	return true;
}
bool typeuint16::test_equal(uint8_t* value, uint8_t* compareto)
{
	if (*(uint16_t*)compareto == *(uint16_t*)value)
		return true;
	return false;
}
bool typeuint16::test_unequal(uint8_t* value, uint8_t* compareto)
{
	if (*(uint16_t*)compareto != *(uint16_t*)value)
		return true;
	return false;
}
bool typeuint16::test_less(uint8_t* value, uint8_t* compareto)
{
	if (*(uint16_t*)value < *(uint16_t*)compareto)
		return true;
	return false;
}
bool typeuint16::test_less_equal(uint8_t* value, uint8_t* compareto)
{
	if (*(uint16_t*)value <= *(uint16_t*)compareto)
		return true;
	return false;
}
bool typeuint16::test_greater_equal(uint8_t* value, uint8_t* compareto)
{
	if (*(uint16_t*)value >= *(uint16_t*)compareto)
		return true;
	return false;
}
bool typeuint16::test_greater(uint8_t* value, uint8_t* compareto)
{
	if (*(uint16_t*)value > *(uint16_t*)compareto)
		return true;
	return false;
}



//implementation of the acquire-function - uint32
datatype typeuint32::acquire()
{
	//create the new object
	datatype type = datatype();

	//set the attributes used to describe the datatype
	type.size = sizeof(uint32_t);
	type.addr_alignment = sizeof(uint32_t);
	type.name = "uint32";
	type.restricted = false;
	type.tostring = typeuint32::tostring;

	//set the function to read input
	type.readinput = typeuint32::readinput;

	//set the test-functions
	type.validate = typeuint32::validate;
	type.test_equal = typeuint32::test_equal;
	type.test_unequal = typeuint32::test_unequal;
	type.test_less = typeuint32::test_less;
	type.test_less_equal = typeuint32::test_less_equal;
	type.test_greater_equal = typeuint32::test_greater_equal;
	type.test_greater = typeuint32::test_greater;

	//return the type
	return type;
}

//implementation of the functions used to describe the datatype - uint32
std::string typeuint32::tostring(uint8_t* value)
{
	//convert the value to a string
	std::stringstream sstr;
	sstr << *(uint32_t*)value;
	return sstr.str();
}

//implementation of the function to read input - uint32
bool typeuint32::readinput(uint8_t* buffer)
{
	//read the input
	std::cout << "enter a value: ";
	std::string str;
	std::getline(std::cin, str);
	if (str.size() == 0)
		return false;

	//convert the input to a number
	std::stringstream sstr(str);
	uint64_t value;
	sstr >> value;
	if (!sstr.eof())
		return false;

	//set the value
	*(uint32_t*)buffer = (uint32_t)value;
	return true;
}

//implementation of the test-functions - uint32
bool typeuint32::validate(uint8_t* value)
{
	return true;
}
bool typeuint32::test_equal(uint8_t* value, uint8_t* compareto)
{
	if (*(uint32_t*)compareto == *(uint32_t*)value)
		return true;
	return false;
}
bool typeuint32::test_unequal(uint8_t* value, uint8_t* compareto)
{
	if (*(uint32_t*)compareto != *(uint32_t*)value)
		return true;
	return false;
}
bool typeuint32::test_less(uint8_t* value, uint8_t* compareto)
{
	if (*(uint32_t*)value < *(uint32_t*)compareto)
		return true;
	return false;
}
bool typeuint32::test_less_equal(uint8_t* value, uint8_t* compareto)
{
	if (*(uint32_t*)value <= *(uint32_t*)compareto)
		return true;
	return false;
}
bool typeuint32::test_greater_equal(uint8_t* value, uint8_t* compareto)
{
	if (*(uint32_t*)value >= *(uint32_t*)compareto)
		return true;
	return false;
}
bool typeuint32::test_greater(uint8_t* value, uint8_t* compareto)
{
	if (*(uint32_t*)value > *(uint32_t*)compareto)
		return true;
	return false;
}



//implementation of the acquire-function - uint64
datatype typeuint64::acquire()
{
	//create the new object
	datatype type = datatype();

	//set the attributes used to describe the datatype
	type.size = sizeof(uint64_t);
	type.addr_alignment = sizeof(uint64_t);
	type.name = "uint64";
	type.restricted = false;
	type.tostring = typeuint64::tostring;

	//set the function to read input
	type.readinput = typeuint64::readinput;

	//set the test-functions
	type.validate = typeuint64::validate;
	type.test_equal = typeuint64::test_equal;
	type.test_unequal = typeuint64::test_unequal;
	type.test_less = typeuint64::test_less;
	type.test_less_equal = typeuint64::test_less_equal;
	type.test_greater_equal = typeuint64::test_greater_equal;
	type.test_greater = typeuint64::test_greater;

	//return the type
	return type;
}

//implementation of the functions used to describe the datatype - uint64
std::string typeuint64::tostring(uint8_t* value)
{
	//convert the value to a string
	std::stringstream sstr;
	sstr << *(uint64_t*)value;
	return sstr.str();
}

//implementation of the function to read input - uint64
bool typeuint64::readinput(uint8_t* buffer)
{
	//read the input
	std::cout << "enter a value: ";
	std::string str;
	std::getline(std::cin, str);
	if (str.size() == 0)
		return false;

	//convert the input to a number
	std::stringstream sstr(str);
	uint64_t value;
	sstr >> value;
	if (!sstr.eof())
		return false;

	//set the value
	*(uint64_t*)buffer = (uint64_t)value;
	return true;
}

//implementation of the test-functions - uint64
bool typeuint64::validate(uint8_t* value)
{
	return true;
}
bool typeuint64::test_equal(uint8_t* value, uint8_t* compareto)
{
	if (*(uint64_t*)compareto == *(uint64_t*)value)
		return true;
	return false;
}
bool typeuint64::test_unequal(uint8_t* value, uint8_t* compareto)
{
	if (*(uint64_t*)compareto != *(uint64_t*)value)
		return true;
	return false;
}
bool typeuint64::test_less(uint8_t* value, uint8_t* compareto)
{
	if (*(uint64_t*)value < *(uint64_t*)compareto)
		return true;
	return false;
}
bool typeuint64::test_less_equal(uint8_t* value, uint8_t* compareto)
{
	if (*(uint64_t*)value <= *(uint64_t*)compareto)
		return true;
	return false;
}
bool typeuint64::test_greater_equal(uint8_t* value, uint8_t* compareto)
{
	if (*(uint64_t*)value >= *(uint64_t*)compareto)
		return true;
	return false;
}
bool typeuint64::test_greater(uint8_t* value, uint8_t* compareto)
{
	if (*(uint64_t*)value > *(uint64_t*)compareto)
		return true;
	return false;
}