#include "memMenu.h"
using namespace std;

//implementation of the global-functions
void menuGlobalLoad(MenuStruct* menuStruct)
{
	//set the unload & default-function
	menuStruct->setGlobalUnload(menuGlobalUnload);

	//allocate and initialize the global-data
	MenuGlobal* global = (MenuGlobal*)malloc(sizeof(MenuGlobal));
	memset(global, 0, sizeof(MenuGlobal));
	global->entries = vector<SaveEntry>();
	global->process = INVALID_HANDLE_VALUE;
	global->processName = string();
	global->types = vector<datatype>();
	global->overwrite = false;
	global->moduleOffset = (uint64_t)-1;
	global->memoryMode = MemoryMode::memStatic;

	//create the resources
	global->mutex = CreateMutexW(0, 0, 0);
	global->thread = CreateThread(0, 0, MenuGlobalThread, (void*)global, 0, 0);

	//add all of the datatypes
	global->types.push_back(typeuint8::acquire());
	global->types.push_back(typeuint16::acquire());
	global->types.push_back(typeuint32::acquire());
	global->types.push_back(typeuint64::acquire());
	global->types.push_back(typeint8::acquire());
	global->types.push_back(typeint16::acquire());
	global->types.push_back(typeint32::acquire());
	global->types.push_back(typeint64::acquire());
	global->types.push_back(typefloat::acquire());
	global->types.push_back(typedouble::acquire());
	global->types.push_back(typebool::acquire());
	global->types.push_back(typestring::acquire());
	global->types.push_back(typestringTerm::acquire());
	global->types.push_back(typefloat2Dx::acquire());
	global->types.push_back(typefloat2Dy::acquire());
	global->types.push_back(typefloat3Dx::acquire());
	global->types.push_back(typefloat3Dy::acquire());
	global->types.push_back(typefloat3Dz::acquire());

	//set the global-data
	menuStruct->setGlobalData(global);

	//load all of the menu-pages
	menuStruct->attach(menuroot::acquire());
	menuStruct->attach(menuexitverify::acquire());
	menuStruct->attach(menuprocess::acquire());
	menuStruct->attach(menudatatype::acquire());
	menuStruct->attach(menudatatypeverify::acquire());
	menuStruct->attach(menurestartverify::acquire());
	menuStruct->attach(menuscan::acquire());
	menuStruct->attach(menuconstant::acquire());
	menuStruct->attach(menuprint::acquire());
	menuStruct->attach(menuentries::acquire());
	menuStruct->attach(menudump::acquire());
	menuStruct->attach(menudumptype::acquire());
}
void menuGlobalUnload(MenuStruct* menuStruct)
{
	//release the global-data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();
	menuStruct->setGlobalData(0);

	//destroy the thread and the mutex
	WaitForSingleObject(global->mutex, INFINITE);
	TerminateThread(global->thread, 0);
	CloseHandle(global->thread);
	CloseHandle(global->mutex);

	//release all of the save-entries
	for (uint64_t i = 0; i < global->entries.size(); i++)
	{
		global->entries.at(i).name.~basic_string();
		free(global->entries.at(i).buffer);
	}

	//release all of the other resources
	global->entries.~vector();
	if (global->process != INVALID_HANDLE_VALUE)
		CloseHandle(global->process);
	global->processName.~basic_string();
	if (global->scanSize > 0)
	{
		free(global->scanValue);
		free(global->scanAddr);
	}
	global->types.~vector();

	//free the global-object
	free(global);

	//unload the menu-pages
	menuStruct->detach(menuroot::menuID);
	menuStruct->detach(menuexitverify::menuID);
	menuStruct->detach(menuprocess::menuID);
	menuStruct->detach(menudatatype::menuID);
	menuStruct->detach(menudatatypeverify::menuID);
	menuStruct->detach(menurestartverify::menuID);
	menuStruct->detach(menuscan::menuID);
	menuStruct->detach(menuconstant::menuID);
	menuStruct->detach(menuprint::menuID);
	menuStruct->detach(menuentries::menuID);
	menuStruct->detach(menudump::menuID);
	menuStruct->detach(menudumptype::menuID);

	//unload the default-functions
	menuStruct->setGlobalUnload(0);
}
MenuPageMenu DefaultMenu(MenuGlobal* global, uint64_t menuId, bool update)
{
	//create the page-object
	MenuPageMenu pageMenu = MenuPageMenu(update, "");

	//setup the default-string
	if (global->process != INVALID_HANDLE_VALUE)
		pageMenu.MenuString.append("current process: ").append(global->processName);
	else
		pageMenu.MenuString.append("current process: none");
	if (global->overwrite)
		pageMenu.MenuString.append("\noverwrite: enabled");
	else
		pageMenu.MenuString.append("\noverwrite: disabled");
	if (global->memoryMode == MemoryMode::memAll)
		pageMenu.MenuString.append("\nMemory: all");
	else if (global->memoryMode == MemoryMode::memStatic)
		pageMenu.MenuString.append("\nMemory: static");
	else if (global->memoryMode == MemoryMode::memVolatile)
		pageMenu.MenuString.append("\nMemory: volatile");
	if (global->moduleOffset != (uint64_t)-1)
	{
		stringstream ss;
		ss << hex << " [module: 0x" << (void*)global->moduleOffset << "]";
		pageMenu.MenuString.append(ss.str());
	}
	else
		pageMenu.MenuString.append(" [module: unknown]");
	if (global->selected != 0)
		pageMenu.MenuString.append(" (type: ").append(global->selected->name).append(")");
	else
		pageMenu.MenuString.append(" (type: none)");
	if (global->scanSize > 0)
	{
		stringstream ss;
		ss << dec << global->scanSize;
		pageMenu.MenuString.append(" -> scan-size: ").append(ss.str());
	}
	else
		pageMenu.MenuString.append(" -> scan-size: 0");

	//add the menu-entries
	pageMenu.entries.push_back(MenuEntry(DefaultMenu::exit, "exit"));
	if (menuId != menuroot::menuID)
	{
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::returnMenu, "return"));
		if (menuId != menuprocess::menuID && menuId != menudatatype::menuID)
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::rootMenu, "root menu"));
	}
	return pageMenu;
}
bool DefaultEval(MenuGlobal* global, uint64_t type, MenuPageEval* pageEval)
{
	//handle the default types
	switch (type)
	{
	case DefaultMenu::exit:
		pageEval->traverse = MenuTraverse::push;
		pageEval->menuId = menuexitverify::menuID;
		return true;
	case DefaultMenu::returnMenu:
		pageEval->traverse = MenuTraverse::pop;
		return true;
	case DefaultMenu::rootMenu:
		pageEval->traverse = MenuTraverse::root;
		return true;
	}
	return false;
}
void scanByBatch(MenuGlobal* global, void* comp, operationtype operation)
{
	//update the operation-pointer
	bool(*test_function)(uint8_t*, uint8_t*) = 0;
	switch (operation)
	{
	case operationtype::validate:
		test_function = 0;
		break;
	case operationtype::equal:
		test_function = global->selected->test_equal;
		break;
	case operationtype::unequal:
		test_function = global->selected->test_unequal;
		break;
	case operationtype::less:
		test_function = global->selected->test_less;
		break;
	case operationtype::less_equal:
		test_function = global->selected->test_less_equal;
		break;
	case operationtype::greater_equal:
		test_function = global->selected->test_greater_equal;
		break;
	case operationtype::greater:
		test_function = global->selected->test_greater;
		break;
	}

	//loop through all of the batches and scan them
	uint64_t index = 0;
	uint64_t offset = 0;
	MEMORY_BASIC_INFORMATION mem_info;
	uint64_t nextaddress = 0;
	uint64_t percent_counter = 0;
	while (VirtualQueryEx(global->process, (void*)nextaddress, &mem_info, sizeof(MEMORY_BASIC_INFORMATION)))
	{
		//remove all of the rest of the addresses before this batch
		bool done = (index >= global->scanSize);
		while (!done)
		{
			if (index >= global->scanSize)
				done = true;
			else if (global->scanAddr[index] < (uint64_t)mem_info.BaseAddress)
			{
				offset++;
				index++;
			}
			else
				done = true;
		}

		//validate the batch
		if ((mem_info.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE)) && (mem_info.State & MEM_COMMIT) && (mem_info.Protect & (PAGE_GUARD | PAGE_NOACCESS)) == 0)
		{
			//allocate a buffer to hold the batch
			uint8_t* buffer = (uint8_t*)malloc(mem_info.RegionSize);

			//read the memory
			SIZE_T read_count = 0;
			uint8_t try_counter = 0;
			do {
				if (!ReadProcessMemory(global->process, mem_info.BaseAddress, buffer, mem_info.RegionSize, &read_count))
					read_count = 0;
			} while (read_count == 0 && try_counter++ < 0x20);

			//loop through the buffer as long as possible
			bool done = false;
			while (!done)
			{
				if (index >= global->scanSize)
					done = true;
				else if (global->scanAddr[index] + global->selected->size <= (uint64_t)mem_info.BaseAddress + read_count)
				{
					bool keep_value = false;
					if (test_function == 0)
					{
						if (global->selected->validate(&buffer[global->scanAddr[index] - (uint64_t)mem_info.BaseAddress]))
							keep_value = true;
					}
					else if (test_function(&buffer[global->scanAddr[index] - (uint64_t)mem_info.BaseAddress], (uint8_t*)(comp == 0 ? (void*)((uint64_t)global->scanValue + index * global->selected->size) : comp)))
						keep_value = true;

					//copy the data
					if (keep_value)
					{
						global->scanAddr[index - offset] = global->scanAddr[index];
						memcpy((void*)((uint64_t)global->scanValue + (index - offset) * global->selected->size), &buffer[global->scanAddr[index] - (uint64_t)mem_info.BaseAddress], global->selected->size);
					}
					else
						offset++;
					index++;
				}
				else
					done = true;

				//print the percentage-values
				if ((uint64_t)((((float)index * 100.0f) / (float)global->scanSize) + 0.5f) != percent_counter)
				{
					percent_counter = (uint64_t)((((float)index * 100.0f) / (float)global->scanSize) + 0.5f);
					cout << dec << percent_counter << "%" << endl;
				}
			}

			//free the buffer
			free(buffer);
		}

		//adjust the next-address
		nextaddress = (uint64_t)mem_info.BaseAddress + mem_info.RegionSize;
	}

	//remove the rest of the addreses
	offset += (global->scanSize - index);

	//resize the arrays
	if (offset > 0)
	{
		global->scanSize -= offset;
		if (global->scanSize == 0)
		{
			free(global->scanAddr);
			free(global->scanValue);
		}
		else
		{
			global->scanAddr = (uint64_t*)realloc(global->scanAddr, global->scanSize * sizeof(uint64_t));
			global->scanValue = (uint8_t*)realloc(global->scanValue, global->scanSize * global->selected->size);
		}
	}
}
void scanByAddress(MenuGlobal* global, void* comp, operationtype operation)
{
	//update the operation-pointer
	bool(*test_function)(uint8_t*, uint8_t*) = 0;
	switch (operation)
	{
	case operationtype::validate:
		test_function = 0;
		break;
	case operationtype::equal:
		test_function = global->selected->test_equal;
		break;
	case operationtype::unequal:
		test_function = global->selected->test_unequal;
		break;
	case operationtype::less:
		test_function = global->selected->test_less;
		break;
	case operationtype::less_equal:
		test_function = global->selected->test_less_equal;
		break;
	case operationtype::greater_equal:
		test_function = global->selected->test_greater_equal;
		break;
	case operationtype::greater:
		test_function = global->selected->test_greater;
		break;
	}

	//allocate a buffer for the values
	uint8_t* buffer = (uint8_t*)malloc(global->selected->size);
	SIZE_T readsize = 0;

	//loop through the addresses
	uint64_t offset = 0;
	uint64_t percent_counter = 0;
	for (uint64_t i = 0; i < global->scanSize; i++)
	{
		//try to read the value
		if (!ReadProcessMemory(global->process, (void*)global->scanAddr[i], buffer, global->selected->size, &readsize))
			readsize = 0;

		//check if the size is right
		if (readsize != global->selected->size)
			offset++;
		else
		{
			//test the values
			if (test_function == 0)
			{
				if (global->selected->validate(buffer))
				{
					global->scanAddr[i - offset] = global->scanAddr[i];
					memcpy((void*)((uint64_t)global->scanValue + (i - offset) * global->selected->size), buffer, global->selected->size);
				}
				else
					offset++;
			}
			else if (test_function(buffer, (uint8_t*)(comp == 0 ? (void*)((uint64_t)global->scanValue + i * global->selected->size) : comp)))
			{
				global->scanAddr[i - offset] = global->scanAddr[i];
				memcpy((void*)((uint64_t)global->scanValue + (i - offset) * global->selected->size), buffer, global->selected->size);
			}
			else
				offset++;
		}
	}
	free(buffer);

	//resize the arrays
	if (offset > 0)
	{
		global->scanSize -= offset;
		if (global->scanSize == 0)
		{
			free(global->scanAddr);
			free(global->scanValue);
		}
		else
		{
			global->scanAddr = (uint64_t*)realloc(global->scanAddr, global->scanSize * sizeof(uint64_t));
			global->scanValue = (uint8_t*)realloc(global->scanValue, global->scanSize * global->selected->size);
		}
	}
}
const char* scanData(MenuGlobal* global, operationtype operation, bool self)
{
	//read the value to compare against
	uint8_t* comp = (uint8_t*)malloc(global->selected->size);
	memset(comp, 0, global->selected->size);
	if (operation != operationtype::validate && !self)
		if (!global->selected->readinput(comp)) {
			free(comp);
			return "invalid input!";
		}

	//call the appropriate function
	if (global->scanSize < 0x8000)
		scanByAddress(global, (self ? 0 : comp), operation);
	else
		scanByBatch(global, (self ? 0 : comp), operation);
	free(comp);
	return "scan successful!";
}
ostream& printIndex(ostream& stream, uint64_t index)
{
	if (index == 0)
		stream << "0000000";
	else
	{
		uint64_t maxindex = 1000000;
		while (index < maxindex)
		{
			stream << '0';
			maxindex /= 10;
		}
		stream << dec << index;
	}
	return stream;
}
DWORD __stdcall MenuGlobalThread(void* ptr)
{
	//variables
	MenuGlobal* global = (MenuGlobal*)ptr;

	//enter the main-loop
	while (true) {
		//lock the mutex
		WaitForSingleObject(global->mutex, INFINITE);

		//check if overwriting is enabled
		if (global->overwrite && global->process != INVALID_HANDLE_VALUE) {
			//iterate through the variables and overwrite them
			for (uint64_t i = 0; i < global->entries.size(); i++) {
				if (global->entries.at(i).overwrite) {
					WriteProcessMemory(global->process, (void*)global->entries.at(i).address, global->entries.at(i).writeBuffer, global->entries.at(i).datatype->size, 0);
				}
			}
		}

		//release the mutex
		ReleaseMutex(global->mutex);

		//yield cpu-time
		Sleep(1);
	}

	return 0;
}



