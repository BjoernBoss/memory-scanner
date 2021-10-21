/* SPDX-License-Identifier: BSD-3-Clause */
/* Copyright (c) 2021 Bjoern Boss Henrichsen */
#pragma once

#include <Console-Menu/console-menu.h>
#include <common/datatype.h>

#include <string>
#include <iostream>
#include <vector>
#define NOMINMAX
#include <Windows.h>

/* define the global menu instance */
class MenuInstance : public menu::Instance {
public:
	enum class MemoryMode : uint8_t {
		memAll, memVolatile, memStatic
	};
	struct Entry {
		uint8_t* buffer;
		uint8_t* writeBuffer;
		uint64_t address;
		Datatype* datatype;
		bool overwrite;
		std::string name;

	public:
		Entry();
	};
	struct Default {
		static constexpr menu::EntryId exit = 1;
		static constexpr menu::EntryId returnMenu = exit + 1;
		static constexpr menu::EntryId rootMenu = returnMenu + 1;
		static constexpr menu::EntryId custom = rootMenu + 1;
	};

public:
	MemoryMode memoryMode;
	std::string processName;
	volatile HANDLE process;
	volatile bool overwrite;
	uint64_t moduleOffset;
	uint64_t staticSectionStart;
	uint64_t staticSectionEnd;
	std::vector<Datatype*> types;
	std::vector<Entry> entries;
	Datatype* selected;
	uint8_t* intermediate;
	uint64_t* scanAddr;
	uint8_t* scanValue;
	size_t scanSize;
	Datatype* dumpType;
	uint64_t dumpAddress;
	uint8_t dumpTypeSpecific;
	CRITICAL_SECTION mutex;
	HANDLE thread;
	volatile uint8_t threadState;

public:
	MenuInstance();

public:
	virtual void init() override;
	virtual void teardown() override;
	virtual const char* root() override;

public:
	menu::Layout layout(const char* current, bool update);
	bool eval(menu::EntryId selected, menu::Behavior* behavior);
	Entry& entry(uint64_t address, Datatype* type, const uint8_t* buffer, std::string name = "");
	const char* queryIndex(const std::string& text, size_t& index, size_t limit);

public:
	const char* scanInitial(Datatype::Operation operation);
	void scanByBatch(const uint8_t* comp, Datatype::Operation operation);
	void scanByAddress(const uint8_t* comp, Datatype::Operation operation);
	const char* scanData(Datatype::Operation operation, bool self);

public:
	std::ostream& printIndex(std::ostream& stream, uint64_t index);
	static DWORD __stdcall globalThread(void* ptr);
};

/* define the root page */
struct RootPage : public menu::Page {
public:
	constexpr static const char* Id = "root";
	constexpr static const char* Title = "root menu";

private:
	using menu::Page::Page;

public:
	static RootPage* acquire();

public:
	menu::Layout construct() override;
	menu::Behavior evaluate(menu::EntryId selected) override;
	void teardown() override;
};

/* define the exit verify page */
struct ExitVerifyPage : public menu::Page {
public:
	constexpr static const char* Id = "exitverify";
	constexpr static const char* Title = "exit-verify";

private:
	using menu::Page::Page;

public:
	static ExitVerifyPage* acquire();

public:
	menu::Layout construct() override;
	menu::Behavior evaluate(menu::EntryId selected) override;
	void teardown() override;
};

/* define the process page */
struct ProcessPage : public menu::Page {
public:
	constexpr static const char* Id = "process";
	constexpr static const char* Title = "select process";

private:
	using menu::Page::Page;

private:
	std::vector<HWND> pWindows;

public:
	static ProcessPage* acquire();

public:
	void unload() override;
	menu::Layout construct() override;
	menu::Behavior evaluate(menu::EntryId selected) override;
	void teardown() override;

private:
	static BOOL _stdcall fEnumWindowsProc(HWND hwnd, LPARAM lParam);
};

