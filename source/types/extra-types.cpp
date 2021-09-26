#include "extra-types.h"

#include <iostream>
#include <sstream>
#include <algorithm>

/* implement the boolean type */
types::Bool::Bool() : Datatype(sizeof(bool), sizeof(bool), "bool", true) {}
std::string types::Bool::toString(const uint8_t* value) {
	return std::to_string(*value);
}
bool types::Bool::readInput(uint8_t* value) {
	/* read the line from the input */
	std::cout << "enter a value (0/1): ";
	std::string line;
	std::getline(std::cin, line);
	if (line.empty())
		return false;

	/* parse the number */
	std::stringstream sstr(line);
	return (sstr >> *value) && sstr.eof() && *value <= 1;
}
bool types::Bool::validate(const uint8_t* value) {
	return *value <= 1;
}
bool types::Bool::test(const uint8_t* value, const uint8_t* compareto, Operation operation) {
	/* check if the value is valid (compareto is expected to be valid) */
	if (*value > 1)
		return false;

	/* handle the separate operations */
	switch (operation) {
	case Operation::equal:
		return *value == *compareto;
	case Operation::unequal:
		return *value != *compareto;
	}
	return false;
}

/* implement the string types */
types::String::String(bool term) : Datatype(sizeof(char) * 64, sizeof(char), term ? "string-term" : "string", true), pTerminated(term) {}
bool types::String::fValidate(const char* value) const {
	/* iterate through the characters and validate them */
	for (size_t i = 0; i < 64; i++) {
		if (value[i] == 0)
			return i > 0;
		if (value[i] < 0x20 || value[i] == 0x7f)
			return false;
	}
	return !pTerminated;
}
std::string types::String::toString(const uint8_t* value) {
	/* check if the length needs to be determined */
	if (pTerminated)
		return reinterpret_cast<const char*>(value);

	/* determine the length of the string */
	size_t len = 0;
	while (value[len] && len < 64)
		++len;
	return std::move(std::string(reinterpret_cast<const char*>(value), len));
}
bool types::String::readInput(uint8_t* value) {
	std::string line;

	/* read the input */
	std::cout << "enter a string (max " << (pTerminated ? "63" : "64") << "): ";
	std::getline(std::cin, line);
	if (line.empty() == 0)
		return false;

	/* validate the input */
	if (!fValidate(line.c_str()))
		return false;

	/* copy the string into the buffer */
	std::memcpy(value, line.data(), std::min(line.size() + 1, static_cast<size_t>(64)));
}
bool types::String::validate(const uint8_t* value) {
	return fValidate(reinterpret_cast<const char*>(value));
}
bool types::String::test(const uint8_t* value, const uint8_t* compareto, Operation operation) {
	/* check if the value is valid (compareto is expected to be valid) */
	if (!fValidate(reinterpret_cast<const char*>(value)))
		return false;

	/* check if the two strings are considered equal */
	bool equal = true;
	for (size_t i = 0; i < 64; i++) {
		if (value[i] == 0) {
			if (pTerminated && compareto[i] != 0)
				equal = false;
			break;
		}
		if (value[i] == compareto[i])
			continue;
		equal = false;
		break;
	}

	/* handle the separate operations */
	switch (operation) {
	case Operation::equal:
		return equal;
	case Operation::unequal:
		return !equal;
	}
	return false;
}