//implementation of the functions - menuroot
MenuPage menuroot::acquire()
{
	//create the new object
	MenuPage menu = MenuPage();

	//set the attributes used to describe the menu
	menu.id = menuroot::menuID;
	menu.title = menuroot::title;

	//set the functions
	menu.load = 0;
	menu.unload = 0;
	menu.update = 0;
	menu.menu = menuroot::menu;
	menu.eval = menuroot::eval;

	//return the object
	return menu;
}
MenuPageMenu menuroot::menu(MenuStruct* menuStruct)
{
	//extract the global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();

	//update the menu-mode
	if (global->process != INVALID_HANDLE_VALUE && global->moduleOffset == (uint64_t)-1)
		global->memoryMode = MemoryMode::memAll;

	//setup the page-menu
	MenuPageMenu pageMenu = DefaultMenu(global, menuID, false);

	//add the menu-entries
	if (menuStruct->getUseCtrl())
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 18, "input: default"));
	else
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 18, "input: Control + Num"));
	if (!menuStruct->isLoaded(menuprocess::menuID))
	{
		if (global->process == INVALID_HANDLE_VALUE)
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 1, "process: open"));
		else
		{
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 1, "process: change"));
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 2, "process: close"));
		}
	}
	if (!menuStruct->isLoaded(menudatatype::menuID))
	{
		if (global->selected == 0)
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 3, "datatype: select"));
		else
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 3, "datatype: change"));
	}
	if (!menuStruct->isLoaded(menudump::menuID) && global->process != INVALID_HANDLE_VALUE)
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 5, "view memory"));
	if (!menuStruct->isLoaded(menuentries::menuID))
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 4, "saved entries"));
	if (global->overwrite)
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 7, "overwriting: disable"));
	else
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 7, "overwriting: enable"));
	if (global->moduleOffset != (uint64_t)-1) {
		if (global->memoryMode == MemoryMode::memAll)
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 6, "memory-mode: static"));
		else if (global->memoryMode == MemoryMode::memStatic)
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 6, "memory-mode: volatile"));
		else
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 6, "memory-mode: all"));
	}
	if (global->process != INVALID_HANDLE_VALUE && global->selected != 0)
	{
		if (global->scanSize == 0)
		{
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 10, "new scan: unknown"));
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 11, "new scan: equal"));
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 12, "new scan: unequal"));
			if (!global->selected->restricted)
			{
				pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 13, "new scan: less"));
				pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 14, "new scan: less equal"));
				pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 15, "new scan: greater equal"));
				pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 16, "new scan: greater"));
			}
		}
		else {
			if (!menuStruct->isLoaded(menuprint::menuID) && global->process != INVALID_HANDLE_VALUE)
				pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 9, "scan: print"));
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 8, "scan: restart"));
			if (!menuStruct->isLoaded(menuscan::menuID))
				pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 17, "scan: manage"));
		}
	}
	return pageMenu;
}
MenuPageEval menuroot::eval(MenuStruct* menuStruct, uint64_t type)
{
	//extract the global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();

	//setup the default-pageeval
	MenuPageEval pageEval = MenuPageEval("", MenuTraverse::stay, 0);
	if (DefaultEval(global, type, &pageEval))
		return pageEval;

	//handle the type
	switch (type)
	{
	case DefaultMenu::custom + 1:
		return MenuPageEval("", MenuTraverse::push, menuprocess::menuID);
	case DefaultMenu::custom + 2:
		WaitForSingleObject(global->mutex, INFINITE);
		CloseHandle(global->process);
		global->process = INVALID_HANDLE_VALUE;
		ReleaseMutex(global->mutex);
		return pageEval;
	case DefaultMenu::custom + 3:
		if (global->scanSize > 0)
			return MenuPageEval("", MenuTraverse::push, menudatatypeverify::menuID);
		return MenuPageEval("", MenuTraverse::push, menudatatype::menuID);
	case DefaultMenu::custom + 4:
		return MenuPageEval("", MenuTraverse::push, menuentries::menuID);
	case DefaultMenu::custom + 5:
		return MenuPageEval("", MenuTraverse::push, menudump::menuID);
	case DefaultMenu::custom + 6:
		if (global->memoryMode == MemoryMode::memAll)
			global->memoryMode = MemoryMode::memStatic;
		else if (global->memoryMode == MemoryMode::memStatic)
			global->memoryMode = MemoryMode::memVolatile;
		else
			global->memoryMode = MemoryMode::memAll;
		return pageEval;
	case DefaultMenu::custom + 7:
		WaitForSingleObject(global->mutex, INFINITE);
		global->overwrite = !global->overwrite;
		ReleaseMutex(global->mutex);
		return pageEval;
	case DefaultMenu::custom + 8:
		return MenuPageEval("", MenuTraverse::push, menurestartverify::menuID);
	case DefaultMenu::custom + 9:
		return MenuPageEval("", MenuTraverse::push, menuprint::menuID);
	case DefaultMenu::custom + 10:  //unknown-scan
		pageEval.message = initialScan(global, operationtype::validate);
		if (global->scanSize > 0) {
			pageEval.traverse = MenuTraverse::push;
			pageEval.menuId = menuscan::menuID;
		}
		return pageEval;
	case DefaultMenu::custom + 11:  //equal-scan
		pageEval.message = initialScan(global, operationtype::equal);
		if (global->scanSize > 0) {
			pageEval.traverse = MenuTraverse::push;
			pageEval.menuId = menuscan::menuID;
		}
		return pageEval;
	case DefaultMenu::custom + 12:  //unequal-scan
		pageEval.message = initialScan(global, operationtype::unequal);
		if (global->scanSize > 0) {
			pageEval.traverse = MenuTraverse::push;
			pageEval.menuId = menuscan::menuID;
		}
		return pageEval;
	case DefaultMenu::custom + 13:  //less-scan
		pageEval.message = initialScan(global, operationtype::less);
		if (global->scanSize > 0) {
			pageEval.traverse = MenuTraverse::push;
			pageEval.menuId = menuscan::menuID;
		}
		return pageEval;
	case DefaultMenu::custom + 14:  //less-equal-scan
		pageEval.message = initialScan(global, operationtype::less_equal);
		if (global->scanSize > 0) {
			pageEval.traverse = MenuTraverse::push;
			pageEval.menuId = menuscan::menuID;
		}
		return pageEval;
	case DefaultMenu::custom + 15:  //greater-equal-scan
		pageEval.message = initialScan(global, operationtype::greater_equal);
		if (global->scanSize > 0) {
			pageEval.traverse = MenuTraverse::push;
			pageEval.menuId = menuscan::menuID;
		}
		return pageEval;
	case DefaultMenu::custom + 16:  //greater-scan
		pageEval.message = initialScan(global, operationtype::greater);
		if (global->scanSize > 0) {
			pageEval.traverse = MenuTraverse::push;
			pageEval.menuId = menuscan::menuID;
		}
		return pageEval;
	case DefaultMenu::custom + 17:  //manage scan
		pageEval.menuId = menuscan::menuID;
		pageEval.traverse = MenuTraverse::push;
		return pageEval;
	case DefaultMenu::custom + 18:
		menuStruct->setUseCtrl(!menuStruct->getUseCtrl());
		return pageEval;
	}
	return pageEval;
}
const char* menuroot::initialScan(MenuGlobal* global, operationtype operation)
{
	//read the value to compare against
	uint8_t* comp = (uint8_t*)malloc(global->selected->size);
	memset(comp, 0, global->selected->size);
	if (operation != operationtype::validate)
		if (!global->selected->readinput(comp))
			return "invalid input!";

	//allocate the array
	global->scanSize = 0x8000;
	global->scanAddr = (uint64_t*)malloc(sizeof(uint64_t) * global->scanSize);
	global->scanValue = (uint8_t*)malloc(global->selected->size * global->scanSize);

	//update the operation-pointer
	bool(*test_function)(uint8_t*, uint8_t*) = 0;
	switch (operation)
	{
	case operationtype::validate:
		test_function = 0;
		break;
	case operationtype::equal:
		test_function = global->selected->test_equal;
		break;
	case operationtype::unequal:
		test_function = global->selected->test_unequal;
		break;
	case operationtype::less:
		test_function = global->selected->test_less;
		break;
	case operationtype::less_equal:
		test_function = global->selected->test_less_equal;
		break;
	case operationtype::greater_equal:
		test_function = global->selected->test_greater_equal;
		break;
	case operationtype::greater:
		test_function = global->selected->test_greater;
		break;
	}

	//loop through all of the batches and scan them
	uint64_t index = 0;
	MEMORY_BASIC_INFORMATION mem_info;
	uint64_t nextaddress = 0;
	while (VirtualQueryEx(global->process, (void*)nextaddress, &mem_info, sizeof(MEMORY_BASIC_INFORMATION))) {
		//adjust the next-address
		nextaddress = (uint64_t)mem_info.BaseAddress + mem_info.RegionSize;

		//validate the address
		if (global->memoryMode != MemoryMode::memAll) {
			if (global->memoryMode == MemoryMode::memStatic) {
				if ((uint64_t)mem_info.BaseAddress + (uint64_t)mem_info.RegionSize <= global->staticSectionStart || (uint64_t)mem_info.BaseAddress >= global->staticSectionEnd)
					continue;
			}
			else if (global->memoryMode == MemoryMode::memVolatile) {
				if ((uint64_t)mem_info.BaseAddress >= global->staticSectionStart && (uint64_t)mem_info.BaseAddress + (uint64_t)mem_info.RegionSize <= global->staticSectionEnd)
					continue;
			}
		}

		//validate the batch
		if ((mem_info.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE)) && (mem_info.State & MEM_COMMIT) && (mem_info.Protect & (PAGE_GUARD | PAGE_NOACCESS)) == 0)
		{
			//allocate a buffer to hold the batch
			uint8_t* buffer = (uint8_t*)malloc(mem_info.RegionSize);

			//read the memory
			SIZE_T read_count = 0;
			uint8_t try_counter = 0;
			do {
				if (!ReadProcessMemory(global->process, mem_info.BaseAddress, buffer, mem_info.RegionSize, &read_count))
					read_count = 0;
			} while (read_count == 0 && try_counter++ < 0x20);

			//loop through the memory
			for (uint64_t i = ((uint64_t)mem_info.BaseAddress % global->selected->addr_alignment > 0) ? (global->selected->addr_alignment - ((uint64_t)mem_info.BaseAddress % global->selected->addr_alignment)) : 0; i < read_count; i += global->selected->addr_alignment)
			{
				//validate the address
				if (global->memoryMode != MemoryMode::memAll) {
					if (global->memoryMode == MemoryMode::memStatic) {
						if ((uint64_t)mem_info.BaseAddress + i < global->staticSectionStart || (uint64_t)mem_info.BaseAddress + i >= global->staticSectionEnd)
							continue;
					}
					else if (global->memoryMode == MemoryMode::memVolatile) {
						if ((uint64_t)mem_info.BaseAddress + i >= global->staticSectionStart && (uint64_t)mem_info.BaseAddress + i < global->staticSectionEnd)
							continue;
					}
				}

				//check if there is still enough space
				bool copy_value = false;
				if (i + global->selected->size <= read_count)
				{
					//validate the value
					if (test_function == 0)
					{
						if (global->selected->validate(&buffer[i]))
							copy_value = true;
					}
					else if (test_function(&buffer[i], comp))
						copy_value = true;

					//copy the value
					if (copy_value)
					{
						//check if the buffer has to be resized
						if (index == global->scanSize)
						{
							global->scanSize <<= 1;
							global->scanAddr = (uint64_t*)realloc(global->scanAddr, global->scanSize * sizeof(uint64_t));
							global->scanValue = (uint8_t*)realloc(global->scanValue, global->scanSize * global->selected->size);
						}

						//copy the value
						global->scanAddr[index] = (uint64_t)mem_info.BaseAddress + i;
						memcpy((void*)((uint64_t)global->scanValue + index * global->selected->size), &buffer[i], global->selected->size);
						index++;
					}
				}
			}

			//free the buffer
			free(buffer);
		}

		//output the address
		cout << mem_info.BaseAddress << endl;
	}

	//free the value to compare against
	free(comp);

	//resize the arrays
	if (global->scanSize > index)
	{
		global->scanSize = index;
		if (global->scanSize == 0)
		{
			free(global->scanAddr);
			free(global->scanValue);
		}
		else
		{
			global->scanAddr = (uint64_t*)realloc(global->scanAddr, global->scanSize * sizeof(uint64_t));
			global->scanValue = (uint8_t*)realloc(global->scanValue, global->scanSize * global->selected->size);
		}
	}
	return "scan successful!";
}



