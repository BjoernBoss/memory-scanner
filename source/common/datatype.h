#pragma once

#include <inttypes.h>
#include <string>

/* define the datatype interface */
class Datatype {
public:
	enum class Operation : uint8_t {
		validate,
		equal,
		unequal,
		less,
		lessEqual,
		greaterEqual,
		greater
	};

private:
	uint8_t pSize;
	uint8_t pAlign;
	std::string pName;
	bool pRestricted;

protected:
	Datatype(uint8_t size, uint8_t align, const std::string& name, bool restricted);
	virtual ~Datatype() = default;
	Datatype(Datatype&&) = delete;
	Datatype(const Datatype&) = delete;

public:
	virtual std::string toString(const uint8_t* value) = 0;
	virtual bool readInput(uint8_t* value) = 0;
	virtual bool validate(const uint8_t* value) = 0;
	virtual bool test(const uint8_t* value, const uint8_t* compareto, Operation operation) = 0;
};