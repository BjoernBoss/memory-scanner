#pragma once

#include <common/datatype.h>

#include <iostream>
#include <sstream>
#include <type_traits>

namespace types {
	/* define the int type */
	template <class Type>
	class Int : public Datatype {
	public:
		Int();

	public:
		std::string toString(const uint8_t* value) override {
			return std::to_string(*reinterpret_cast<const Type*>(value));
		}
		bool readInput(uint8_t* value, bool operation) override {
			/* read the line from the input */
			std::cout << "enter a value: ";
			std::string line;
			std::getline(std::cin, line);
			if (line.empty())
				return false;

			/* parse the number */
			std::stringstream sstr(line);
			std::conditional_t<std::is_signed_v<Type>, int64_t, uint64_t> temp = 0;
			if (!(sstr >> temp) || !sstr.eof())
				return false;
			*reinterpret_cast<Type*>(value) = static_cast<Type>(temp);
			return true;
		}
		bool validate(const uint8_t* value) override {
			return true;
		}
		bool test(const uint8_t* value, const uint8_t* compareto, Operation operation) override {
			const Type& a = *reinterpret_cast<const Type*>(value);
			const Type& b = *reinterpret_cast<const Type*>(compareto);

			/* handle the separate operations */
			switch (operation) {
			case Operation::equal:
				return a == b;
			case Operation::unequal:
				return a != b;
			case Operation::less:
				return a < b;
			case Operation::lessEqual:
				return a <= b;
			case Operation::greaterEqual:
				return a >= b;
			case Operation::greater:
				return a > b;
			}
			return false;
		}
	};
}