//implementation of the functions - menuexitverify
MenuPage menuexitverify::acquire()
{
	//create the new object
	MenuPage menu = MenuPage();

	//set the attributes used to describe the menu
	menu.id = menuexitverify::menuID;
	menu.title = menuexitverify::title;

	//set the functions
	menu.load = 0;
	menu.unload = 0;
	menu.update = 0;
	menu.menu = menuexitverify::menu;
	menu.eval = menuexitverify::eval;

	//return the object
	return menu;
}
MenuPageMenu menuexitverify::menu(MenuStruct* menuStruct)
{
	//extract the global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();

	//setup the page-menu
	MenuPageMenu pageMenu = MenuPageMenu(false, "Are you sure you want to exit the program?");

	//add the menu-entries
	pageMenu.entries.push_back(MenuEntry(DefaultMenu::returnMenu, "no"));
	pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom, "yes"));
	return pageMenu;
}
MenuPageEval menuexitverify::eval(MenuStruct* menuStruct, uint64_t type)
{
	if (type == DefaultMenu::returnMenu)
		return MenuPageEval("", MenuTraverse::pop, 0);
	return MenuPageEval("", MenuTraverse::exit, 0);
}



//implementation of the functions - menuprocess
MenuPage menuprocess::acquire()
{
	//create the new object
	MenuPage menu = MenuPage();

	//set the attributes used to describe the menu
	menu.id = menuprocess::menuID;
	menu.title = menuprocess::title;

	//set the functions
	menu.load = menuprocess::load;
	menu.unload = menuprocess::unload;
	menu.update = 0;
	menu.menu = menuprocess::menu;
	menu.eval = menuprocess::eval;

	//return the object
	return menu;
}
void menuprocess::load(MenuStruct* menuStruct)
{
	//allocate the local-data
	vector<HWND>* arr = (vector<HWND>*)malloc(sizeof(vector<HWND>));
	memset(arr, 0, sizeof(vector<HWND>));
	*arr = vector<HWND>();

	//set the local-data
	menuStruct->setLocalData(menuprocess::menuID, (uint8_t*)arr);
}
void menuprocess::unload(MenuStruct* menuStruct)
{
	//free the local data
	vector<HWND>* local = (vector<HWND>*)menuStruct->getLocalData(menuprocess::menuID);
	menuStruct->setLocalData(menuprocess::menuID, 0);
	local->~vector();
	free(local);
}
MenuPageMenu menuprocess::menu(MenuStruct* menuStruct)
{
	//extract the local & global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();
	vector<HWND>* local = (vector<HWND>*)menuStruct->getLocalData(menuprocess::menuID);

	//setup the page-menu
	MenuPageMenu pageMenu = DefaultMenu(global, menuID, false);

	//enumerate the windows
	local->clear();
	EnumWindows(menuprocess::EnumWindowsProc, (LPARAM)local);

	//add the menu-entries
	pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom, "refresh"));

	//add all of the windows
	char buffer[80];
	for (uint64_t i = 0; i < local->size(); i++)
	{
		if (GetWindowTextA(local->at(i), buffer, 80) == 0)
		{
			local->erase(local->begin() + i);
			i--;
		}
		else
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 1 + i, "process: " + string(buffer)));
	}
	return pageMenu;
}
MenuPageEval menuprocess::eval(MenuStruct* menuStruct, uint64_t type)
{
	//extract the local & global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();
	vector<HWND>* local = (vector<HWND>*)menuStruct->getLocalData(menuprocess::menuID);

	//setup the default-pageeval
	MenuPageEval pageEval = MenuPageEval("", MenuTraverse::stay, 0);
	if (DefaultEval(global, type, &pageEval))
		return pageEval;

	//handle the type
	if (type == DefaultMenu::custom)
		return pageEval;

	//lock the mutex
	WaitForSingleObject(global->mutex, INFINITE);

	//extract the process-name
	type -= 1 + DefaultMenu::custom;
	char buffer[80];
	if (GetWindowTextA(local->at(type), buffer, 80) == 0)
	{
		pageEval.message = "failed to extract the name of the process!";
		ReleaseMutex(global->mutex);
		return pageEval;
	}

	//try to open the process
	DWORD id = 0;
	GetWindowThreadProcessId(local->at(type), &id);
	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, false, id);
	if (process == INVALID_HANDLE_VALUE)
	{
		pageEval.message = "failed to open the process!";
		ReleaseMutex(global->mutex);
		return pageEval;
	}

	//close the old process
	if (global->process != INVALID_HANDLE_VALUE)
		CloseHandle(global->process);
	global->process = process;

	//release the mutex
	ReleaseMutex(global->mutex);

	//set the name
	global->processName = string(buffer);

	//clear the variables
	uint64_t batch_count = 0;
	uint64_t total_memory = 0;

	//loop through the memory of the process and extract the memory
	uint64_t nextaddress = 0;
	MEMORY_BASIC_INFORMATION mem_info;
	while (VirtualQueryEx(global->process, (void*)nextaddress, (PMEMORY_BASIC_INFORMATION)&mem_info, sizeof(MEMORY_BASIC_INFORMATION)) != 0)
	{
		//check if the memory is writeable as readonly is not of any interest
		if ((mem_info.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE)) && (mem_info.State & MEM_COMMIT) && (mem_info.Protect & (PAGE_GUARD | PAGE_NOACCESS)) == 0)
		{
			batch_count++;
			total_memory += mem_info.RegionSize;
		}

		//adjust the nextaddress
		nextaddress = (uint64_t)mem_info.BaseAddress + mem_info.RegionSize;
	}

	//add the data to the message
	stringstream sstream;
	sstream << "process successfully opened!" << endl;
	sstream << dec << "total-batches: " << batch_count << endl;
	sstream << dec << "total-memory : " << total_memory << endl;
	pageEval.traverse = MenuTraverse::root;
	global->moduleOffset = (uint64_t)-1;

	//get the module-offset
	HMODULE modOffset;
	DWORD maxSize;
	if (EnumProcessModules(global->process, &modOffset, sizeof(HMODULE), &maxSize) == 0) {
		sstream << "static-memory: none (failed to obtain module-offset)";
		pageEval.message = sstream.str();
		return pageEval;
	}
	sstream << "module-offset: 0x" << (void*)modOffset << endl;
	sstream << "static-memory: ";
	global->staticSectionStart = (uint64_t)modOffset;
	global->moduleOffset = (uint64_t)modOffset;

	//find the file and open it (to extract the static addresses)
	char nameBuffer[1024];
	memset(nameBuffer, 0, 1024);
	if (GetModuleFileNameExA(global->process, 0, nameBuffer, 1024) == 0) {
		sstream << "none (failed to obtain module-path)";
		global->moduleOffset = (uint64_t)-1;
		pageEval.message = sstream.str();
		return pageEval;
	}

	//open the file
	HANDLE fileHandle = CreateFileA(nameBuffer, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	if (fileHandle == INVALID_HANDLE_VALUE) {
		sstream << "none (failed to open module-file)";
		global->moduleOffset = (uint64_t)-1;
		pageEval.message = sstream.str();
		return pageEval;
	}

	//extract the offset to the dos-header
	IMAGE_DOS_HEADER dosHeader;
	if (ReadFile(fileHandle, &dosHeader, sizeof(IMAGE_DOS_HEADER), 0, 0) == 0) {
		CloseHandle(fileHandle);
		sstream << "none (failed to read DOS-Header)";
		global->moduleOffset = (uint64_t)-1;
		pageEval.message = sstream.str();
		return pageEval;
	}

	//extract the nt-header
	IMAGE_NT_HEADERS32 imageHeader;
	if (SetFilePointer(fileHandle, dosHeader.e_lfanew, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
		CloseHandle(fileHandle);
		sstream << "none (failed to read PE-Header)";
		global->moduleOffset = (uint64_t)-1;
		pageEval.message = sstream.str();
		return pageEval;
	}
	if (ReadFile(fileHandle, &imageHeader, sizeof(IMAGE_NT_HEADERS32), 0, 0) == 0) {
		CloseHandle(fileHandle);
		sstream << "none (failed to read PE-Header)";
		global->moduleOffset = (uint64_t)-1;
		pageEval.message = sstream.str();
		return pageEval;
	}

	//check if its a 32 or 64-bit file
	if (imageHeader.OptionalHeader.Magic == 0x10b) {
		//extract the section-headers (and look for the .data-header)
		IMAGE_SECTION_HEADER sectionHeader;
		if (imageHeader.FileHeader.NumberOfSections == 0) {
			CloseHandle(fileHandle);
			sstream << "none (.data section not found [x86])";
			global->moduleOffset = (uint64_t)-1;
			pageEval.message = sstream.str();
			return pageEval;
		}
		for (uint64_t i = 0; i < imageHeader.FileHeader.NumberOfSections; i++) {
			//read the header
			if (ReadFile(fileHandle, &sectionHeader, sizeof(IMAGE_SECTION_HEADER), 0, 0) == 0) {
				CloseHandle(fileHandle);
				sstream << "none (failed to read SECTION-Header [x86])";
				global->moduleOffset = (uint64_t)-1;
				pageEval.message = sstream.str();
				return pageEval;
			}

			//check if its the .data-header
			if (memcmp(sectionHeader.Name, ".data", 6) == 0) {
				CloseHandle(fileHandle);
				global->staticSectionStart += sectionHeader.VirtualAddress;
				global->staticSectionEnd = sectionHeader.Misc.VirtualSize + global->staticSectionStart;
				break;
			}
			else if (i + 1 == imageHeader.FileHeader.NumberOfSections) {
				//setup the final string
				CloseHandle(fileHandle);
				sstream << "none (.data section not found [x86])";
				global->moduleOffset = (uint64_t)-1;
				pageEval.message = sstream.str();
				return pageEval;
			}
		}
	}
	else {
		//extract the 64nt-header
		IMAGE_NT_HEADERS64 imageHeader64;
		if (SetFilePointer(fileHandle, dosHeader.e_lfanew, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
			CloseHandle(fileHandle);
			sstream << "none (failed to read PE-Header [x86_64])";
			global->moduleOffset = (uint64_t)-1;
			pageEval.message = sstream.str();
			return pageEval;
		}
		if (ReadFile(fileHandle, &imageHeader64, sizeof(IMAGE_NT_HEADERS64), 0, 0) == 0) {
			CloseHandle(fileHandle);
			sstream << "none (failed to read PE-Header [x86_64])";
			global->moduleOffset = (uint64_t)-1;
			pageEval.message = sstream.str();
			return pageEval;
		}

		//extract the section-headers (and look for the .data-header)
		IMAGE_SECTION_HEADER sectionHeader;
		if (imageHeader64.FileHeader.NumberOfSections == 0) {
			CloseHandle(fileHandle);
			sstream << "none (.data section not found [x86_64])";
			global->moduleOffset = (uint64_t)-1;
			pageEval.message = sstream.str();
			return pageEval;
		}
		for (uint64_t i = 0; i < imageHeader64.FileHeader.NumberOfSections; i++) {
			//read the header
			if (ReadFile(fileHandle, &sectionHeader, sizeof(IMAGE_SECTION_HEADER), 0, 0) == 0) {
				CloseHandle(fileHandle);
				sstream << "none (failed to read SECTION-Header [x86_64])";
				global->moduleOffset = (uint64_t)-1;
				pageEval.message = sstream.str();
				return pageEval;
			}

			//check if its the .data-header
			if (memcmp(sectionHeader.Name, ".data", 6) == 0) {
				CloseHandle(fileHandle);
				global->staticSectionStart += sectionHeader.VirtualAddress;
				global->staticSectionEnd = sectionHeader.Misc.VirtualSize + global->staticSectionStart;
				break;
			}
			else if (i + 1 == imageHeader64.FileHeader.NumberOfSections) {
				//setup the final string
				CloseHandle(fileHandle);
				sstream << "none (.data section not found [x86_64])";
				global->moduleOffset = (uint64_t)-1;
				pageEval.message = sstream.str();
				return pageEval;
			}
		}
	}

	//add the output-result
	sstream << hex << "0x" << (void*)global->staticSectionStart << " - 0x" << (void*)global->staticSectionEnd;
	pageEval.message = sstream.str();
	return pageEval;
}
BOOL _stdcall menuprocess::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	char Text[8];
	if (IsWindowVisible(hwnd))
	{
		GetWindowTextA(hwnd, Text, 8);
		if (Text[0] != '\0')
			((vector<HWND>*)lParam)->push_back(hwnd);
	}
	return TRUE;
}



//implementation of the functions - menudatatype
MenuPage menudatatype::acquire()
{
	//create the new object
	MenuPage menu = MenuPage();

	//set the attributes used to describe the menu
	menu.id = menudatatype::menuID;
	menu.title = menudatatype::title;

	//set the functions
	menu.load = 0;
	menu.unload = 0;
	menu.update = 0;
	menu.menu = menudatatype::menu;
	menu.eval = menudatatype::eval;

	//return the object
	return menu;
}
MenuPageMenu menudatatype::menu(MenuStruct* menuStruct)
{
	//extract the local & global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();

	//setup the page-menu
	MenuPageMenu pageMenu = DefaultMenu(global, menuID, false);

	//add all of the datatypes
	for (uint64_t i = 0; i < global->types.size(); i++)
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + i, "datatype: " + string(global->types.at(i).name)));
	return pageMenu;
}
MenuPageEval menudatatype::eval(MenuStruct* menuStruct, uint64_t type)
{
	//extract the local & global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();

	//setup the default-pageeval
	MenuPageEval pageEval = MenuPageEval("", MenuTraverse::pop, 0);
	if (DefaultEval(global, type, &pageEval))
		return pageEval;

	//check if the type has changed
	type -= DefaultMenu::custom;
	if (global->selected == &global->types.at(type))
	{
		pageEval.message = "datatype unchanged!";
		return pageEval;
	}

	//clear the current scan
	if (global->scanSize > 0)
	{
		free(global->scanAddr);
		free(global->scanValue);
		global->scanSize = 0;
	}

	//set the datatype
	global->selected = &global->types.at(type);
	pageEval.message = "datatype changed!";
	pageEval.traverse = MenuTraverse::root;
	return pageEval;
}



