#pragma once

#include <common/datatype.h>

#include <iostream>
#include <sstream>
#include <math.h>

namespace types {
	namespace helper {
		/* define the helper function for float validation */
		template <class Type>
		static bool FloatValidate(const Type& value) {
			static constexpr Type Bound = static_cast<Type>(1.0e20f);
			if (std::isnan<Type>(value) || std::isinf<Type>(value))
				return false;

			/* check if the values is within the bounds */
			return (value >= -Bound && value <= Bound);
		}

		/* define the helper function for float comparisions */
		template <class Type>
		static bool FloatEqual(const Type& a, const Type& b) {
			static constexpr Type Precision = static_cast<Type>(0.00001f);

			/* check if one of the variables is zero, in which case the comparison does not work */
			if (a == static_cast<Type>(0.0f))
				return std::abs(b) <= Precision;
			if (b == static_cast<Type>(0.0f))
				return std::abs(a) <= Precision;

			/* compare the actual value */
			const Type _a = std::abs(a);
			const Type _b = std::abs(b);
			return std::abs(a - b) <= std::min(_a, _b) * Precision;
		}
	}

	/* define the float type */
	template <class Type>
	class Float : public Datatype {
	public:
		Float();

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
			if (!(sstr >> *reinterpret_cast<Type*>(value)) || !sstr.eof())
				return false;
			return helper::FloatValidate<Type>(*reinterpret_cast<Type*>(value));
		}
		bool validate(const uint8_t* value) override {
			return helper::FloatValidate<Type>(*reinterpret_cast<const Type*>(value));
		}
		bool test(const uint8_t* value, const uint8_t* compareto, Operation operation) override {
			const Type& a = *reinterpret_cast<const Type*>(value);
			const Type& b = *reinterpret_cast<const Type*>(compareto);

			/* check if the value is valid (compareto is expected to be valid) */
			if (!helper::FloatValidate<Type>(a))
				return false;

			/* handle the separate operations */
			switch (operation) {
			case Operation::equal:
				return helper::FloatEqual<Type>(a, b);
			case Operation::unequal:
				return !helper::FloatEqual<Type>(a, b);
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

	/* define the 2d-float structure type */
	class Float2 : public Datatype {
	private:
		size_t pIndex;

	public:
		Float2(size_t index);

	public:
		std::string toString(const uint8_t* value) override;
		bool readInput(uint8_t* value, bool operation) override;
		bool validate(const uint8_t* value) override;
		bool test(const uint8_t* value, const uint8_t* compareto, Operation operation) override;
	};

	/* define the 3d-float structure type */
	class Float3 : public Datatype {
	private:
		size_t pIndex;

	public:
		Float3(size_t index);

	public:
		std::string toString(const uint8_t* value) override;
		bool readInput(uint8_t* value, bool operation) override;
		bool validate(const uint8_t* value) override;
		bool test(const uint8_t* value, const uint8_t* compareto, Operation operation) override;
	};
}