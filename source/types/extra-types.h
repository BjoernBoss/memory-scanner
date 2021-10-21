/* SPDX-License-Identifier: BSD-3-Clause */
/* Copyright (c) 2021 Bjoern Boss Henrichsen */
#pragma once

#include <common/datatype.h>

namespace types {
	/* define the boolean type */
	class Bool : public Datatype {
	public:
		Bool();

	public:
		std::string toString(const uint8_t* value) override;
		bool readInput(uint8_t* value, bool operation) override;
		bool validate(const uint8_t* value) override;
		bool test(const uint8_t* value, const uint8_t* compareto, Operation operation) override;
	};

	/* define the string types */
	class String : public Datatype {
	private:
		bool pTerminated;

	public:
		String(bool term);

	private:
		bool fValidate(const char* value) const;

	public:
		std::string toString(const uint8_t* value) override;
		bool readInput(uint8_t* value, bool operation) override;
		bool validate(const uint8_t* value) override;
		bool test(const uint8_t* value, const uint8_t* compareto, Operation operation) override;
	};
}