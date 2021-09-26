#include "float_types.h"

//implementation of the acquire-function - float
datatype typefloat::acquire()
{
	//create the new object
	datatype type = datatype();

	//set the attributes used to describe the datatype
	type.size = sizeof(float);
	type.addr_alignment = sizeof(float);
	type.name = "float";
	type.restricted = false;
	type.tostring = typefloat::tostring;

	//set the function to read input
	type.readinput = typefloat::readinput;

	//set the test-functions
	type.validate = typefloat::validate;
	type.test_equal = typefloat::test_equal;
	type.test_unequal = typefloat::test_unequal;
	type.test_less = typefloat::test_less;
	type.test_less_equal = typefloat::test_less_equal;
	type.test_greater_equal = typefloat::test_greater_equal;
	type.test_greater = typefloat::test_greater;

	//return the type
	return type;
}

//implementation of the functions used to describe the datatype - float
std::string typefloat::tostring(uint8_t* value)
{
	//convert the value to a string
	std::stringstream sstr;
	sstr << *(float*)value;
	return sstr.str();
}

//implementation of the function to read input - float
bool typefloat::readinput(uint8_t* buffer)
{
	//read the input
	std::cout << "enter a value: ";
	std::string str;
	std::getline(std::cin, str);
	if (str.size() == 0)
		return false;

	//convert the input to a number
	std::stringstream sstr(str);
	float value;
	sstr >> value;
	if (!sstr.eof() || !validate((uint8_t*)&value))
		return false;

	//set the value
	*(float*)buffer = value;
	return true;
}

//implementation of the test-functions - float
bool typefloat::validate(uint8_t* value)
{
	if (isnan(*(float*)value))
		return false;
	if (isinf(*(float*)value))
		return false;
	if (*(float*)value > 1.0e20f || *(float*)value < -1.0e20f)
		return false;
	return true;
}
bool typefloat::test_equal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (abs(*(float*)compareto - *(float*)value) < 1.0e-4)
		return true;
	return false;
}
bool typefloat::test_unequal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (abs(*(float*)compareto - *(float*)value) >= 1.0e-4)
		return true;
	return false;
}
bool typefloat::test_less(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (*(float*)value < *(float*)compareto)
		return true;
	return false;
}
bool typefloat::test_less_equal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (*(float*)value <= *(float*)compareto)
		return true;
	return false;
}
bool typefloat::test_greater_equal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (*(float*)value >= *(float*)compareto)
		return true;
	return false;
}
bool typefloat::test_greater(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (*(float*)value > *(float*)compareto)
		return true;
	return false;
}



//implementation of the acquire-function - double
datatype typedouble::acquire()
{
	//create the new object
	datatype type = datatype();

	//set the attributes used to describe the datatype
	type.size = sizeof(double);
	type.addr_alignment = sizeof(double);
	type.name = "double";
	type.restricted = false;
	type.tostring = typedouble::tostring;

	//set the function to read input
	type.readinput = typedouble::readinput;

	//set the test-functions
	type.validate = typedouble::validate;
	type.test_equal = typedouble::test_equal;
	type.test_unequal = typedouble::test_unequal;
	type.test_less = typedouble::test_less;
	type.test_less_equal = typedouble::test_less_equal;
	type.test_greater_equal = typedouble::test_greater_equal;
	type.test_greater = typedouble::test_greater;

	//return the type
	return type;
}

//implementation of the functions used to describe the datatype - double
std::string typedouble::tostring(uint8_t* value)
{
	//convert the value to a string
	std::stringstream sstr;
	sstr << *(double*)value;
	return sstr.str();
}

//implementation of the function to read input - double
bool typedouble::readinput(uint8_t* buffer)
{
	//read the input
	std::cout << "enter a value: ";
	std::string str;
	std::getline(std::cin, str);
	if (str.size() == 0)
		return false;

	//convert the input to a number
	std::stringstream sstr(str);
	double value;
	sstr >> value;
	if (!sstr.eof() || !validate((uint8_t*)&value))
		return false;

	//set the value
	*(double*)buffer = value;
	return true;
}

