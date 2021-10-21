/* SPDX-License-Identifier: BSD-3-Clause */
/* Copyright (c) 2021 Bjoern Boss Henrichsen */
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
	std::string pName;
	bool pRestricted;
	uint8_t pSize;
	uint8_t pAlign;

protected:
	Datatype(uint8_t size, uint8_t align, const std::string& name, bool restricted);
	Datatype(Datatype&&) = delete;
	Datatype(const Datatype&) = delete;

public:
	virtual ~Datatype() = default;

public:
	virtual std::string toString(const uint8_t* value) = 0;
	virtual bool readInput(uint8_t* value, bool operation) = 0;
	virtual bool validate(const uint8_t* value) = 0;
	virtual bool test(const uint8_t* value, const uint8_t* compareto, Operation operation) = 0;

public:
	uint8_t size() const;
	uint8_t align() const;
	const std::string& name() const;
	bool restricted() const;
};