//implementation of the functions - menudatatypeverify
MenuPage menudatatypeverify::acquire()
{
	//create the new object
	MenuPage menu = MenuPage();

	//set the attributes used to describe the menu
	menu.id = menudatatypeverify::menuID;
	menu.title = menudatatypeverify::title;

	//set the functions
	menu.load = 0;
	menu.unload = 0;
	menu.update = 0;
	menu.menu = menudatatypeverify::menu;
	menu.eval = menudatatypeverify::eval;

	//return the object
	return menu;
}
MenuPageMenu menudatatypeverify::menu(MenuStruct* menuStruct)
{
	//extract the global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();

	//setup the page-menu
	MenuPageMenu pageMenu = MenuPageMenu(false, "Are you sure you want to change the datatype?\nThe current scan will be lost.");

	//add the menu-entries
	pageMenu.entries.push_back(MenuEntry(DefaultMenu::returnMenu, "no"));
	pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom, "yes"));
	return pageMenu;
}
MenuPageEval menudatatypeverify::eval(MenuStruct* menuStruct, uint64_t type)
{
	if (type == DefaultMenu::returnMenu)
		return MenuPageEval("", MenuTraverse::pop, 0);
	return MenuPageEval("", MenuTraverse::move, menudatatype::menuID);
}



//implementation of the functions - menurestartverify
MenuPage menurestartverify::acquire()
{
	//create the new object
	MenuPage menu = MenuPage();

	//set the attributes used to describe the menu
	menu.id = menurestartverify::menuID;
	menu.title = menurestartverify::title;

	//set the functions
	menu.load = 0;
	menu.unload = 0;
	menu.update = 0;
	menu.menu = menurestartverify::menu;
	menu.eval = menurestartverify::eval;

	//return the object
	return menu;
}
MenuPageMenu menurestartverify::menu(MenuStruct* menuStruct)
{
	//extract the global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();

	//setup the page-menu
	MenuPageMenu pageMenu = MenuPageMenu(false, "Are you sure you want to restart the scan?\nThe current scan will be lost.");

	//add the menu-entries
	pageMenu.entries.push_back(MenuEntry(DefaultMenu::returnMenu, "no"));
	pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom, "yes"));
	return pageMenu;
}
MenuPageEval menurestartverify::eval(MenuStruct* menuStruct, uint64_t type)
{
	//check if a restart has been selected
	if (type == DefaultMenu::returnMenu)
		return MenuPageEval("", MenuTraverse::pop, 0);

	//extract the global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();

	//clear the scan
	free(global->scanAddr);
	free(global->scanValue);
	global->scanSize = 0;
	return MenuPageEval("", MenuTraverse::root, 0);
}



