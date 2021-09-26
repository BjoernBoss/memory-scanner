#include "extra_types.h"

//implementation of the functions - bool
datatype typebool::acquire()
{
	//create the new object
	datatype type = datatype();

	//set the attributes used to describe the datatype
	type.size = sizeof(bool);
	type.addr_alignment = sizeof(bool);
	type.name = "bool";
	type.restricted = true;
	type.tostring = typebool::tostring;

	//set the function to read input
	type.readinput = typebool::readinput;

	//set the test-functions
	type.validate = typebool::validate;
	type.test_equal = typebool::test_equal;
	type.test_unequal = typebool::test_unequal;
	type.test_less = 0;
	type.test_less_equal = 0;
	type.test_greater_equal = 0;
	type.test_greater = 0;

	//return the type
	return type;
}
std::string typebool::tostring(uint8_t* value)
{
	//convert the value to a string
	std::stringstream sstr;
	sstr << *(bool*)value;
	return sstr.str();
}
bool typebool::readinput(uint8_t* buffer)
{
	//read the input
	std::cout << "enter a value: ";
	std::string str;
	std::getline(std::cin, str);
	if (str.size() == 0)
		return false;

	//convert the input to a number
	std::stringstream sstr(str);
	bool value;
	sstr >> value;
	if (!sstr.eof() || !validate((uint8_t*)&value))
		return false;

	//set the value
	*(bool*)buffer = value;
	return true;
}
bool typebool::validate(uint8_t* value)
{
	if (*value > 1)
		return false;
	return true;
}
bool typebool::test_equal(uint8_t* value, uint8_t* compareto)
{
	if (*value > 1)
		return false;
	if (*value == *compareto)
		return true;
	return false;
}
bool typebool::test_unequal(uint8_t* value, uint8_t* compareto)
{
	if (*value > 1)
		return false;
	if (*value != *compareto)
		return true;
	return false;
}

//implementation of the functions - string
datatype typestring::acquire()
{
	//create the new object
	datatype type = datatype();

	//set the attributes used to describe the datatype
	type.size = sizeof(char) * 64;
	type.addr_alignment = 1;
	type.name = "string";
	type.restricted = true;
	type.tostring = typestring::tostring;

	//set the function to read input
	type.readinput = typestring::readinput;

	//set the test-functions
	type.validate = typestring::validate;
	type.test_equal = typestring::test_equal;
	type.test_unequal = typestring::test_unequal;
	type.test_less = 0;
	type.test_less_equal = 0;
	type.test_greater_equal = 0;
	type.test_greater = 0;

	//return the type
	return type;
}
std::string typestring::tostring(uint8_t* value)
{
	//convert the value to a string
	std::stringstream sstr;
	for (uint64_t i = 0; i < 64; i++) {
		if (value[i] == 0)
			break;
		else
			sstr << value[i];
	}
	return sstr.str();
}
bool typestring::readinput(uint8_t* buffer)
{
	//read the input
	std::cout << "enter a string (max 64): ";
	std::string str;
	std::getline(std::cin, str);
	if (str.size() == 0 || str.size() > 64)
		return false;

	//set the string
	memcpy(buffer, str.c_str(), (str.size() < 64 ? str.size() + 1 : str.size()));
	return true;
}
bool typestring::validate(uint8_t* value)
{
	//validate the characters
	for (uint64_t i = 0; i < 64; i++) {
		if (value[i] == 0) {
			if (i == 0)
				return false;
			return true;
		}
		else if (value[i] < 0x20 || value[i] == 0x7f)
			return false;
	}
	return true;
}
bool typestring::test_equal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	for (uint64_t i = 0; i < 64; i++) {
		if (compareto[i] == 0)
			return true;
		if (compareto[i] != value[i])
			return false;
	}
	return true;
}
bool typestring::test_unequal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	for (uint64_t i = 0; i < 64; i++) {
		if (compareto[i] == 0)
			return false;
		if (compareto[i] != value[i])
			return true;
	}
	return false;
}

//implementation of the functions - stringTerm
datatype typestringTerm::acquire()
{
	//create the new object
	datatype type = datatype();

	//set the attributes used to describe the datatype
	type.size = sizeof(char) * 64;
	type.addr_alignment = 1;
	type.name = "stringTerm";
	type.restricted = true;
	type.tostring = typestringTerm::tostring;

	//set the function to read input
	type.readinput = typestringTerm::readinput;

	//set the test-functions
	type.validate = typestringTerm::validate;
	type.test_equal = typestringTerm::test_equal;
	type.test_unequal = typestringTerm::test_unequal;
	type.test_less = 0;
	type.test_less_equal = 0;
	type.test_greater_equal = 0;
	type.test_greater = 0;

	//return the type
	return type;
}
std::string typestringTerm::tostring(uint8_t* value)
{
	//convert the value to a stringTerm
	std::stringstream sstr;
	for (uint64_t i = 0; i < 64; i++) {
		if (value[i] == 0)
			break;
		else
			sstr << value[i];
	}
	return sstr.str();
}
bool typestringTerm::readinput(uint8_t* buffer)
{
	//read the input
	std::cout << "enter a string (max 64): ";
	std::string str;
	std::getline(std::cin, str);
	if (str.size() == 0 || str.size() > 64)
		return false;

	//set the stringTerm
	memcpy(buffer, str.c_str(), (str.size() < 64 ? str.size() + 1 : str.size()));
	return true;
}
bool typestringTerm::validate(uint8_t* value)
{
	//validate the characters
	for (uint64_t i = 0; i < 64; i++) {
		if (value[i] == 0) {
			if (i == 0)
				return false;
			return true;
		}
		else if (value[i] < 0x20 || value[i] == 0x7f)
			return false;
	}
	return true;
}
bool typestringTerm::test_equal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	for (uint64_t i = 0; i < 64; i++) {
		if (compareto[i] != value[i])
			return false;
	}
	return true;
}
bool typestringTerm::test_unequal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	for (uint64_t i = 0; i < 64; i++) {
		if (compareto[i] != value[i])
			return true;
	}
	return false;
}