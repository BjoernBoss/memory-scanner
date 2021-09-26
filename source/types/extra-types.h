#pragma once
#include "datatype.h"

//define the bool-datatype
struct typebool
{
	//define the functions
	static datatype acquire();
	static std::string tostring(uint8_t* value);
	static bool readinput(uint8_t* buffer);
	static bool validate(uint8_t* value);
	static bool test_equal(uint8_t* compareto, uint8_t* value);
	static bool test_unequal(uint8_t* compareto, uint8_t* value);
};

//define the string-datatype
struct typestring
{
	//define the functions
	static datatype acquire();
	static std::string tostring(uint8_t* value);
	static bool readinput(uint8_t* buffer);
	static bool validate(uint8_t* value);
	static bool test_equal(uint8_t* compareto, uint8_t* value);
	static bool test_unequal(uint8_t* compareto, uint8_t* value);
};

//define the stringTerm-datatype
struct typestringTerm
{
	//define the functions
	static datatype acquire();
	static std::string tostring(uint8_t* value);
	static bool readinput(uint8_t* buffer);
	static bool validate(uint8_t* value);
	static bool test_equal(uint8_t* compareto, uint8_t* value);
	static bool test_unequal(uint8_t* compareto, uint8_t* value);
};