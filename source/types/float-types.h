#pragma once
#include "datatype.h"
#include <math.h>

//define the float-datatype
struct typefloat
{
	//define the acquire-function
	static datatype acquire();

	//define the functions used to describe the datatype
	static std::string tostring(uint8_t* value);

	//define the function to read input
	static bool readinput(uint8_t* buffer);

	//define the test-functions
	static bool validate(uint8_t* value);
	static bool test_equal(uint8_t* compareto, uint8_t* value);
	static bool test_unequal(uint8_t* compareto, uint8_t* value);
	static bool test_less(uint8_t* compareto, uint8_t* value);
	static bool test_less_equal(uint8_t* compareto, uint8_t* value);
	static bool test_greater_equal(uint8_t* compareto, uint8_t* value);
	static bool test_greater(uint8_t* compareto, uint8_t* value);
};

//define the double-datatype
struct typedouble
{
	//define the acquire-function
	static datatype acquire();

	//define the functions used to describe the datatype
	static std::string tostring(uint8_t* value);

	//define the function to read input
	static bool readinput(uint8_t* buffer);

	//define the test-functions
	static bool validate(uint8_t* value);
	static bool test_equal(uint8_t* compareto, uint8_t* value);
	static bool test_unequal(uint8_t* compareto, uint8_t* value);
	static bool test_less(uint8_t* compareto, uint8_t* value);
	static bool test_less_equal(uint8_t* compareto, uint8_t* value);
	static bool test_greater_equal(uint8_t* compareto, uint8_t* value);
	static bool test_greater(uint8_t* compareto, uint8_t* value);
};

//define the float2Dx-datatype
struct typefloat2Dx
{
	//define the acquire-function
	static datatype acquire();

	//define the functions used to describe the datatype
	static std::string tostring(uint8_t* value);

	//define the function to read input
	static bool readinput(uint8_t* buffer);

	//define the test-functions
	static bool validate(uint8_t* value);
	static bool test_equal(uint8_t* compareto, uint8_t* value);
	static bool test_unequal(uint8_t* compareto, uint8_t* value);
	static bool test_less(uint8_t* compareto, uint8_t* value);
	static bool test_less_equal(uint8_t* compareto, uint8_t* value);
	static bool test_greater_equal(uint8_t* compareto, uint8_t* value);
	static bool test_greater(uint8_t* compareto, uint8_t* value);
};

//define the float2Dy-datatype
struct typefloat2Dy
{
	//define the acquire-function
	static datatype acquire();

	//define the functions used to describe the datatype
	static std::string tostring(uint8_t* value);

	//define the function to read input
	static bool readinput(uint8_t* buffer);

	//define the test-functions
	static bool validate(uint8_t* value);
	static bool test_equal(uint8_t* compareto, uint8_t* value);
	static bool test_unequal(uint8_t* compareto, uint8_t* value);
	static bool test_less(uint8_t* compareto, uint8_t* value);
	static bool test_less_equal(uint8_t* compareto, uint8_t* value);
	static bool test_greater_equal(uint8_t* compareto, uint8_t* value);
	static bool test_greater(uint8_t* compareto, uint8_t* value);
};

//define the float3Dx-datatype
struct typefloat3Dx
{
	//define the acquire-function
	static datatype acquire();

	//define the functions used to describe the datatype
	static std::string tostring(uint8_t* value);

	//define the function to read input
	static bool readinput(uint8_t* buffer);

	//define the test-functions
	static bool validate(uint8_t* value);
	static bool test_equal(uint8_t* compareto, uint8_t* value);
	static bool test_unequal(uint8_t* compareto, uint8_t* value);
	static bool test_less(uint8_t* compareto, uint8_t* value);
	static bool test_less_equal(uint8_t* compareto, uint8_t* value);
	static bool test_greater_equal(uint8_t* compareto, uint8_t* value);
	static bool test_greater(uint8_t* compareto, uint8_t* value);
};

//define the float3Dy-datatype
struct typefloat3Dy
{
	//define the acquire-function
	static datatype acquire();

	//define the functions used to describe the datatype
	static std::string tostring(uint8_t* value);

	//define the function to read input
	static bool readinput(uint8_t* buffer);

	//define the test-functions
	static bool validate(uint8_t* value);
	static bool test_equal(uint8_t* compareto, uint8_t* value);
	static bool test_unequal(uint8_t* compareto, uint8_t* value);
	static bool test_less(uint8_t* compareto, uint8_t* value);
	static bool test_less_equal(uint8_t* compareto, uint8_t* value);
	static bool test_greater_equal(uint8_t* compareto, uint8_t* value);
	static bool test_greater(uint8_t* compareto, uint8_t* value);
};

//define the float3Dz-datatype
struct typefloat3Dz
{
	//define the acquire-function
	static datatype acquire();

	//define the functions used to describe the datatype
	static std::string tostring(uint8_t* value);

	//define the function to read input
	static bool readinput(uint8_t* buffer);

	//define the test-functions
	static bool validate(uint8_t* value);
	static bool test_equal(uint8_t* compareto, uint8_t* value);
	static bool test_unequal(uint8_t* compareto, uint8_t* value);
	static bool test_less(uint8_t* compareto, uint8_t* value);
	static bool test_less_equal(uint8_t* compareto, uint8_t* value);
	static bool test_greater_equal(uint8_t* compareto, uint8_t* value);
	static bool test_greater(uint8_t* compareto, uint8_t* value);
};