//implementation of the test-functions - double
bool typedouble::validate(uint8_t* value)
{
	if (isnan(*(double*)value))
		return false;
	if (isinf(*(double*)value))
		return false;
	if (*(double*)value > 1.0e20 || *(double*)value < -1.0e20)
		return false;
	return true;
}
bool typedouble::test_equal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (abs(*(double*)compareto - *(double*)value) < 1.0e-4)
		return true;
	return false;
}
bool typedouble::test_unequal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (abs(*(double*)compareto - *(double*)value) >= 1.0e-4)
		return true;
	return false;
}
bool typedouble::test_less(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (*(double*)value < *(double*)compareto)
		return true;
	return false;
}
bool typedouble::test_less_equal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (*(double*)value <= *(double*)compareto)
		return true;
	return false;
}
bool typedouble::test_greater_equal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (*(double*)value >= *(double*)compareto)
		return true;
	return false;
}
bool typedouble::test_greater(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (*(double*)value > *(double*)compareto)
		return true;
	return false;
}



//implementation of the acquire-function - float2dx
datatype typefloat2Dx::acquire()
{
	//create the new object
	datatype type = datatype();

	//set the attributes used to describe the datatype
	type.size = sizeof(float) * 2;
	type.addr_alignment = sizeof(float);
	type.name = "float2Dx";
	type.restricted = false;
	type.tostring = typefloat2Dx::tostring;

	//set the function to read input
	type.readinput = typefloat2Dx::readinput;

	//set the test-functions
	type.validate = typefloat2Dx::validate;
	type.test_equal = typefloat2Dx::test_equal;
	type.test_unequal = typefloat2Dx::test_unequal;
	type.test_less = typefloat2Dx::test_less;
	type.test_less_equal = typefloat2Dx::test_less_equal;
	type.test_greater_equal = typefloat2Dx::test_greater_equal;
	type.test_greater = typefloat2Dx::test_greater;

	//return the type
	return type;
}

//implementation of the functions used to describe the datatype - float2dx
std::string typefloat2Dx::tostring(uint8_t* value)
{
	//convert the value to a string
	std::stringstream sstr;
	sstr << ((float*)value)[0] << "/" << ((float*)value)[1];
	return sstr.str();
}

//implementation of the function to read input - float2dx
bool typefloat2Dx::readinput(uint8_t* buffer)
{
	//variables
	float buf[2];

	//read the input
	std::cout << "enter value x: ";
	std::string str;
	std::getline(std::cin, str);
	if (str.size() == 0)
		return false;

	//convert the input to a number
	std::stringstream sstr(str);
	sstr >> buf[0];
	if (!sstr.eof())
		return false;

	//read the input
	std::cout << "enter value y: ";
	std::getline(std::cin, str);
	if (str.size() == 0)
		return false;

	//convert the input to a number
	sstr = std::stringstream(str);
	sstr >> buf[1];
	if (!sstr.eof() || !validate((uint8_t*)buf))
		return false;

	//set the value
	memcpy(buffer, buf, sizeof(float) * 2);
	return true;
}

//implementation of the test-functions - float2dx
bool typefloat2Dx::validate(uint8_t* value)
{
	for (int i = 0; i < 2; i++)
	{
		if (isnan(((float*)value)[i]))
			return false;
		if (isinf(((float*)value)[i]))
			return false;
		if (((float*)value)[i] > 1.0e20f || ((float*)value)[i] < -1.0e20f)
			return false;
	}
	return true;
}
bool typefloat2Dx::test_equal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (abs(((float*)compareto)[0] - ((float*)value)[0]) < 1.0e-4)
		return true;
	return false;
}
bool typefloat2Dx::test_unequal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (abs(((float*)compareto)[0] - ((float*)value)[0]) >= 1.0e-4)
		return true;
	return false;
}
bool typefloat2Dx::test_less(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (((float*)value)[0] < ((float*)compareto)[0])
		return true;
	return false;
}
bool typefloat2Dx::test_less_equal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (((float*)value)[0] <= ((float*)compareto)[0])
		return true;
	return false;
}
bool typefloat2Dx::test_greater_equal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (((float*)value)[0] >= ((float*)compareto)[0])
		return true;
	return false;
}
bool typefloat2Dx::test_greater(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (((float*)value)[0] > ((float*)compareto)[0])
		return true;
	return false;
}



//implementation of the acquire-function - float2dy
datatype typefloat2Dy::acquire()
{
	//create the new object
	datatype type = datatype();

	//set the attributes used to describe the datatype
	type.size = sizeof(float) * 2;
	type.addr_alignment = sizeof(float);
	type.name = "float2Dy";
	type.restricted = false;
	type.tostring = typefloat2Dy::tostring;

	//set the function to read input
	type.readinput = typefloat2Dy::readinput;

	//set the test-functions
	type.validate = typefloat2Dy::validate;
	type.test_equal = typefloat2Dy::test_equal;
	type.test_unequal = typefloat2Dy::test_unequal;
	type.test_less = typefloat2Dy::test_less;
	type.test_less_equal = typefloat2Dy::test_less_equal;
	type.test_greater_equal = typefloat2Dy::test_greater_equal;
	type.test_greater = typefloat2Dy::test_greater;

	//return the type
	return type;
}