//implementation of the functions - menuscan
MenuPage menuscan::acquire()
{
	//create the new object
	MenuPage menu = MenuPage();

	//set the attributes used to describe the menu
	menu.id = menuscan::menuID;
	menu.title = menuscan::title;

	//set the functions
	menu.load = 0;
	menu.unload = 0;
	menu.update = 0;
	menu.menu = menuscan::menu;
	menu.eval = menuscan::eval;

	//return the object
	return menu;
}
MenuPageMenu menuscan::menu(MenuStruct* menuStruct)
{
	//extract the global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();

	//setup the page-menu
	MenuPageMenu pageMenu = DefaultMenu(global, menuID, false);

	//add the menu-entries
	if (global->scanSize > 0)
	{
		if (!menuStruct->isLoaded(menuconstant::menuID))
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 1, "filter out fluctuations"));
		if (!menuStruct->isLoaded(menuprint::menuID))
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 2, "scan: print"));
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 3, "scan: restart"));
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 4, "scan: refresh"));
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 5, "scan: unchanged"));
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 6, "scan: changed"));
		if (!global->selected->restricted) {
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 7, "scan: decreased"));
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 8, "scan: increased"));
		}
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 9, "scan: equal to"));
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 10, "scan: unequal to"));
		if (!global->selected->restricted)
		{
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 11, "scan: less than"));
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 12, "scan: less-equal than"));
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 13, "scan: greater-equal than"));
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 14, "scan: greater than"));
		}
	}
	else
		pageMenu.MenuString.append("\n\nThe current scan is empty!");

	//return the page-structure
	return pageMenu;
}
MenuPageEval menuscan::eval(MenuStruct* menuStruct, uint64_t type)
{
	//extract the global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();

	//setup the default-pageeval
	MenuPageEval pageEval = MenuPageEval("", MenuTraverse::stay, 0);
	if (DefaultEval(global, type, &pageEval))
		return pageEval;

	//handle the type
	switch (type)
	{
	case DefaultMenu::custom + 1:  //constant-filter
		pageEval.menuId = menuconstant::menuID;
		pageEval.traverse = MenuTraverse::push;
		break;
	case DefaultMenu::custom + 2:  //print-scan
		pageEval.menuId = menuprint::menuID;
		pageEval.traverse = MenuTraverse::push;
		break;
	case DefaultMenu::custom + 3:  //restart-scan
		pageEval.menuId = menurestartverify::menuID;
		pageEval.traverse = MenuTraverse::push;
		break;
	case DefaultMenu::custom + 4:  //refresh
		pageEval.message = scanData(global, operationtype::validate, false);
		break;
	case DefaultMenu::custom + 5:  //unchanged
		pageEval.message = scanData(global, operationtype::equal, true);
		break;
	case DefaultMenu::custom + 6:  //changed
		pageEval.message = scanData(global, operationtype::unequal, true);
		break;
	case DefaultMenu::custom + 7:  //decreased
		pageEval.message = scanData(global, operationtype::less, true);
		break;
	case DefaultMenu::custom + 8:  //increased
		pageEval.message = scanData(global, operationtype::greater, true);
		break;

	case DefaultMenu::custom + 9:  //equal
		pageEval.message = scanData(global, operationtype::equal, false);
		break;
	case DefaultMenu::custom + 10:  //unequal
		pageEval.message = scanData(global, operationtype::unequal, false);
		break;
	case DefaultMenu::custom + 11:  //less
		pageEval.message = scanData(global, operationtype::less, false);
		break;
	case DefaultMenu::custom + 12:  //less-equal
		pageEval.message = scanData(global, operationtype::less_equal, false);
		break;
	case DefaultMenu::custom + 13:  //greater-equal
		pageEval.message = scanData(global, operationtype::greater_equal, false);
		break;
	default:  //greater
		pageEval.message = scanData(global, operationtype::greater, false);
		break;
	}
	return pageEval;
}



//implementation of the functions - menuconstant
MenuPage menuconstant::acquire()
{
	//create the new object
	MenuPage menu = MenuPage();

	//set the attributes used to describe the menu
	menu.id = menuconstant::menuID;
	menu.title = menuconstant::title;

	//set the functions
	menu.load = 0;
	menu.unload = 0;
	menu.update = menuconstant::update;
	menu.menu = menuconstant::menu;
	menu.eval = menuconstant::eval;

	//return the object
	return menu;
}
bool menuconstant::update(MenuStruct* menuStruct)
{
	//extract the global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();

	//copy the values
	uint64_t bufferSize = global->scanSize;
	void* buffer = (void*)malloc(global->scanSize * global->selected->size);
	memcpy(buffer, global->scanValue, global->scanSize * global->selected->size);

	//refresh the data
	scanData(global, operationtype::validate, false);
	if (bufferSize != global->scanSize) {
		free(buffer);
		return false;
	}

	//check if any of the data have changed
	uint64_t offset = 0;
	for (uint64_t i = 0; i < bufferSize; i++)
	{
		if (global->selected->test_equal((uint8_t*)((uint64_t)buffer + i * global->selected->size), (uint8_t*)((uint64_t)global->scanValue + i * global->selected->size)))
		{
			global->scanAddr[i - offset] = global->scanAddr[i];
			memcpy((void*)((uint64_t)global->scanValue + (i - offset) * global->selected->size), (void*)((uint64_t)global->scanValue + i * global->selected->size), global->selected->size);
		}
		else
			offset++;
	}
	free(buffer);
	if (offset > 0)
	{
		global->scanSize -= offset;
		if (global->scanSize == 0)
		{
			free(global->scanAddr);
			free(global->scanValue);
		}
		else
		{
			global->scanAddr = (uint64_t*)realloc(global->scanAddr, global->scanSize * sizeof(uint64_t));
			global->scanValue = (void*)realloc(global->scanValue, global->scanSize * global->selected->size);
		}
		return false;
	}
	return true;
}
MenuPageMenu menuconstant::menu(MenuStruct* menuStruct)
{
	//extract the global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();

	//setup the page-menu
	MenuPageMenu pageMenu = DefaultMenu(global, menuID, false);

	//return the input-type
	if (global->scanSize == 0)
		pageMenu.MenuString.append("\n\nThe current scan is empty!");
	else
		pageMenu.update = true;
	return pageMenu;
}
MenuPageEval menuconstant::eval(MenuStruct* menuStruct, uint64_t type)
{
	//setup the page-menu
	MenuPageEval pageEval = MenuPageEval("", MenuTraverse::pop, 0);
	DefaultEval((MenuGlobal*)menuStruct->getGlobalData(), type, &pageEval);
	return pageEval;
}



//implementation of the functions - menuprint
MenuPage menuprint::acquire()
{
	//create the new object
	MenuPage menu = MenuPage();

	//set the attributes used to describe the menu
	menu.id = menuprint::menuID;
	menu.title = menuprint::title;

	//set the functions
	menu.load = 0;
	menu.unload = 0;
	menu.update = menuprint::update;
	menu.menu = menuprint::menu;
	menu.eval = menuprint::eval;

	//return the object
	return menu;
}
bool menuprint::update(MenuStruct* menuStruct)
{
	//extract the global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();

	//check for a forced update-screen
	if (global->updateScreen) {
		if (GetTickCount64() - (uint64_t)menuStruct->getLocalData(menuprint::menuID) >= 125)
			return false;
	}

	//copy the values
	uint64_t bufferSize = global->scanSize;
	void* buffer = (void*)malloc(global->scanSize * global->selected->size);
	memcpy(buffer, global->scanValue, global->scanSize * global->selected->size);

	//refresh the data
	scanData(global, operationtype::validate, false);
	if (bufferSize != global->scanSize) {
		free(buffer);
		if (GetTickCount64() - (uint64_t)menuStruct->getLocalData(menuprint::menuID) >= 125)
			return false;
		global->updateScreen = true;
		return true;
	}

	//check if any of the data have changed
	for (uint64_t i = 0; i < bufferSize; i++)
	{
		if (!global->selected->test_equal((uint8_t*)((uint64_t)buffer + i * global->selected->size), (uint8_t*)((uint64_t)global->scanValue + i * global->selected->size)))
		{
			free(buffer);
			if (GetTickCount64() - (uint64_t)menuStruct->getLocalData(menuprint::menuID) >= 125)
				return false;
			global->updateScreen = true;
			return true;
		}
	}
	free(buffer);
	return true;
}
MenuPageMenu menuprint::menu(MenuStruct* menuStruct)
{
	//extract the global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();

	//create the page-object
	MenuPageMenu pageMenu = DefaultMenu(global, menuID, true);
	uint64_t prevSize = global->scanSize;
	if (global->scanSize > 0) {
		//refresh the data
		scanData(global, operationtype::validate, false);

		//write the current scan to the output
		stringstream sstr;
		for (uint64_t i = 0; i < global->scanSize; i++) {
			//check if the address is static
			if (global->moduleOffset != (uint64_t)-1) {
				if (global->scanAddr[i] >= global->staticSectionStart && global->scanAddr[i] < global->staticSectionEnd)
					printIndex(sstr, i) << " - " << "[module+0x" << hex << (void*)(global->scanAddr[i] - global->moduleOffset) << "] -> ";
				else
					printIndex(sstr, i) << " - " << " [base+0x" << hex << (void*)global->scanAddr[i] << "]  -> ";
			}
			else
				printIndex(sstr, i) << " - [0x" << hex << (void*)global->scanAddr[i] << "] -> ";
			sstr << global->selected->tostring((uint8_t*)((uint64_t)global->scanValue + i * global->selected->size)) << endl;
		}
		pageMenu.MenuString.append("\n\n").append(sstr.str());

		//add the menu-entries
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 1, "remove"));
		if (!menuStruct->isLoaded(menudump::menuID))
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 4, "view in memory"));
		if (!menuStruct->isLoaded(menuentries::menuID))
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 6, "saved entries"));
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 2, "save"));
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 3, "save all"));
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 5, "change value"));
	}
	else {
		pageMenu.MenuString.append("\n\nThe current scan is empty!");
		pageMenu.update = false;
	}

	//set the last print and the update-screen-flag
	menuStruct->setLocalData(menuprint::menuID, (void*)GetTickCount64());
	if (prevSize == global->scanSize)
		global->updateScreen = false;
	else
		global->updateScreen = true;

	//return the page-structure
	return pageMenu;
}
MenuPageEval menuprint::eval(MenuStruct* menuStruct, uint64_t type)
{
	//extract the global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();

	//setup the default-pageeval
	MenuPageEval pageEval = MenuPageEval("", MenuTraverse::stay, 0);
	if (DefaultEval(global, type, &pageEval))
		return pageEval;

	//handle the type
	if (type == DefaultMenu::custom + 1) {
		//receive the index
		uint64_t index = 0;
		if (!menuStruct->getNumber("enter the index", &index))
		{
			pageEval.message = "invalid index!";
			return pageEval;
		}
		if (index >= global->scanSize)
		{
			pageEval.message = "index out of range!";
			return pageEval;
		}

		//remove the entry
		for (uint64_t i = index + 1; i < global->scanSize; i++) {
			global->scanAddr[i - 1] = global->scanAddr[i];
			memcpy((void*)((uint64_t)global->scanValue + (i - 1) * global->selected->size), (void*)((uint64_t)global->scanValue + i * global->selected->size), global->selected->size);
		}

		//resize the array
		global->scanSize--;
		if (global->scanSize == 0)
		{
			free(global->scanAddr);
			free(global->scanValue);
		}
		else
		{
			global->scanAddr = (uint64_t*)realloc(global->scanAddr, global->scanSize * sizeof(uint64_t));
			global->scanValue = (void*)realloc(global->scanValue, global->scanSize * global->selected->size);
		}
		pageEval.message = "entry successfully removed!";
	}
	else if (type == DefaultMenu::custom + 2) {
		//receive the index
		uint64_t index = 0;
		if (!menuStruct->getNumber("enter the index", &index))
		{
			pageEval.message = "invalid index!";
			return pageEval;
		}
		if (index >= global->scanSize)
		{
			pageEval.message = "index out of range!";
			return pageEval;
		}

		//receive the name
		cout << "enter the name: ";
		string name;
		getline(cin, name);

		//create the entry
		SaveEntry save;
		save.address = global->scanAddr[index];
		save.buffer = malloc(global->selected->size);
		save.writeBuffer = malloc(global->selected->size);
		memcpy(save.buffer, (void*)((uint64_t)global->scanValue + index * global->selected->size), global->selected->size);
		memcpy(save.writeBuffer, save.buffer, global->selected->size);
		save.datatype = global->selected;
		save.name = name;
		save.overwrite = false;

		//add the entry
		WaitForSingleObject(global->mutex, INFINITE);
		global->entries.push_back(save);
		pageEval.message = "entry successfully added to list!";
		ReleaseMutex(global->mutex);
	}
	else if (type == DefaultMenu::custom + 3) {
		//receive the name
		cout << "enter the name: ";
		string name;
		getline(cin, name);

		//loop through the entries
		WaitForSingleObject(global->mutex, INFINITE);
		for (uint64_t i = 0; i < global->scanSize; i++)
		{
			//create the entries
			SaveEntry save;
			save.address = global->scanAddr[i];
			save.buffer = malloc(global->selected->size);
			save.writeBuffer = malloc(global->selected->size);
			memcpy(save.buffer, (void*)((uint64_t)global->scanValue + i * global->selected->size), global->selected->size);
			memcpy(save.writeBuffer, save.buffer, global->selected->size);
			save.datatype = global->selected;
			save.overwrite = false;

			//create the name
			stringstream sstr;
			sstr << name << "_" << dec << i;
			save.name = sstr.str();

			//add the entry
			global->entries.push_back(save);
		}
		ReleaseMutex(global->mutex);
		pageEval.message = "entries successfully added to list!";
	}
	else if (type == DefaultMenu::custom + 4) {
		//receive the index
		uint64_t index = 0;
		if (!menuStruct->getNumber("enter the index", &index)) {
			pageEval.message = "invalid index!";
			return pageEval;
		}
		if (index >= global->scanSize) {
			pageEval.message = "index out of range!";
			return pageEval;
		}

		//set the address
		global->dumpAddress = global->scanAddr[index];
		if (global->dumpAddress > 64)
			global->dumpAddress -= 64;
		else
			global->dumpAddress = 0;
		if (global->selected->size == global->selected->addr_alignment) {
			bool isMultiple = false;
			for (uint64_t i = 1; i <= 64; i <<= 1) {
				if (global->selected->size == i) {
					isMultiple = true;
					break;
				}
			}
			if (isMultiple) {
				global->dumpType = global->selected;
				pageEval.message = "address & type of memory-view set!";
			}
			else {
				global->dumpType = 0;
				pageEval.message = "address of memory-view set (type is not suited)!";
			}
		}
		else
			pageEval.message = "address of memory-view set (type is not suited)!";
		pageEval.menuId = menudump::menuID;
		pageEval.traverse = MenuTraverse::push;
	}
	else if (type == DefaultMenu::custom + 5) {
		//receive the index
		uint64_t index = 0;
		if (!menuStruct->getNumber("enter the index", &index)) {
			pageEval.message = "invalid index!";
			return pageEval;
		}
		if (index >= global->scanSize) {
			pageEval.message = "index out of range!";
			return pageEval;
		}

		//receive the new value to write
		uint8_t* buffer = (uint8_t*)malloc(global->selected->size);
		if (!global->selected->readinput(buffer))
			pageEval.message = "invalid value!";
		else {
			//try to write the value to memory
			if (WriteProcessMemory(global->process, (void*)global->scanAddr[index], buffer, global->selected->size, 0))
				pageEval.message = "value written!";
			else
				pageEval.message = "writing failed!";
		}
	}
	else {
		pageEval.traverse = MenuTraverse::push;
		pageEval.menuId = menuentries::menuID;
	}

	return pageEval;
}