/* define the datatype page */
struct DatatypePage : public menu::Page {
public:
	constexpr static const char* Id = "datatype";
	constexpr static const char* Title = "select datatype";

private:
	using menu::Page::Page;

public:
	static DatatypePage* acquire();

public:
	menu::Layout construct() override;
	menu::Behavior evaluate(menu::EntryId selected) override;
	void teardown() override;
};

/* define the datatype verify page */
struct DatatypeVerifyPage : public menu::Page {
public:
	constexpr static const char* Id = "datatypeverify";
	constexpr static const char* Title = "datatype-verify";

private:
	using menu::Page::Page;

public:
	static DatatypeVerifyPage* acquire();

public:
	menu::Layout construct() override;
	menu::Behavior evaluate(menu::EntryId selected) override;
	void teardown() override;
};

/* define the restart verify page */
struct RestartVerifyPage : public menu::Page {
public:
	constexpr static const char* Id = "restartverify";
	constexpr static const char* Title = "restart-verify";

private:
	using menu::Page::Page;

public:
	static RestartVerifyPage* acquire();

public:
	menu::Layout construct() override;
	menu::Behavior evaluate(menu::EntryId selected) override;
	void teardown() override;
};

/* define the scan page */
struct ScanPage : public menu::Page {
public:
	constexpr static const char* Id = "scan";
	constexpr static const char* Title = "scan menu";

private:
	using menu::Page::Page;

public:
	static ScanPage* acquire();

public:
	menu::Layout construct() override;
	menu::Behavior evaluate(menu::EntryId selected) override;
	void teardown() override;
};

/* define the constant page */
struct ConstantPage : public menu::Page {
public:
	constexpr static const char* Id = "constant";
	constexpr static const char* Title = "fluctuation-filter";

private:
	using menu::Page::Page;

private:
	uint64_t pStamp;
	bool pRefresh;

public:
	static ConstantPage* acquire();

public:
	bool update() override;
	menu::Layout construct() override;
	menu::Behavior evaluate(menu::EntryId selected) override;
	void teardown() override;
};

/* define the print page */
struct PrintPage : public menu::Page {
public:
	constexpr static const char* Id = "print";
	constexpr static const char* Title = "current scan";

private:
	using menu::Page::Page;

private:
	uint64_t pStamp;
	bool pRefresh;

public:
	static PrintPage* acquire();

public:
	bool update() override;
	menu::Layout construct() override;
	menu::Behavior evaluate(menu::EntryId selected) override;
	void teardown() override;
};

/* define the entries page */
struct EntriesPage : public menu::Page {
public:
	constexpr static const char* Id = "entries";
	constexpr static const char* Title = "saved entries";

private:
	using menu::Page::Page;

private:
	uint64_t pStamp;
	bool pRefresh;

public:
	static EntriesPage* acquire();

public:
	bool update() override;
	menu::Layout construct() override;
	menu::Behavior evaluate(menu::EntryId selected) override;
	void teardown() override;
};

/* define the dump page */
struct DumpPage : public menu::Page {
public:
	constexpr static const char* Id = "dump";
	constexpr static const char* Title = "memory viewer";

private:
	using menu::Page::Page;

private:
	uint64_t pStamp;
	bool pRefresh;
	bool pScanValid;
	uint8_t pBuffer[256];

public:
	static DumpPage* acquire();

public:
	bool update() override;
	menu::Layout construct() override;
	menu::Behavior evaluate(menu::EntryId selected) override;
	void teardown() override;
};

/* define the dumptype page */
struct DumptypePage : public menu::Page {
public:
	constexpr static const char* Id = "dumptype";
	constexpr static const char* Title = "memory viewer datatype";

private:
	using menu::Page::Page;

public:
	static DumptypePage* acquire();

public:
	menu::Layout construct() override;
	menu::Behavior evaluate(menu::EntryId selected) override;
	void teardown() override;
};