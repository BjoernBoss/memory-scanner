#pragma once
#include <inttypes.h>
#include <string>
#include <iostream>
#include <sstream>
#include <Windows.h>

//define the operations
enum class operationtype : uint8_t
{
	validate, 
	equal, 
	unequal, 
	less, 
	less_equal, 
	greater_equal, 
	greater
};

//define the structure describing one datatype
struct datatype
{
	//implement the constructors
	datatype();
	~datatype();

	//define the attributes used to describe the datatype
	uint8_t size;
	uint8_t addr_alignment;
	const char* name;
	bool restricted;
	std::string(*tostring)(uint8_t*);

	//define the function to read input
	bool(*readinput)(uint8_t*);

	//define the test-functions
	bool(*validate)(uint8_t*);
	bool(*test_equal)(uint8_t*, uint8_t*);
	bool(*test_unequal)(uint8_t*, uint8_t*);
	bool(*test_less)(uint8_t*, uint8_t*);
	bool(*test_less_equal)(uint8_t*, uint8_t*);
	bool(*test_greater_equal)(uint8_t*, uint8_t*);
	bool(*test_greater)(uint8_t*, uint8_t*);
};