//implementation of the functions - menuentries
MenuPage menuentries::acquire()
{
	//create the new object
	MenuPage menu = MenuPage();

	//set the attributes used to describe the menu
	menu.id = menuentries::menuID;
	menu.title = menuentries::title;

	//set the functions
	menu.load = 0;
	menu.unload = 0;
	menu.update = menuentries::update;
	menu.menu = menuentries::menu;
	menu.eval = menuentries::eval;

	//return the object
	return menu;
}
bool menuentries::update(MenuStruct* menuStruct)
{
	//extract the global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();

	//check for a forced update-screen
	if (global->updateScreen) {
		if (GetTickCount64() - (uint64_t)menuStruct->getLocalData(menuentries::menuID) >= 125)
			return false;
	}

	//iterate through the entries and overwrite them/check for changes
	for (uint64_t i = 0; i < global->entries.size(); i++) {
		SaveEntry* entry = &global->entries.at(i);
		if (entry->overwrite) {
			if (global->overwrite)
				WriteProcessMemory(global->process, (void*)entry->address, entry->buffer, entry->datatype->size, 0);
		}
		else if (!global->updateScreen) {
			uint8_t* buffer = (uint8_t*)malloc(entry->datatype->size);
			if (ReadProcessMemory(global->process, (void*)entry->address, buffer, entry->datatype->size, 0)) {
				if (!entry->datatype->test_equal(buffer, (uint8_t*)entry->buffer))
					global->updateScreen = true;
			}
			free(buffer);
		}
	}

	//check if any of the data have changed
	if (GetTickCount64() - (uint64_t)menuStruct->getLocalData(menuentries::menuID) >= 250 && global->updateScreen)
		return false;
	return true;
}
MenuPageMenu menuentries::menu(MenuStruct* menuStruct)
{
	//extract the global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();

	//create the page-object
	MenuPageMenu pageMenu = DefaultMenu(global, menuID, true);
	if (global->process == INVALID_HANDLE_VALUE || global->entries.size() == 0)
		pageMenu.update = false;

	//create the strings
	if (global->entries.size() > 0) {
		//refresh the values and write the current scan to the output
		stringstream sstr;
		for (uint64_t i = 0; i < global->entries.size(); i++) {
			//read the current value
			if (global->process != INVALID_HANDLE_VALUE) {
				if (!global->entries.at(i).overwrite || !global->overwrite)
					ReadProcessMemory(global->process, (void*)global->entries.at(i).address, global->entries.at(i).buffer, global->entries.at(i).datatype->size, 0);
				else
					memcpy(global->entries.at(i).buffer, global->entries.at(i).writeBuffer, global->entries.at(i).datatype->size);
			}

			//add the entry to the result-string
			if (global->entries.at(i).overwrite)
				printIndex(sstr, i) << " - w - ";
			else
				printIndex(sstr, i) << " - r - ";

			//write the name to the string
			for (uint32_t j = 0; j < 16; j++) {
				if (j < global->entries.at(i).name.size())
					sstr << global->entries.at(i).name.at(j);
				else
					sstr << ' ';
			}

			//write the address to the string
			if (global->moduleOffset != (uint64_t)-1) {
				if (global->entries.at(i).address >= global->staticSectionStart && global->entries.at(i).address < global->staticSectionEnd)
					sstr << " [module+0x" << hex << (void*)(global->entries.at(i).address - global->moduleOffset) << "] -> ";
				else
					sstr << "  [base+0x" << hex << (void*)global->entries.at(i).address << "]  -> ";
			}
			else
				sstr << " [0x" << hex << (void*)global->scanAddr[i] << "] -> ";

			//write the address and the value to the string
			sstr << global->entries.at(i).datatype->tostring((uint8_t*)global->entries.at(i).buffer) << endl;
		}
		pageMenu.MenuString.append("\n\n").append(sstr.str());

		//add the menu-entries
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 1, "remove"));
		if (!menuStruct->isLoaded(menudump::menuID))
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 5, "view in memory"));
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 3, "change value"));
		if (global->overwrite)
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 7, "overwriting: disable"));
		else
			pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 7, "overwriting: enable"));
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 2, "toggle overwrite"));
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 4, "rename"));
	}
	else
		pageMenu.MenuString.append("\n\nThere are no entries!");
	pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 6, "add value by address"));

	//set the last print and the update-screen-flag
	menuStruct->setLocalData(menuentries::menuID, (void*)GetTickCount64());
	global->updateScreen = false;

	//return the page-structure
	return pageMenu;
}
MenuPageEval menuentries::eval(MenuStruct* menuStruct, uint64_t type)
{
	//extract the global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();

	//setup the default-pageeval
	MenuPageEval pageEval = MenuPageEval("", MenuTraverse::stay, 0);
	if (DefaultEval(global, type, &pageEval))
		return pageEval;

	//handle the type
	if (type == DefaultMenu::custom + 1) {
		//receive the index
		uint64_t index = 0;
		if (!menuStruct->getNumber("enter the index", &index))
		{
			pageEval.message = "invalid index!";
			return pageEval;
		}
		if (index >= global->entries.size())
		{
			pageEval.message = "index out of range!";
			return pageEval;
		}

		//remove the entry
		WaitForSingleObject(global->mutex, INFINITE);
		global->entries.erase(global->entries.begin() + index);
		pageEval.message = "entry removed!";
		ReleaseMutex(global->mutex);
	}
	else if (type == DefaultMenu::custom + 2) {
		//receive the index
		uint64_t index = 0;
		if (!menuStruct->getNumber("enter the index", &index))
		{
			pageEval.message = "invalid index!";
			return pageEval;
		}
		if (index >= global->entries.size())
		{
			pageEval.message = "index out of range!";
			return pageEval;
		}

		//check if overwriting is being enabled or disabled
		WaitForSingleObject(global->mutex, INFINITE);
		if (global->entries.at(index).overwrite) {
			global->entries.at(index).overwrite = false;
			pageEval.message = "entry overwriting disabled!";
		}
		else {
			memcpy(global->entries.at(index).writeBuffer, global->entries.at(index).buffer, global->entries.at(index).datatype->size);
			global->entries.at(index).overwrite = true;
			pageEval.message = "entry overwriting enabled!";
		}
		ReleaseMutex(global->mutex);
	}
	else if (type == DefaultMenu::custom + 3) {
		//receive the index
		uint64_t index = 0;
		if (!menuStruct->getNumber("enter the index", &index))
		{
			pageEval.message = "invalid index!";
			return pageEval;
		}
		if (index >= global->entries.size())
		{
			pageEval.message = "index out of range!";
			return pageEval;
		}

		//lock the mutex
		WaitForSingleObject(global->mutex, INFINITE);

		//receive the new value to write
		if (!global->entries.at(index).datatype->readinput((uint8_t*)global->entries.at(index).writeBuffer))
			pageEval.message = "invalid value!";
		else {
			//try to write the value to memory
			if (global->overwrite) {
				memcpy(global->entries.at(index).buffer, global->entries.at(index).writeBuffer, global->entries.at(index).datatype->size);
				pageEval.message = "value written!";
			}
			else if (WriteProcessMemory(global->process, (void*)global->entries.at(index).address, global->entries.at(index).writeBuffer, global->entries.at(index).datatype->size, 0)) {
				memcpy(global->entries.at(index).buffer, global->entries.at(index).writeBuffer, global->entries.at(index).datatype->size);
				pageEval.message = "value written!";
			}
			else
				pageEval.message = "writing failed!";
		}
		ReleaseMutex(global->mutex);
	}
	else if (type == DefaultMenu::custom + 4) {
		//receive the index
		uint64_t index = 0;
		if (!menuStruct->getNumber("enter the index", &index))
		{
			pageEval.message = "invalid index!";
			return pageEval;
		}
		if (index >= global->entries.size())
		{
			pageEval.message = "index out of range!";
			return pageEval;
		}

		//receive the name
		cout << "enter the new name: ";
		getline(cin, global->entries.at(index).name);
		pageEval.message = "entry renamed!";
	}
	else if (type == DefaultMenu::custom + 5) {
		//receive the index
		uint64_t index = 0;
		if (!menuStruct->getNumber("enter the index", &index)) {
			pageEval.message = "invalid index!";
			return pageEval;
		}
		if (index >= global->entries.size())
		{
			pageEval.message = "index out of range!";
			return pageEval;
		}

		//set the address
		global->dumpAddress = global->entries.at(index).address;
		if (global->dumpAddress > 64)
			global->dumpAddress -= 64;
		else
			global->dumpAddress = 0;
		if (global->entries.at(index).datatype->size == global->entries.at(index).datatype->addr_alignment) {
			bool isMultiple = false;
			for (uint64_t i = 1; i <= 64; i <<= 1) {
				if (global->entries.at(index).datatype->size == i) {
					isMultiple = true;
					break;
				}
			}
			if (isMultiple) {
				global->dumpType = global->entries.at(index).datatype;
				pageEval.message = "address & type of memory-view set!";
			}
			else {
				global->dumpType = 0;
				pageEval.message = "address of memory-view set (type is not suited)!";
			}
		}
		else
			pageEval.message = "address of memory-view set (type is not suited)!";
		pageEval.menuId = menudump::menuID;
		pageEval.traverse = MenuTraverse::push;
	}
	else if (type == DefaultMenu::custom + 6) {
		//receive the address
		uint64_t address = 0;
		if (!menuStruct->getNumberHex("enter the address", &address))
		{
			pageEval.message = "invalid number!";
			return pageEval;
		}

		//receive the datatype
		for (uint64_t i = 0; i < global->types.size(); i++) {
			cout << "[" << (global->types.size() >= 10 && i < 10 ? "0" : "") << dec << i << "] - " << global->types.at(i).name << endl;
		}
		uint64_t index;
		if (!menuStruct->getNumber("select the datatype", &index))
		{
			pageEval.message = "invalid index!";
			return pageEval;
		}
		else if (index >= global->types.size()) {
			pageEval.message = "index out of range!";
			return pageEval;
		}

		//receive the name
		cout << "enter the name: ";
		string name;
		getline(cin, name);

		//create the entry
		SaveEntry save;
		save.address = address;
		save.buffer = malloc(global->types.at(index).size);
		save.writeBuffer = malloc(global->types.at(index).size);
		memset(save.buffer, 0, global->types.at(index).size);
		memset(save.writeBuffer, 0, global->types.at(index).size);
		save.datatype = &global->types.at(index);
		save.name = name;
		save.overwrite = false;

		//add the entry
		WaitForSingleObject(global->mutex, INFINITE);
		global->entries.push_back(save);
		pageEval.message = "entry successfully added to list!";
		ReleaseMutex(global->mutex);
	}
	else {
		WaitForSingleObject(global->mutex, INFINITE);
		global->overwrite = !global->overwrite;
		pageEval.message = "overwrite toggled!";
		ReleaseMutex(global->mutex);
	}
	return pageEval;
}



