#include "float-types.h"

#include <iomanip>

/* implement the float type constructors */
types::Float<float>::Float() : Datatype(sizeof(float), sizeof(float), "float", false) {}
types::Float<double>::Float() : Datatype(sizeof(double), sizeof(double), "double", false) {}

/* implement the 2d-float structure type */
types::Float2::Float2(size_t index) : Datatype(sizeof(float) * 2, sizeof(float), index == 0 ? "float2x" : "float2y", false), pIndex(index) {}
std::string types::Float2::toString(const uint8_t* value) {
	const float* packed = reinterpret_cast<const float*>(value);

	/* construct the string */
	std::stringstream sstr;
	sstr << std::setfill(' ') << std::setw(10) << packed[0] << '/' << std::setw(10) << packed[1];
	return sstr.str();
}
bool types::Float2::readInput(uint8_t* value) {
	float* packed = reinterpret_cast<float*>(value);
	std::string line;
	std::stringstream sstr;

	/* read the first variable */
	std::cout << "enter value x" << (pIndex == 0 ? "(primary)" : "") << ": ";
	std::getline(std::cin, line);
	if (line.empty())
		return false;

	/* parse the number */
	sstr.str(line);
	if (!(sstr >> packed[0]) || !sstr.eof() || !helper::FloatValidate<float>(packed[0]))
		return false;

	/* read the second variable */
	std::cout << "enter value y" << (pIndex == 1 ? "(primary)" : "") << ": ";
	std::getline(std::cin, line);
	if (line.empty())
		return false;

	/* parse the second number */
	sstr.str(line);
	if (!(sstr >> packed[0]) || !sstr.eof() || !helper::FloatValidate<float>(packed[1]))
		return false;
	return true;
}
bool types::Float2::validate(const uint8_t* value) {
	const float* packed = reinterpret_cast<const float*>(value);
	return helper::FloatValidate<float>(packed[0]) && helper::FloatValidate<float>(packed[1]);
}
bool types::Float2::test(const uint8_t* value, const uint8_t* compareto, Operation operation) {
	const float* a = reinterpret_cast<const float*>(value);
	const float* b = reinterpret_cast<const float*>(compareto);

	/* check if the value is valid (compareto is expected to be valid) */
	if (!helper::FloatValidate<float>(a[0]) || !helper::FloatValidate<float>(a[1]))
		return false;

	/* handle the separate operations */
	switch (operation) {
	case Operation::equal:
		return helper::FloatEqual<float>(a[pIndex], b[pIndex]);
	case Operation::unequal:
		return !helper::FloatEqual<float>(a[pIndex], b[pIndex]);
	case Operation::less:
		return a[pIndex] < b[pIndex];
	case Operation::lessEqual:
		return a[pIndex] <= b[pIndex];
	case Operation::greaterEqual:
		return a[pIndex] >= b[pIndex];
	case Operation::greater:
		return a[pIndex] > b[pIndex];
	}
	return false;
}

/* implement the 3d-float structure type */
types::Float3::Float3(size_t index) : Datatype(sizeof(float) * 3, sizeof(float), index == 0 ? "float3x" : (index == 1 ? "float3y" : "float3z"), false), pIndex(index) {}
std::string types::Float3::toString(const uint8_t* value) {
	const float* packed = reinterpret_cast<const float*>(value);

	/* construct the string */
	std::stringstream sstr;
	sstr << std::setfill(' ') << std::setw(10) << packed[0] << '/' << std::setw(10) << packed[1] << '/' << std::setw(10) << packed[2];
	return sstr.str();
}
bool types::Float3::readInput(uint8_t* value) {
	float* packed = reinterpret_cast<float*>(value);
	std::string line;
	std::stringstream sstr;

	/* read the first variable */
	std::cout << "enter value x" << (pIndex == 0 ? "(primary)" : "") << ": ";
	std::getline(std::cin, line);
	if (line.empty())
		return false;

	/* parse the number */
	sstr.str(line);
	if (!(sstr >> packed[0]) || !sstr.eof() || !helper::FloatValidate<float>(packed[0]))
		return false;

	/* read the second variable */
	std::cout << "enter value y" << (pIndex == 1 ? "(primary)" : "") << ": ";
	std::getline(std::cin, line);
	if (line.empty())
		return false;

	/* parse the second number */
	sstr.str(line);
	if (!(sstr >> packed[1]) || !sstr.eof() || !helper::FloatValidate<float>(packed[1]))
		return false;

	/* read the third variable */
	std::cout << "enter value z" << (pIndex == 2 ? "(primary)" : "") << ": ";
	std::getline(std::cin, line);
	if (line.empty())
		return false;

	/* parse the third number */
	sstr.str(line);
	if (!(sstr >> packed[2]) || !sstr.eof() || !helper::FloatValidate<float>(packed[2]))
		return false;
	return true;
}
bool types::Float3::validate(const uint8_t* value) {
	const float* packed = reinterpret_cast<const float*>(value);
	return helper::FloatValidate<float>(packed[0]) && helper::FloatValidate<float>(packed[1]) && helper::FloatValidate<float>(packed[2]);
}
bool types::Float3::test(const uint8_t* value, const uint8_t* compareto, Operation operation) {
	const float* a = reinterpret_cast<const float*>(value);
	const float* b = reinterpret_cast<const float*>(compareto);

	/* check if the value is valid (compareto is expected to be valid) */
	if (!helper::FloatValidate<float>(a[0]) || !helper::FloatValidate<float>(a[1]) || !helper::FloatValidate<float>(a[2]))
		return false;

	/* handle the separate operations */
	switch (operation) {
	case Operation::equal:
		return helper::FloatEqual<float>(a[pIndex], b[pIndex]);
	case Operation::unequal:
		return !helper::FloatEqual<float>(a[pIndex], b[pIndex]);
	case Operation::less:
		return a[pIndex] < b[pIndex];
	case Operation::lessEqual:
		return a[pIndex] <= b[pIndex];
	case Operation::greaterEqual:
		return a[pIndex] >= b[pIndex];
	case Operation::greater:
		return a[pIndex] > b[pIndex];
	}
	return false;
}