#pragma once
#include "datatype.h"

//define the uint8-datatype
struct typeuint8
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

//define the uint16-datatype
struct typeuint16
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

//define the uint32-datatype
struct typeuint32
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

//define the uint64-datatype
struct typeuint64
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