//implementation of the functions used to describe the datatype - float2dy
std::string typefloat2Dy::tostring(uint8_t* value)
{
	//convert the value to a string
	std::stringstream sstr;
	sstr << ((float*)value)[0] << "/" << ((float*)value)[1];
	return sstr.str();
}

//implementation of the function to read input - float2dy
bool typefloat2Dy::readinput(uint8_t* buffer)
{
	//variables
	float buf[2];

	//read the input
	std::cout << "enter value x: ";
	std::string str;
	std::getline(std::cin, str);
	if (str.size() == 0)
		return false;

	//convert the input to a number
	std::stringstream sstr(str);
	sstr >> buf[0];
	if (!sstr.eof())
		return false;

	//read the input
	std::cout << "enter value y: ";
	std::getline(std::cin, str);
	if (str.size() == 0)
		return false;

	//convert the input to a number
	sstr = std::stringstream(str);
	sstr >> buf[1];
	if (!sstr.eof() || !validate((uint8_t*)buf))
		return false;

	//set the value
	memcpy(buffer, buf, sizeof(float) * 2);
	return true;
}

//implementation of the test-functions - float2dy
bool typefloat2Dy::validate(uint8_t* value)
{
	for (int i = 0; i < 2; i++)
	{
		if (isnan(((float*)value)[i]))
			return false;
		if (isinf(((float*)value)[i]))
			return false;
		if (((float*)value)[i] > 1.0e20f || ((float*)value)[i] < -1.0e20f)
			return false;
	}
	return true;
}
bool typefloat2Dy::test_equal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (abs(((float*)compareto)[1] - ((float*)value)[1]) < 1.0e-4)
		return true;
	return false;
}
bool typefloat2Dy::test_unequal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (abs(((float*)compareto)[1] - ((float*)value)[1]) >= 1.0e-4)
		return true;
	return false;
}
bool typefloat2Dy::test_less(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (((float*)value)[1] < ((float*)compareto)[1])
		return true;
	return false;
}
bool typefloat2Dy::test_less_equal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (((float*)value)[1] <= ((float*)compareto)[1])
		return true;
	return false;
}
bool typefloat2Dy::test_greater_equal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (((float*)value)[1] >= ((float*)compareto)[1])
		return true;
	return false;
}
bool typefloat2Dy::test_greater(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (((float*)value)[1] > ((float*)compareto)[1])
		return true;
	return false;
}



//implementation of the acquire-function - float3dx
datatype typefloat3Dx::acquire()
{
	//create the new object
	datatype type = datatype();

	//set the attributes used to describe the datatype
	type.size = sizeof(float) * 3;
	type.addr_alignment = sizeof(float);
	type.name = "float3Dx";
	type.restricted = false;
	type.tostring = typefloat3Dx::tostring;

	//set the function to read input
	type.readinput = typefloat3Dx::readinput;

	//set the test-functions
	type.validate = typefloat3Dx::validate;
	type.test_equal = typefloat3Dx::test_equal;
	type.test_unequal = typefloat3Dx::test_unequal;
	type.test_less = typefloat3Dx::test_less;
	type.test_less_equal = typefloat3Dx::test_less_equal;
	type.test_greater_equal = typefloat3Dx::test_greater_equal;
	type.test_greater = typefloat3Dx::test_greater;

	//return the type
	return type;
}

//implementation of the functions used to describe the datatype - float3dx
std::string typefloat3Dx::tostring(uint8_t* value)
{
	//convert the value to a string
	std::stringstream sstr;
	sstr << ((float*)value)[0] << "/" << ((float*)value)[1] << "/" << ((float*)value)[2];
	return sstr.str();
}

//implementation of the function to read input - float3dx
bool typefloat3Dx::readinput(uint8_t* buffer)
{
	//variables
	float buf[3];

	//read the input
	std::cout << "enter value x: ";
	std::string str;
	std::getline(std::cin, str);
	if (str.size() == 0)
		return false;

	//convert the input to a number
	std::stringstream sstr(str);
	sstr >> buf[0];
	if (!sstr.eof())
		return false;

	//read the input
	std::cout << "enter value y: ";
	std::getline(std::cin, str);
	if (str.size() == 0)
		return false;

	//convert the input to a number
	sstr = std::stringstream(str);
	sstr >> buf[1];
	if (!sstr.eof())
		return false;

	//read the input
	std::cout << "enter value z: ";
	std::getline(std::cin, str);
	if (str.size() == 0)
		return false;

	//convert the input to a number
	sstr = std::stringstream(str);
	sstr >> buf[2];
	if (!sstr.eof() || !validate((uint8_t*)buf))
		return false;

	//set the value
	memcpy(buffer, buf, sizeof(float) * 3);
	return true;
}