//implementation of the functions - menudump
MenuPage menudump::acquire()
{
	//create the new object
	MenuPage menu = MenuPage();

	//set the attributes used to describe the menu
	menu.id = menudump::menuID;
	menu.title = menudump::title;

	//set the functions
	menu.load = menudump::load;
	menu.unload = menudump::unload;
	menu.update = menudump::update;
	menu.menu = menudump::menu;
	menu.eval = menudump::eval;

	//return the object
	return menu;
}
void menudump::load(MenuStruct* menuStruct)
{
	//allocate the local-data
	DumpInfo* info = (DumpInfo*)malloc(sizeof(DumpInfo));
	memset(info, 0, sizeof(DumpInfo));

	//set the local-data
	menuStruct->setLocalData(menudump::menuID, (uint8_t*)info);
}
void menudump::unload(MenuStruct* menuStruct)
{
	//free the local data
	DumpInfo* info = (DumpInfo*)menuStruct->getLocalData(menudump::menuID);
	menuStruct->setLocalData(menudump::menuID, 0);
	free(info);
}
bool menudump::update(MenuStruct* menuStruct)
{
	//extract the local & global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();
	DumpInfo* local = (DumpInfo*)menuStruct->getLocalData(menudump::menuID);

	//check for a forced update-screen
	if (global->updateScreen) {
		if (GetTickCount64() - local->lastPrint >= 125)
			return false;
	}

	//copy the values
	uint8_t buffer[256];

	//read the memory and check if the data have changed
	if (ReadProcessMemory(global->process, (void*)global->dumpAddress, buffer, 256, 0)) {
		if (!local->scanValid) {
			local->scanValid = true;
			global->updateScreen = true;
			memcpy(local->buffer, buffer, 256);
		}
		else if (global->dumpType == 0) {
			if (memcmp(buffer, local->buffer, 256) != 0) {
				global->updateScreen = true;
				memcpy(local->buffer, buffer, 256);
			}
		}
		else {
			//check if any of the values have changed
			for (uint64_t i = 0; i < 256; i += global->dumpType->size) {
				if (!global->dumpType->test_equal(buffer + i, local->buffer + i)) {
					global->updateScreen = true;
					memcpy(local->buffer, buffer, 256);
					break;
				}
			}
		}
	}

	//check if any of the data have changed
	if (GetTickCount64() - local->lastPrint >= 250 && global->updateScreen)
		return false;
	return true;
}
MenuPageMenu menudump::menu(MenuStruct* menuStruct)
{
	//extract the local & global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();
	DumpInfo* local = (DumpInfo*)menuStruct->getLocalData(menudump::menuID);

	//create the page-object
	MenuPageMenu pageMenu = DefaultMenu(global, menuID, true);

	//add the default-entries
	pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 8, "select datatype"));
	if (global->dumpType != 0)
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 9, "change value"));
	pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 2, "decrease address"));
	pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 3, "previous chunk (-256)"));
	pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 4, "previous chunk (-128)"));
	pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 1, "set address"));
	pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 5, "next chunk (+128)"));
	pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 6, "next chunk (+256)"));
	pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 7, "increase address"));
	if (global->moduleOffset != (uint64_t)-1)
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 11, "dump static memory to file"));

	//align the address to the currently selected type
	if (global->dumpType != 0)
		global->dumpAddress -= (global->dumpAddress % global->dumpType->size);
	else
		global->dumpAddress -= (global->dumpAddress % 8);

	//read the memory
	if (!ReadProcessMemory(global->process, (void*)global->dumpAddress, local->buffer, 256, 0))
		local->scanValid = false;
	else
		local->scanValid = true;

	//create the header-string
	stringstream sstr;
	if (global->moduleOffset != (uint64_t)-1)
		sstr << " ";
	sstr << "address: 0x" << hex << (void*)global->dumpAddress << endl;
	pageMenu.MenuString.append("\n\n").append(sstr.str());

	//add the entries
	if (global->dumpType == 0) {

		//create the hex-dump
		sstr.str("");
		if (global->moduleOffset != (uint64_t)-1)
			sstr << " ";

		//print the header
		if (global->dumpTypeSpecific == 0)
			sstr << "hex | 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f |" << endl;
		else if (global->dumpTypeSpecific == 1)
			sstr << "hex | 00       04       08       0c       |" << endl;
		else
			sstr << "hex | 00               08               |" << endl;

		//print the separation-line
		if (global->moduleOffset != (uint64_t)-1)
			sstr << "-";
		sstr << "----+-------------------------------------------------+-----------------" << endl;

		//print the separate rows
		for (uint64_t i = 0; i < 16; i++) {
			//create the static-data
			if (global->moduleOffset != (uint64_t)-1) {
				if ((i << 4) + global->dumpAddress >= global->staticSectionStart && (i << 4) + global->dumpAddress < global->staticSectionEnd)
					sstr << "s";
				else
					sstr << "v";
			}

			//create the initial string
			sstr << ' ' << dec << (char)(i < 10 ? ('0' + i) : ('a' + (i - 10))) << '0' << " | ";

			//add the hex-data
			if (global->dumpTypeSpecific == 0) {
				for (uint64_t j = 0; j < 16; j++) {
					if (local->scanValid) {
						if (local->buffer[(i << 4) + j] >= 0xa0)
							sstr << (char)('a' + ((local->buffer[(i << 4) + j] >> 4) - 10));
						else
							sstr << (char)('0' + (local->buffer[(i << 4) + j] >> 4));
						if ((local->buffer[(i << 4) + j] & 0x0f) >= 0x0a)
							sstr << (char)('a' + ((local->buffer[(i << 4) + j] & 0x0f) - 10));
						else
							sstr << (char)('0' + (local->buffer[(i << 4) + j] & 0x0f));
						sstr << ' ';
					}
					else
						sstr << "?? ";
				}
			}
			else if (global->dumpTypeSpecific == 1) {
				for (uint64_t j = 0; j < 16; j += 4) {
					if (local->scanValid) {
						for (uint64_t k = 0; k < 4; k++) {
							if (local->buffer[(i << 4) + j + (3 - k)] >= 0xa0)
								sstr << (char)('a' + ((local->buffer[(i << 4) + j + (3 - k)] >> 4) - 10));
							else
								sstr << (char)('0' + (local->buffer[(i << 4) + j + (3 - k)] >> 4));
							if ((local->buffer[(i << 4) + j + (3 - k)] & 0x0f) >= 0x0a)
								sstr << (char)('a' + ((local->buffer[(i << 4) + j + (3 - k)] & 0x0f) - 10));
							else
								sstr << (char)('0' + (local->buffer[(i << 4) + j + (3 - k)] & 0x0f));
						}
						sstr << ' ';
					}
					else
						sstr << "???????? ";
				}
			}
			else {
				for (uint64_t j = 0; j < 16; j += 8) {
					if (local->scanValid) {
						for (uint64_t k = 0; k < 8; k++) {
							if (local->buffer[(i << 4) + j + (7 - k)] >= 0xa0)
								sstr << (char)('a' + ((local->buffer[(i << 4) + j + (7 - k)] >> 4) - 10));
							else
								sstr << (char)('0' + (local->buffer[(i << 4) + j + (7 - k)] >> 4));
							if ((local->buffer[(i << 4) + j + (7 - k)] & 0x0f) >= 0x0a)
								sstr << (char)('a' + ((local->buffer[(i << 4) + j + (7 - k)] & 0x0f) - 10));
							else
								sstr << (char)('0' + (local->buffer[(i << 4) + j + (7 - k)] & 0x0f));
						}
						sstr << ' ';
					}
					else
						sstr << "???????????????? ";
				}
			}
			sstr << "| ";

			//add the characters
			for (uint64_t j = 0; j < 16; j++) {
				if (local->buffer[(i << 4) + j] >= 0x20 && local->buffer[(i << 4) + j] < 0x7f)
					sstr << local->buffer[(i << 4) + j];
				else
					sstr << '.';
			}
			sstr << endl;
		}
		pageMenu.MenuString.append("\n").append(sstr.str());
	}
	else {
		//create the header
		stringstream sstr;
		uint64_t entriesPerLine = (8 / (global->dumpType->size > 8 ? 8 : global->dumpType->size));
		uint64_t charsPerEntry = 48 / entriesPerLine;
		string str = global->dumpType->name;
		if (str.size() > 10)
			str.resize(10);
		else {
			while (str.size() < 10)
				str.append(" ");
		}
		sstr << str << "|";
		for (uint64_t i = 0; i < entriesPerLine; i++) {
			sstr << "+" << dec << i * global->dumpType->size;
			while (sstr.str().size() < 9 + (i + 1) * (charsPerEntry + 1))
				sstr << " ";
			sstr << (i + 1 == entriesPerLine ? "\n" : "|");
		}
		sstr << "----------";
		for (uint64_t i = 0; i < (charsPerEntry + 1) * entriesPerLine; i++) {
			if (i % (charsPerEntry + 1) == 0)
				sstr << "+";
			else
				sstr << "-";
		}
		sstr << endl;

		//write the memory to the screen
		for (uint64_t i = 0; i < 256; i += entriesPerLine * global->dumpType->size) {
			//create the static-data
			uint64_t curSize = sstr.str().size();
			if (global->moduleOffset != (uint64_t)-1) {
				if (i + global->dumpAddress >= global->staticSectionStart && i + global->dumpAddress < global->staticSectionEnd)
					sstr << "s ";
				else
					sstr << "v ";
			}

			//add the row-header
			sstr << "+" << dec << i;
			while (sstr.str().size() - curSize < 10)
				sstr << " ";
			sstr << "|";

			//add the values
			for (uint64_t j = 0; j < entriesPerLine; j++) {
				str = "";

				//create the string
				if (!local->scanValid)
					str = "?";
				else if (!global->dumpType->validate(local->buffer + i + j * global->dumpType->size)) {
					str = "?";
				}
				else
					str = global->dumpType->tostring(local->buffer + i + j * global->dumpType->size);

				//clip the string and add it to the result
				if (str.size() > charsPerEntry)
					str.resize(charsPerEntry);
				else {
					while (str.size() < charsPerEntry)
						str.append(" ");
				}
				sstr << str << (j + 1 == entriesPerLine ? "\n" : " ");
			}
		}
		pageMenu.MenuString.append("\n").append(sstr.str());

		//add the menu
		pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 10, "add to entries"));
	}

	//set the last print and the update-screen-flag
	local->lastPrint = GetTickCount64();
	global->updateScreen = false;

	return pageMenu;
}
MenuPageEval menudump::eval(MenuStruct* menuStruct, uint64_t type)
{
	//extract the local & global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();
	DumpInfo* local = (DumpInfo*)menuStruct->getLocalData(menudump::menuID);

	//setup the default-pageeval
	MenuPageEval pageEval = MenuPageEval("", MenuTraverse::stay, 0);
	if (DefaultEval(global, type, &pageEval))
		return pageEval;

	//handle the type
	if (type == DefaultMenu::custom + 1) {
		uint64_t addr;
		if (!menuStruct->getNumberHex("enter a new address", &addr))
			pageEval.message = "invalid address!";
		else {
			global->dumpAddress = addr;
			pageEval.message = "address successfully changed!";
		}
	}
	else if (type == DefaultMenu::custom + 2) {
		uint64_t offset;
		if (!menuStruct->getNumber("enter the offset", &offset))
			pageEval.message = "invalid offset!";
		else {
			pageEval.message = "address successfully decreased!";
			global->dumpAddress -= offset;
		}
	}
	else if (type == DefaultMenu::custom + 3) {
		pageEval.message = "address successfully decreased!";
		global->dumpAddress -= 256;
	}
	else if (type == DefaultMenu::custom + 4) {
		pageEval.message = "address successfully decreased!";
		global->dumpAddress -= 128;
	}
	else if (type == DefaultMenu::custom + 5) {
		pageEval.message = "address successfully increased!";
		global->dumpAddress += 128;
	}
	else if (type == DefaultMenu::custom + 6) {
		pageEval.message = "address successfully increased!";
		global->dumpAddress += 256;
	}
	else if (type == DefaultMenu::custom + 7) {
		uint64_t offset;
		if (!menuStruct->getNumber("enter the offset", &offset))
			pageEval.message = "invalid offset!";
		else {
			pageEval.message = "address successfully increased!";
			global->dumpAddress += offset;
		}
	}
	else if (type == DefaultMenu::custom + 8) {
		pageEval.traverse = MenuTraverse::push;
		pageEval.menuId = menudumptype::menuID;
	}
	else if (type == DefaultMenu::custom + 9) {
		uint64_t offset;
		if (!menuStruct->getNumber("enter the offset in the table", &offset))
			pageEval.message = "invalid offset!";
		else {
			if (offset >= 256)
				pageEval.message = "offset out of range!";
			else {
				//align the index
				offset -= (offset % global->dumpType->size);

				//receive the new value to write
				uint8_t* buffer = (uint8_t*)malloc(global->dumpType->size);
				if (!global->dumpType->readinput(buffer))
					pageEval.message = "invalid value!";
				else {
					//try to write the value to memory
					if (WriteProcessMemory(global->process, (void*)(global->dumpAddress + offset), buffer, global->dumpType->size, 0))
						pageEval.message = "value written!";
					else
						pageEval.message = "writing failed!";
				}
			}
		}
	}
	else if (type == DefaultMenu::custom + 10) {
		uint64_t offset;
		if (!menuStruct->getNumber("enter the offset in the table", &offset))
			pageEval.message = "invalid offset!";
		else {
			if (offset >= 256)
				pageEval.message = "offset out of range!";
			else {
				//align the index
				offset -= (offset % global->dumpType->size);

				//receive the name
				cout << "enter the name: ";
				string name;
				getline(cin, name);

				//create the entry
				SaveEntry save;
				save.address = global->dumpAddress + offset;
				save.buffer = malloc(global->dumpType->size);
				memcpy(save.buffer, (void*)(local->buffer + offset), global->dumpType->size);
				save.datatype = global->dumpType;
				save.name = name;
				save.overwrite = false;

				//add the entry
				WaitForSingleObject(global->mutex, INFINITE);
				global->entries.push_back(save);
				pageEval.message = "entry successfully added to list!";
				ReleaseMutex(global->mutex);
			}
		}
	}
	else {
		//get the filepath
		cout << "enter the filepath: ";
		string path;
		getline(cin, path);
		if (path.size() == 0)
			pageEval.message = "invalid path!";
		else {
			//create and open the file
			ofstream file = ofstream(path, ios::out | ios::trunc);
			if (!file.is_open())
				pageEval.message = "failed to open the file!";
			else {

				//read the memory
				uint8_t* buffer = (uint8_t*)malloc(global->staticSectionEnd - global->staticSectionStart);
				if (ReadProcessMemory(global->process, (void*)global->staticSectionStart, buffer, global->staticSectionEnd - global->staticSectionStart, 0) == 0) {
					pageEval.message = "failed to read the memory!";
				}
				else {
					//print the data to the file
					uint64_t typeSize = (global->dumpType == 0 ? (global->dumpTypeSpecific == 0 ? 1 : (global->dumpTypeSpecific == 1 ? 4 : 8)) : global->dumpType->size);
					uint64_t limit = global->staticSectionEnd - global->staticSectionStart - typeSize;
					for (uint64_t i = 0; i <= limit; i += typeSize) {
						if ((i & 0x1f) == 0) {
							if (i)
								file << endl;
							file << "[module+0x" << (void*)(i + global->staticSectionStart - global->moduleOffset) << "] - ";
						}
						stringstream sstr;
						if (global->dumpType != 0) {
							if (!global->dumpType->validate(buffer + i))
								sstr << "?";
							else
								sstr << global->dumpType->tostring(buffer + i);
							if (sstr.str().size() > 12)
								sstr.str().resize(12);
							while (sstr.str().size() < 12)
								sstr << " ";
						}
						else {
							for (int8_t j = (int8_t)typeSize - 1; j >= 0; j--) {
								if (buffer[i + j] >= 0xa0)
									sstr << (char)('a' + (buffer[i + j] >> 4) - 10);
								else
									sstr << (char)('0' + (buffer[i + j] >> 4));
								if ((buffer[i + j] & 0x0f) >= 0x0a)
									sstr << (char)('a' + (buffer[i + j] & 0x0f) - 10);
								else
									sstr << (char)('0' + (buffer[i + j] & 0x0f));
							}
						}
						file << sstr.str() << " ";
					}
				}

				//clsoe the resources
				free(buffer);
				file.close();
				pageEval.message = "dump successfully written!";
			}
		}
	}

	return pageEval;
}



