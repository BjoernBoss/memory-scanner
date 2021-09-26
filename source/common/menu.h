#pragma once
#include <Windows.h>
#include "ConsoleMenu.h"
#include "datatype.h"
#include "uint_types.h"
#include "int_types.h"
#include "float_types.h"
#include "extra_types.h"
#include <iostream>
#include <string>
#include <Psapi.h>
#include <fstream>

//define the global data
enum class MemoryMode : uint8_t {
	memAll, memVolatile, memStatic
};
struct SaveEntry
{
	void* buffer;
	void* writeBuffer;
	uint64_t address;
	datatype* datatype;
	bool overwrite;
	std::string name;
};
struct MenuGlobal
{
	MemoryMode memoryMode;
	std::string processName;
	volatile HANDLE process;
	volatile bool overwrite;
	uint64_t staticSectionStart;
	uint64_t staticSectionEnd;
	uint64_t moduleOffset;
	std::vector<datatype> types;
	std::vector<SaveEntry> entries;
	datatype* selected;
	uint64_t* scanAddr;
	void* scanValue;
	uint64_t scanSize;
	bool updateScreen;
	datatype* dumpType;
	uint64_t dumpAddress;
	uint8_t dumpTypeSpecific;
	HANDLE mutex;
	HANDLE thread;
};
struct DefaultMenu
{
	constexpr static uint64_t exit				= 1;
	constexpr static uint64_t returnMenu		= exit + 1;
	constexpr static uint64_t rootMenu			= returnMenu + 1;
	constexpr static uint64_t custom			= rootMenu + 1;
};

//define the global-functions
void menuGlobalLoad(MenuStruct* menuStruct);
void menuGlobalUnload(MenuStruct* menuStruct);
MenuPageMenu DefaultMenu(MenuGlobal* global, uint64_t menuId, bool update);
bool DefaultEval(MenuGlobal* global, uint64_t type, MenuPageEval* pageEval);
void scanByBatch(MenuGlobal* global, void* comp, operationtype operation);
void scanByAddress(MenuGlobal* global, void* comp, operationtype operation);
const char* scanData(MenuGlobal* global, operationtype operation, bool self);
std::ostream& printIndex(std::ostream& stream, uint64_t index);
DWORD __stdcall MenuGlobalThread(void* ptr);

//define the root-menu
struct menuroot
{
	//define the id
	constexpr static uint64_t menuID = 1;
	constexpr static const char* title = "root menu";

	//define the functions
	static MenuPage acquire();
	static MenuPageMenu menu(MenuStruct* menuStruct);
	static MenuPageEval eval(MenuStruct* menuStruct, uint64_t type);
private:
	static const char* initialScan(MenuGlobal* global, operationtype operation);
};

//define the exitverify-menu
struct menuexitverify
{
	//define the id
	constexpr static uint64_t menuID = menuroot::menuID + 1;
	constexpr static const char* title = "exit-verify";

	//define the functions
	static MenuPage acquire();
	static MenuPageMenu menu(MenuStruct* menuStruct);
	static MenuPageEval eval(MenuStruct* menuStruct, uint64_t type);
};

//define the process-menu
struct menuprocess
{
	//define the id
	constexpr static uint64_t menuID = menuexitverify::menuID + 1;
	constexpr static const char* title = "select process";

	//define the functions
	static MenuPage acquire();
	static void load(MenuStruct* menuStruct);
	static void unload(MenuStruct* menuStruct);
	static MenuPageMenu menu(MenuStruct* menuStruct);
	static MenuPageEval eval(MenuStruct* menuStruct, uint64_t type);
private:
	static BOOL _stdcall EnumWindowsProc(HWND hwnd, LPARAM lParam);
};

//define the datatype-menu
struct menudatatype
{
	//define the id and title
	constexpr static uint64_t menuID = menuprocess::menuID + 1;;
	constexpr static const char* title = "select datatype";

	//define the functions
	static MenuPage acquire();
	static MenuPageMenu menu(MenuStruct* menuStruct);
	static MenuPageEval eval(MenuStruct* menuStruct, uint64_t type);
};

//define the datatypeverify-menu
struct menudatatypeverify
{
	//define the id and title
	constexpr static uint64_t menuID = menudatatype::menuID + 1;;
	constexpr static const char* title = "datatype-verify";

	//define the functions
	static MenuPage acquire();
	static MenuPageMenu menu(MenuStruct* menuStruct);
	static MenuPageEval eval(MenuStruct* menuStruct, uint64_t type);
};

//define the restartverify-menu
struct menurestartverify
{
	//define the id
	constexpr static uint64_t menuID = menudatatypeverify::menuID + 1;
	constexpr static const char* title = "restart-verify";

	//define the functions
	static MenuPage acquire();
	static MenuPageMenu menu(MenuStruct* menuStruct);
	static MenuPageEval eval(MenuStruct* menuStruct, uint64_t type);
};

//define the scan-menu
struct menuscan
{
	//define the id
	constexpr static uint64_t menuID = menurestartverify::menuID + 1;;
	constexpr static const char* title = "scan menu";

	//define the functions
	static MenuPage acquire();
	static MenuPageMenu menu(MenuStruct* menuStruct);
	static MenuPageEval eval(MenuStruct* menuStruct, uint64_t type);
};

//define the constant-menu
struct menuconstant
{
	//define the id
	constexpr static uint64_t menuID = menuscan::menuID + 1;
	constexpr static const char* title = "fluctuation-filter";

	//define the functions
	static MenuPage acquire();
	static bool update(MenuStruct* menuStruct);
	static MenuPageMenu menu(MenuStruct* menuStruct);
	static MenuPageEval eval(MenuStruct* menuStruct, uint64_t type);
};

//define the print-menu
struct menuprint
{
	//define the id
	constexpr static uint64_t menuID = menuconstant::menuID + 1;
	constexpr static const char* title = "current scan";

	//define the functions
	static MenuPage acquire();
	static bool update(MenuStruct* menuStruct);
	static MenuPageMenu menu(MenuStruct* menuStruct);
	static MenuPageEval eval(MenuStruct* menuStruct, uint64_t type);
};

//define the entries-menu
struct menuentries
{
	//define the id
	constexpr static uint64_t menuID = menuprint::menuID + 1;
	constexpr static const char* title = "saved entries";

	//define the functions
	static MenuPage acquire();
	static bool update(MenuStruct* menuStruct);
	static MenuPageMenu menu(MenuStruct* menuStruct);
	static MenuPageEval eval(MenuStruct* menuStruct, uint64_t type);
};

//define the dump-menu
struct menudump
{
	struct DumpInfo
	{
		uint64_t lastPrint;
		bool scanValid;
		uint8_t buffer[256];
	};

	//define the id
	constexpr static uint64_t menuID = menuentries::menuID + 1;
	constexpr static const char* title = "memory viewer";

	//define the functions
	static MenuPage acquire();
	static void load(MenuStruct* menuStruct);
	static void unload(MenuStruct* menuStruct);
	static bool update(MenuStruct* menuStruct);
	static MenuPageMenu menu(MenuStruct* menuStruct);
	static MenuPageEval eval(MenuStruct* menuStruct, uint64_t type);
};

//define the dumptype-menu
struct menudumptype
{
	//define the id
	constexpr static uint64_t menuID = menudump::menuID + 1;
	constexpr static const char* title = "memory viewer datatype";

	//define the functions
	static MenuPage acquire();
	static MenuPageMenu menu(MenuStruct* menuStruct);
	static MenuPageEval eval(MenuStruct* menuStruct, uint64_t type);
};