//implementation of the test-functions - float3dx
bool typefloat3Dx::validate(uint8_t* value)
{
	for (int i = 0; i < 3; i++)
	{
		if (isnan(((float*)value)[i]))
			return false;
		if (isinf(((float*)value)[i]))
			return false;
		if (((float*)value)[i] > 1.0e20f || ((float*)value)[i] < -1.0e20f)
			return false;
	}
	return true;
}
bool typefloat3Dx::test_equal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (abs(((float*)compareto)[0] - ((float*)value)[0]) < 1.0e-4)
		return true;
	return false;
}
bool typefloat3Dx::test_unequal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (abs(((float*)compareto)[0] - ((float*)value)[0]) >= 1.0e-4)
		return true;
	return false;
}
bool typefloat3Dx::test_less(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (((float*)value)[0] < ((float*)compareto)[0])
		return true;
	return false;
}
bool typefloat3Dx::test_less_equal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (((float*)value)[0] <= ((float*)compareto)[0])
		return true;
	return false;
}
bool typefloat3Dx::test_greater_equal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (((float*)value)[0] >= ((float*)compareto)[0])
		return true;
	return false;
}
bool typefloat3Dx::test_greater(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (((float*)value)[0] > ((float*)compareto)[0])
		return true;
	return false;
}



//implementation of the acquire-function - float3dy
datatype typefloat3Dy::acquire()
{
	//create the new object
	datatype type = datatype();

	//set the attributes used to describe the datatype
	type.size = sizeof(float) * 3;
	type.addr_alignment = sizeof(float);
	type.name = "float3Dy";
	type.restricted = false;
	type.tostring = typefloat3Dy::tostring;

	//set the function to read input
	type.readinput = typefloat3Dy::readinput;

	//set the test-functions
	type.validate = typefloat3Dy::validate;
	type.test_equal = typefloat3Dy::test_equal;
	type.test_unequal = typefloat3Dy::test_unequal;
	type.test_less = typefloat3Dy::test_less;
	type.test_less_equal = typefloat3Dy::test_less_equal;
	type.test_greater_equal = typefloat3Dy::test_greater_equal;
	type.test_greater = typefloat3Dy::test_greater;

	//return the type
	return type;
}

//implementation of the functions used to describe the datatype - float3dy
std::string typefloat3Dy::tostring(uint8_t* value)
{
	//convert the value to a string
	std::stringstream sstr;
	sstr << ((float*)value)[0] << "/" << ((float*)value)[1] << "/" << ((float*)value)[2];
	return sstr.str();
}

//implementation of the function to read input - float3dy
bool typefloat3Dy::readinput(uint8_t* buffer)
{
	//variables
	float buf[3];

	//read the input
	std::cout << "enter value x: ";
	std::string str;
	std::getline(std::cin, str);
	if (str.size() == 0)
		return false;

	//convert the input to a number
	std::stringstream sstr(str);
	sstr >> buf[0];
	if (!sstr.eof())
		return false;

	//read the input
	std::cout << "enter value y: ";
	std::getline(std::cin, str);
	if (str.size() == 0)
		return false;

	//convert the input to a number
	sstr = std::stringstream(str);
	sstr >> buf[1];
	if (!sstr.eof())
		return false;

	//read the input
	std::cout << "enter value z: ";
	std::getline(std::cin, str);
	if (str.size() == 0)
		return false;

	//convert the input to a number
	sstr = std::stringstream(str);
	sstr >> buf[2];
	if (!sstr.eof() || !validate((uint8_t*)buf))
		return false;

	//set the value
	memcpy(buffer, buf, sizeof(float) * 3);
	return true;
}