//implementation of the functions - menudumptype
MenuPage menudumptype::acquire()
{
	//create the new object
	MenuPage menu = MenuPage();

	//set the attributes used to describe the menu
	menu.id = menudumptype::menuID;
	menu.title = menudumptype::title;

	//set the functions
	menu.load = 0;
	menu.unload = 0;
	menu.update = 0;
	menu.menu = menudumptype::menu;
	menu.eval = menudumptype::eval;

	//return the object
	return menu;
}
MenuPageMenu menudumptype::menu(MenuStruct* menuStruct)
{
	//extract the local & global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();

	//create the page-object
	MenuPageMenu pageMenu = DefaultMenu(global, menuID, false);

	//add the default-entry
	pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 1, "hex-dump"));
	pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 2, "Ptr32"));
	pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 3, "Ptr64"));

	//add the datatypes
	for (uint64_t i = 0; i < global->types.size(); i++) {
		//check if the type is valid
		if (global->types.at(i).addr_alignment == global->types.at(i).size) {
			//check if the size is a multiple of two
			bool isMultiple = false;
			for (uint64_t j = 1; j <= 64; j <<= 1) {
				if (global->types.at(i).size == j) {
					isMultiple = true;
					break;
				}
			}

			//add the type
			if (isMultiple)
				pageMenu.entries.push_back(MenuEntry(DefaultMenu::custom + 4 + i, string("datatype: ") + global->types.at(i).name));
		}
	}

	return pageMenu;
}
MenuPageEval menudumptype::eval(MenuStruct* menuStruct, uint64_t type)
{
	//extract the local & global data
	MenuGlobal* global = (MenuGlobal*)menuStruct->getGlobalData();

	//setup the default-pageeval
	MenuPageEval pageEval = MenuPageEval("", MenuTraverse::pop, 0);
	if (DefaultEval(global, type, &pageEval))
		return pageEval;

	//handle the type
	if (type == DefaultMenu::custom + 1) {
		global->dumpType = 0;
		global->dumpTypeSpecific = 0;
	}
	else if (type == DefaultMenu::custom + 2) {
		global->dumpType = 0;
		global->dumpTypeSpecific = 1;
	}
	else if (type == DefaultMenu::custom + 3) {
		global->dumpType = 0;
		global->dumpTypeSpecific = 2;
	}
	else
		global->dumpType = &global->types.at(type - DefaultMenu::custom - 4);
	pageEval.message = "datatype successfully changed!";
	return pageEval;
}