//implementation of the test-functions - float3dy
bool typefloat3Dy::validate(uint8_t* value)
{
	for (int i = 0; i < 3; i++)
	{
		if (isnan(((float*)value)[i]))
			return false;
		if (isinf(((float*)value)[i]))
			return false;
		if (((float*)value)[i] > 1.0e20f || ((float*)value)[i] < -1.0e20f)
			return false;
	}
	return true;
}
bool typefloat3Dy::test_equal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (abs(((float*)compareto)[1] - ((float*)value)[1]) < 1.0e-4)
		return true;
	return false;
}
bool typefloat3Dy::test_unequal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (abs(((float*)compareto)[1] - ((float*)value)[1]) >= 1.0e-4)
		return true;
	return false;
}
bool typefloat3Dy::test_less(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (((float*)value)[1] < ((float*)compareto)[1])
		return true;
	return false;
}
bool typefloat3Dy::test_less_equal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (((float*)value)[1] <= ((float*)compareto)[1])
		return true;
	return false;
}
bool typefloat3Dy::test_greater_equal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (((float*)value)[1] >= ((float*)compareto)[1])
		return true;
	return false;
}
bool typefloat3Dy::test_greater(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (((float*)value)[1] > ((float*)compareto)[1])
		return true;
	return false;
}



//implementation of the acquire-function - float3dz
datatype typefloat3Dz::acquire()
{
	//create the new object
	datatype type = datatype();

	//set the attributes used to describe the datatype
	type.size = sizeof(float) * 3;
	type.addr_alignment = sizeof(float);
	type.name = "float3Dz";
	type.restricted = false;
	type.tostring = typefloat3Dz::tostring;

	//set the function to read input
	type.readinput = typefloat3Dz::readinput;

	//set the test-functions
	type.validate = typefloat3Dz::validate;
	type.test_equal = typefloat3Dz::test_equal;
	type.test_unequal = typefloat3Dz::test_unequal;
	type.test_less = typefloat3Dz::test_less;
	type.test_less_equal = typefloat3Dz::test_less_equal;
	type.test_greater_equal = typefloat3Dz::test_greater_equal;
	type.test_greater = typefloat3Dz::test_greater;

	//return the type
	return type;
}

//implementation of the functions used to describe the datatype - float3dz
std::string typefloat3Dz::tostring(uint8_t* value)
{
	//convert the value to a string
	std::stringstream sstr;
	sstr << ((float*)value)[0] << "/" << ((float*)value)[1] << "/" << ((float*)value)[2];
	return sstr.str();
}

//implementation of the function to read input - float3dz
bool typefloat3Dz::readinput(uint8_t* buffer)
{
	//variables
	float buf[3];

	//read the input
	std::cout << "enter value x: ";
	std::string str;
	std::getline(std::cin, str);
	if (str.size() == 0)
		return false;

	//convert the input to a number
	std::stringstream sstr(str);
	sstr >> buf[0];
	if (!sstr.eof())
		return false;

	//read the input
	std::cout << "enter value y: ";
	std::getline(std::cin, str);
	if (str.size() == 0)
		return false;

	//convert the input to a number
	sstr = std::stringstream(str);
	sstr >> buf[1];
	if (!sstr.eof())
		return false;

	//read the input
	std::cout << "enter value z: ";
	std::getline(std::cin, str);
	if (str.size() == 0)
		return false;

	//convert the input to a number
	sstr = std::stringstream(str);
	sstr >> buf[2];
	if (!sstr.eof() || !validate((uint8_t*)buf))
		return false;

	//set the value
	memcpy(buffer, buf, sizeof(float) * 3);
	return true;
}

//implementation of the test-functions - float3dz
bool typefloat3Dz::validate(uint8_t* value)
{
	for (int i = 0; i < 3; i++)
	{
		if (isnan(((float*)value)[i]))
			return false;
		if (isinf(((float*)value)[i]))
			return false;
		if (((float*)value)[i] > 1.0e20f || ((float*)value)[i] < -1.0e20f)
			return false;
	}
	return true;
}
bool typefloat3Dz::test_equal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (abs(((float*)compareto)[2] - ((float*)value)[2]) < 1.0e-4)
		return true;
	return false;
}
bool typefloat3Dz::test_unequal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (abs(((float*)compareto)[2] - ((float*)value)[2]) >= 1.0e-4)
		return true;
	return false;
}
bool typefloat3Dz::test_less(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (((float*)value)[2] < ((float*)compareto)[2])
		return true;
	return false;
}
bool typefloat3Dz::test_less_equal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (((float*)value)[2] <= ((float*)compareto)[2])
		return true;
	return false;
}
bool typefloat3Dz::test_greater_equal(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (((float*)value)[2] >= ((float*)compareto)[2])
		return true;
	return false;
}
bool typefloat3Dz::test_greater(uint8_t* value, uint8_t* compareto)
{
	if (!validate(value))
		return false;
	if (((float*)value)[2] > ((float*)compareto)[2])
		return true;
	return false;
}