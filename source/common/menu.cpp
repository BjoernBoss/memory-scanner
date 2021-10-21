/* SPDX-License-Identifier: BSD-3-Clause */
/* Copyright (c) 2021 Bjoern Boss Henrichsen */
#include "menu.h"

#include <types/int-types.h>
#include <types/float-types.h>
#include <types/extra-types.h>

#pragma comment(lib, "Winmm.lib")
#include <iostream>
#include <Psapi.h>
#include <fstream>
#include <sstream>
#include <iomanip>

/* implement the global menu instance */
MenuInstance::Entry::Entry() {
	buffer = 0;
	writeBuffer = 0;
	address = 0;
	datatype = 0;
	overwrite = false;
}
MenuInstance::MenuInstance() {
	memoryMode = MemoryMode::memStatic;
	process = INVALID_HANDLE_VALUE;
	overwrite = false;
	moduleOffset = (uint64_t)-1;
	staticSectionStart = 0;
	staticSectionEnd = 0;

	selected = 0;
	intermediate = 0;
	scanAddr = 0;
	scanValue = 0;
	scanSize = 0;
	dumpType = 0;
	dumpAddress = 0;
	dumpTypeSpecific = 0;

	std::memset(&mutex, 0, sizeof(CRITICAL_SECTION));
	thread = 0;
	threadState = 0;
}
void MenuInstance::init() {
	/* setup the requested timing precision */
	timeBeginPeriod(4);

	/* create the resources */
	threadState = 0;
	InitializeCriticalSection(&mutex);
	thread = CreateThread(0, 0, MenuInstance::globalThread, (void*)this, 0, 0);

	/* add all of the datatypes */
	types.push_back(new types::Int<uint8_t>());
	types.push_back(new types::Int<uint16_t>());
	types.push_back(new types::Int<uint32_t>());
	types.push_back(new types::Int<uint64_t>());
	types.push_back(new types::Int<int8_t>());
	types.push_back(new types::Int<int16_t>());
	types.push_back(new types::Int<int32_t>());
	types.push_back(new types::Int<int64_t>());
	types.push_back(new types::Float<float>());
	types.push_back(new types::Float<double>());
	types.push_back(new types::Bool());
	types.push_back(new types::String(false));
	types.push_back(new types::String(true));
	types.push_back(new types::Float2(0));
	types.push_back(new types::Float2(1));
	types.push_back(new types::Float3(0));
	types.push_back(new types::Float3(1));
	types.push_back(new types::Float3(2));

	/* add all of the menu pages */
	host()->add(RootPage::acquire());
	host()->add(ExitVerifyPage::acquire());
	host()->add(ProcessPage::acquire());
	host()->add(DatatypePage::acquire());
	host()->add(DatatypeVerifyPage::acquire());
	host()->add(RestartVerifyPage::acquire());
	host()->add(ScanPage::acquire());
	host()->add(ConstantPage::acquire());
	host()->add(PrintPage::acquire());
	host()->add(EntriesPage::acquire());
	host()->add(DumpPage::acquire());
	host()->add(DumptypePage::acquire());
}
void MenuInstance::teardown() {
	/* destroy the thread and the mutex */
	threadState = 1;
	while (threadState == 1)
		Sleep(1);
	CloseHandle(thread);
	DeleteCriticalSection(&mutex);

	/* reset the timing precision */
	timeEndPeriod(4);

	/* release all of the saved entries */
	for (size_t i = 0; i < entries.size(); i++) {
		free(entries[i].buffer);
		free(entries[i].writeBuffer);
	}

	/* release all other resources */
	if (process != INVALID_HANDLE_VALUE)
		CloseHandle(process);
	if (scanSize > 0) {
		free(scanValue);
		free(scanAddr);
	}

	/* release all of the datatypes */
	for (size_t i = 0; i < types.size(); i++)
		delete types[i];

	/* release the intermediate buffer */
	if (intermediate)
		free(intermediate);
}
const char* MenuInstance::root() {
	return RootPage::Id;
}
menu::Layout MenuInstance::layout(const char* current, bool update) {
	/* create the initial layout */
	menu::Layout layout(update, "");

	/* setup the default header */
	if (process != INVALID_HANDLE_VALUE)
		layout.header.append("current process: ").append(processName);
	else
		layout.header.append("current process: none");
	if (overwrite)
		layout.header.append("\noverwrite: enabled");
	else
		layout.header.append("\noverwrite: disabled");
	if (memoryMode == MemoryMode::memAll)
		layout.header.append("\nMemory: all");
	else if (memoryMode == MemoryMode::memStatic)
		layout.header.append("\nMemory: static");
	else if (memoryMode == MemoryMode::memVolatile)
		layout.header.append("\nMemory: volatile");
	if (moduleOffset != (uint64_t)-1) {
		std::stringstream ss;
		ss << std::hex << " [module: 0x" << (void*)moduleOffset << "]";
		layout.header.append(ss.str());
	}
	else
		layout.header.append(" [module: unknown]");
	if (selected != 0)
		layout.header.append(" (type: ").append(selected->name()).append(")");
	else
		layout.header.append(" (type: none)");
	if (scanSize > 0) {
		std::stringstream ss;
		ss << std::dec << scanSize;
		layout.header.append(" -> scan-size: ").append(ss.str());
	}
	else
		layout.header.append(" -> scan-size: 0");

	/* add the default menu entries */
	layout.add(Default::exit, "exit");
	if (std::strcmp(current, RootPage::Id) != 0) {
		layout.add(Default::returnMenu, "return");
		if (std::strcmp(current, ProcessPage::Id) != 0 && std::strcmp(current, DatatypePage::Id) != 0)
			layout.add(Default::rootMenu, "root menu");
	}
	return layout;
}
bool MenuInstance::eval(menu::EntryId selected, menu::Behavior* behavior) {
	/* handle the default types */
	switch (selected) {
	case Default::exit:
		behavior->traverse = menu::Traverse::push;
		behavior->target = ExitVerifyPage::Id;
		return true;
	case Default::returnMenu:
		behavior->traverse = menu::Traverse::pop;
		return true;
	case Default::rootMenu:
		behavior->traverse = menu::Traverse::root;
		return true;
	}
	return false;
}
MenuInstance::Entry& MenuInstance::entry(uint64_t address, Datatype* type, const uint8_t* buffer, std::string name) {
	/* ask the user for the name */
	while (name.empty()) {
		std::cout << "enter the name: ";
		std::getline(std::cin, name);
		if (name.empty())
			std::cout << "invalid name!" << std::endl;
	}

	/* populate the entry */
	MenuInstance::Entry save;
	save.address = address;
	save.buffer = (uint8_t*)malloc(type->size());
	save.writeBuffer = (uint8_t*)malloc(type->size());
	if (buffer) {
		std::memcpy(save.buffer, buffer, type->size());
		std::memcpy(save.writeBuffer, buffer, type->size());
	}
	else {
		std::memset(save.buffer, 0, type->size());
		std::memset(save.writeBuffer, 0, type->size());
	}
	save.datatype = type;
	save.name = name;
	save.overwrite = false;

	/* add the entry to the array */
	EnterCriticalSection(&mutex);
	entries.push_back(save);
	LeaveCriticalSection(&mutex);
	return entries.back();
}
const char* MenuInstance::queryIndex(const std::string& text, size_t& index, size_t limit) {
	index = 0;

	/* receive the index */
	uint64_t tmp = 0;
	if (!menu::Instance::host()->getNumber(text, tmp, false))
		return "invalid index!";
	if ((index = (size_t)tmp) >= limit)
		return "index out of range!";
	return 0;
}
const char* MenuInstance::scanInitial(Datatype::Operation operation) {
	/* read the value to compare against */
	if (operation != Datatype::Operation::validate) {
		if (!selected->readInput(intermediate, true))
			return "invalid input!";
	}

	/* allocate the array */
	scanSize = 0x8000;
	scanAddr = (uint64_t*)malloc(sizeof(uint64_t) * scanSize);
	scanValue = (uint8_t*)malloc(selected->size() * scanSize);

	/* loop through all of the batches and scan them */
	size_t index = 0;
	MEMORY_BASIC_INFORMATION memInfo;
	uint64_t current = 0;
	while (VirtualQueryEx(process, (void*)current, &memInfo, sizeof(MEMORY_BASIC_INFORMATION))) {
		/* advance the current address */
		current = (uint64_t)memInfo.BaseAddress + memInfo.RegionSize;

		/* validate the address */
		if (memoryMode != MenuInstance::MemoryMode::memAll) {
			if (memoryMode == MenuInstance::MemoryMode::memStatic) {
				if ((uint64_t)memInfo.BaseAddress + (uint64_t)memInfo.RegionSize <= staticSectionStart || (uint64_t)memInfo.BaseAddress >= staticSectionEnd)
					continue;
			}
			else if (memoryMode == MenuInstance::MemoryMode::memVolatile) {
				if ((uint64_t)memInfo.BaseAddress >= staticSectionStart && (uint64_t)memInfo.BaseAddress + (uint64_t)memInfo.RegionSize <= staticSectionEnd)
					continue;
			}
		}

		/* validate the batch */
		if ((memInfo.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE)) && (memInfo.State & MEM_COMMIT) && (memInfo.Protect & (PAGE_GUARD | PAGE_NOACCESS)) == 0) {
			/* allocate a buffer to hold the batch */
			uint8_t* buffer = (uint8_t*)malloc(memInfo.RegionSize);

			/* read the memory */
			SIZE_T bytesRead = 0;
			uint8_t try_counter = 0;
			do {
				if (ReadProcessMemory(process, memInfo.BaseAddress, buffer, memInfo.RegionSize, &bytesRead))
					break;
			} while (try_counter++ < 0x20);

			/* compute the aligned adresses */
			uint64_t alignedStart = (uint64_t)memInfo.BaseAddress;
			uint64_t alignedEnd = (uint64_t)memInfo.BaseAddress + bytesRead;
			if (uint64_t off = alignedStart % selected->align())
				alignedStart += selected->align() - off;
			alignedEnd -= alignedEnd % selected->align();

			/* loop through the memory */
			for (uint64_t i = alignedStart; i < alignedEnd; i += selected->align()) {
				const uint8_t* value = buffer + (i - (uint64_t)memInfo.BaseAddress);

				/* validate the address */
				if (memoryMode != MenuInstance::MemoryMode::memAll) {
					if (memoryMode == MenuInstance::MemoryMode::memStatic) {
						if (i < staticSectionStart || i >= staticSectionEnd)
							continue;
					}
					else if (memoryMode == MenuInstance::MemoryMode::memVolatile) {
						if (i >= staticSectionStart && i < staticSectionEnd)
							continue;
					}
				}

				/* validate the value */
				bool valid = false;
				if (operation == Datatype::Operation::validate)
					valid = selected->validate(value);
				else
					valid = selected->test(value, intermediate, operation);
				if (!valid)
					continue;

				/* check if the buffer has to be resized */
				if (index == scanSize) {
					scanSize <<= 1;
					scanAddr = (uint64_t*)realloc(scanAddr, scanSize * sizeof(uint64_t));
					scanValue = (uint8_t*)realloc(scanValue, scanSize * selected->size());
				}

				/* copy the value */
				scanAddr[index] = i;
				memcpy(scanValue + index * selected->size(), value, selected->size());
				index++;
			}

			/* free the buffer */
			free(buffer);
		}

		/* output the address */
		std::cout << memInfo.BaseAddress << std::endl;
	}

	/* resize the arrays */
	if (scanSize > index) {
		scanSize = index;
		if (scanSize == 0) {
			free(scanAddr);
			free(scanValue);
		}
		else {
			scanAddr = (uint64_t*)realloc(scanAddr, scanSize * sizeof(uint64_t));
			scanValue = (uint8_t*)realloc(scanValue, scanSize * selected->size());
		}
	}
	return "scan successful!";
}
void MenuInstance::scanByBatch(const uint8_t* comp, Datatype::Operation operation) {
	/* loop through all of the batches and scan them */
	size_t index = 0;
	size_t offset = 0;
	MEMORY_BASIC_INFORMATION memInfo;
	uint64_t current = 0;
	size_t percent = 0;
	while (VirtualQueryEx(process, (void*)current, &memInfo, sizeof(MEMORY_BASIC_INFORMATION))) {
		/* remove all of the rest of the addresses before this batch */
		while (index < scanSize) {
			if (scanAddr[index] >= (uint64_t)memInfo.BaseAddress)
				break;
			offset++;
			index++;
		}

		/* validate the current batch */
		if ((memInfo.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE)) && (memInfo.State & MEM_COMMIT) && (memInfo.Protect & (PAGE_GUARD | PAGE_NOACCESS)) == 0) {
			/* allocate a buffer to read the entire batch */
			uint8_t* buffer = (uint8_t*)malloc(memInfo.RegionSize);

			/* read the memory */
			SIZE_T read_count = 0;
			uint8_t try_counter = 0;
			do {
				if (!ReadProcessMemory(process, memInfo.BaseAddress, buffer, memInfo.RegionSize, &read_count))
					read_count = 0;
			} while (read_count == 0 && try_counter++ < 0x20);

			/* loop through the buffer as long as possible */
			while (index < scanSize) {
				/* check if the selected type fits into the rest of the current batch */
				if (scanAddr[index] + selected->size() > (uint64_t)memInfo.BaseAddress + read_count)
					break;
				const uint8_t* value = buffer + (scanAddr[index] - (uint64_t)memInfo.BaseAddress);

				/* check if the value satisfies the condition */
				bool valid = false;
				if (operation == Datatype::Operation::validate)
					valid = selected->validate(value);
				else
					valid = selected->test(value, comp == 0 ? scanValue + index * selected->size() : comp, operation);

				/* check if the value is considered valid and advance the current index */
				if (valid) {
					scanAddr[index - offset] = scanAddr[index];
					std::memcpy(scanValue + (index - offset) * selected->size(), value, selected->size());
				}
				else
					offset++;
				index++;

				/* print the percentage */
				float progress = ((float)index * 100.0f) / (float)scanSize;
				if (static_cast<uint64_t>(progress + 0.5f) != percent) {
					percent = static_cast<uint64_t>(progress + 0.5f);
					std::cout << std::dec << percent << '%' << std::endl;
				}
			}

			/* release the buffer */
			free(buffer);
		}

		/* advance the address */
		current = (uint64_t)memInfo.BaseAddress + memInfo.RegionSize;
	}

	/* remove all invalid values */
	offset += (scanSize - index);
	if (offset > 0) {
		if ((scanSize -= offset) == 0) {
			free(scanAddr);
			free(scanValue);
		}
		else {
			scanAddr = (uint64_t*)realloc(scanAddr, scanSize * sizeof(uint64_t));
			scanValue = (uint8_t*)realloc(scanValue, scanSize * selected->size());
		}
	}
}
void MenuInstance::scanByAddress(const uint8_t* comp, Datatype::Operation operation) {
	/* allocate the temporary buffer */
	uint8_t* buffer = (uint8_t*)malloc(selected->size());

	/* loop through all of the addresses and read them */
	size_t offset = 0;
	size_t percent = 0;
	for (size_t i = 0; i < scanSize; i++) {
		SIZE_T read = 0;

		/* try to read the value */
		if (!ReadProcessMemory(process, (void*)scanAddr[i], buffer, selected->size(), &read) || read != selected->size()) {
			offset++;
			continue;
		}

		/* verify the value */
		bool valid = false;
		if (operation == Datatype::Operation::validate)
			valid = selected->validate(buffer);
		else
			valid = selected->test(buffer, comp == 0 ? scanValue + i * selected->size() : comp, operation);

		/* check if the value should be stored */
		if (valid) {
			scanAddr[i - offset] = scanAddr[i];
			std::memcpy(scanValue + (i - offset) * selected->size(), buffer, selected->size());
		}
		else
			offset++;
	}

	/* free the buffer */
	free(buffer);

	/* remove all invalid values */
	if (offset > 0) {
		if ((scanSize -= offset) == 0) {
			free(scanAddr);
			free(scanValue);
		}
		else {
			scanAddr = (uint64_t*)realloc(scanAddr, scanSize * sizeof(uint64_t));
			scanValue = (uint8_t*)realloc(scanValue, scanSize * selected->size());
		}
	}
}
const char* MenuInstance::scanData(Datatype::Operation operation, bool self) {
	/* check if the user should enter the value */
	if (operation != Datatype::Operation::validate && !self) {
		if (!selected->readInput(intermediate, true))
			return "invalid input!";
	}

	/* call the appropriate scan function */
	if (scanSize < 0x8000)
		scanByAddress((self ? 0 : intermediate), operation);
	else
		scanByBatch((self ? 0 : intermediate), operation);
	return "scan successful!";
}
std::ostream& MenuInstance::printIndex(std::ostream& stream, uint64_t index) {
	return (stream << std::setfill('0') << std::setw(7) << std::dec << index);
}
DWORD __stdcall MenuInstance::globalThread(void* ptr) {
	MenuInstance* instance = reinterpret_cast<MenuInstance*>(ptr);

	/* enter the main overwrite loop */
	while (instance->threadState == 0) {
		/* lock the resources */
		EnterCriticalSection(&instance->mutex);

		/* check if overwriting is enabled */
		if (instance->overwrite && instance->process != INVALID_HANDLE_VALUE) {
			/* iterate through the variablesand overwrite them */
			for (const Entry& entry : instance->entries) {
				if (!entry.overwrite)
					continue;
				WriteProcessMemory(instance->process, (void*)entry.address, entry.writeBuffer, entry.datatype->size(), 0);
			}
		}

		/* release the resources and yield cpu time */
		LeaveCriticalSection(&instance->mutex);
		Sleep(1);
	}

	/* mark the thread as being terminated */
	instance->threadState = 2;
	return 0;
}



/* implement the root page */
RootPage* RootPage::acquire() {
	return new RootPage(RootPage::Id, RootPage::Title);
}
menu::Layout RootPage::construct() {
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* update the menu-mode */
	if (instance->process != INVALID_HANDLE_VALUE && instance->moduleOffset == (uint64_t)-1)
		instance->memoryMode = MenuInstance::MemoryMode::memAll;

	/* setup the page-menu */
	menu::Layout layout = instance->layout(Id, false);

	/* add the menu-entries */
	if (menu::Page::host()->getUseCtrl())
		layout.add(MenuInstance::Default::custom + 18, "input: default");
	else
		layout.add(MenuInstance::Default::custom + 18, "input: Control + Num");
	if (!menu::Page::host()->isLoaded(ProcessPage::Id)) {
		if (instance->process == INVALID_HANDLE_VALUE)
			layout.add(MenuInstance::Default::custom + 1, "process: open");
		else {
			layout.add(MenuInstance::Default::custom + 1, "process: change");
			layout.add(MenuInstance::Default::custom + 2, "process: close");
		}
	}
	if (!menu::Page::host()->isLoaded(DatatypePage::Id)) {
		if (instance->selected == 0)
			layout.add(MenuInstance::Default::custom + 3, "datatype: select");
		else
			layout.add(MenuInstance::Default::custom + 3, "datatype: change");
	}
	if (!menu::Page::host()->isLoaded(DumpPage::Id) && instance->process != INVALID_HANDLE_VALUE)
		layout.add(MenuInstance::Default::custom + 5, "view memory");
	if (!menu::Page::host()->isLoaded(EntriesPage::Id))
		layout.add(MenuInstance::Default::custom + 4, "saved entries");
	if (instance->overwrite)
		layout.add(MenuInstance::Default::custom + 7, "overwriting: disable");
	else
		layout.add(MenuInstance::Default::custom + 7, "overwriting: enable");
	if (instance->moduleOffset != (uint64_t)-1) {
		if (instance->memoryMode == MenuInstance::MemoryMode::memAll)
			layout.add(MenuInstance::Default::custom + 6, "memory-mode: static");
		else if (instance->memoryMode == MenuInstance::MemoryMode::memStatic)
			layout.add(MenuInstance::Default::custom + 6, "memory-mode: volatile");
		else
			layout.add(MenuInstance::Default::custom + 6, "memory-mode: all");
	}
	if (instance->process != INVALID_HANDLE_VALUE && instance->selected != 0) {
		if (instance->scanSize == 0) {
			layout.add(MenuInstance::Default::custom + 10, "new scan: unknown");
			layout.add(MenuInstance::Default::custom + 11, "new scan: equal");
			layout.add(MenuInstance::Default::custom + 12, "new scan: unequal");
			if (!instance->selected->restricted()) {
				layout.add(MenuInstance::Default::custom + 13, "new scan: less");
				layout.add(MenuInstance::Default::custom + 14, "new scan: less equal");
				layout.add(MenuInstance::Default::custom + 15, "new scan: greater equal");
				layout.add(MenuInstance::Default::custom + 16, "new scan: greater");
			}
		}
		else {
			if (!menu::Page::host()->isLoaded(PrintPage::Id) && instance->process != INVALID_HANDLE_VALUE)
				layout.add(MenuInstance::Default::custom + 9, "scan: print");
			layout.add(MenuInstance::Default::custom + 8, "scan: restart");
			if (!menu::Page::host()->isLoaded(ScanPage::Id))
				layout.add(MenuInstance::Default::custom + 17, "scan: manage");
		}
	}
	return layout;
}
menu::Behavior RootPage::evaluate(menu::EntryId selected) {
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* setup the default-pageeval */
	menu::Behavior behavior("", menu::Traverse::stay);
	if (instance->eval(selected, &behavior))
		return behavior;

	/* handle the selection */
	switch (selected) {
	case MenuInstance::Default::custom + 1:
		return menu::Behavior("", menu::Traverse::push, ProcessPage::Id);
	case MenuInstance::Default::custom + 2:
		EnterCriticalSection(&instance->mutex);
		CloseHandle(instance->process);
		instance->process = INVALID_HANDLE_VALUE;
		LeaveCriticalSection(&instance->mutex);
		return behavior;
	case MenuInstance::Default::custom + 3:
		if (instance->scanSize > 0)
			return menu::Behavior("", menu::Traverse::push, DatatypeVerifyPage::Id);
		return menu::Behavior("", menu::Traverse::push, DatatypePage::Id);
	case MenuInstance::Default::custom + 4:
		return menu::Behavior("", menu::Traverse::push, EntriesPage::Id);
	case MenuInstance::Default::custom + 5:
		return menu::Behavior("", menu::Traverse::push, DumpPage::Id);
	case MenuInstance::Default::custom + 6:
		if (instance->memoryMode == MenuInstance::MemoryMode::memAll)
			instance->memoryMode = MenuInstance::MemoryMode::memStatic;
		else if (instance->memoryMode == MenuInstance::MemoryMode::memStatic)
			instance->memoryMode = MenuInstance::MemoryMode::memVolatile;
		else
			instance->memoryMode = MenuInstance::MemoryMode::memAll;
		return behavior;
	case MenuInstance::Default::custom + 7:
		EnterCriticalSection(&instance->mutex);
		instance->overwrite = !instance->overwrite;
		LeaveCriticalSection(&instance->mutex);
		return behavior;
	case MenuInstance::Default::custom + 8:
		return menu::Behavior("", menu::Traverse::push, RestartVerifyPage::Id);
	case MenuInstance::Default::custom + 9:
		return menu::Behavior("", menu::Traverse::push, PrintPage::Id);
	case MenuInstance::Default::custom + 10:  /* unknown-scan */
		behavior.response = instance->scanInitial(Datatype::Operation::validate);
		if (instance->scanSize > 0) {
			behavior.traverse = menu::Traverse::push;
			behavior.target = ScanPage::Id;
		}
		return behavior;
	case MenuInstance::Default::custom + 11:  /* equal-scan */
		behavior.response = instance->scanInitial(Datatype::Operation::equal);
		if (instance->scanSize > 0) {
			behavior.traverse = menu::Traverse::push;
			behavior.target = ScanPage::Id;
		}
		return behavior;
	case MenuInstance::Default::custom + 12:  /* unequal-scan */
		behavior.response = instance->scanInitial(Datatype::Operation::unequal);
		if (instance->scanSize > 0) {
			behavior.traverse = menu::Traverse::push;
			behavior.target = ScanPage::Id;
		}
		return behavior;
	case MenuInstance::Default::custom + 13:  /* less-scan */
		behavior.response = instance->scanInitial(Datatype::Operation::less);
		if (instance->scanSize > 0) {
			behavior.traverse = menu::Traverse::push;
			behavior.target = ScanPage::Id;
		}
		return behavior;
	case MenuInstance::Default::custom + 14:  /* less-equal-scan */
		behavior.response = instance->scanInitial(Datatype::Operation::lessEqual);
		if (instance->scanSize > 0) {
			behavior.traverse = menu::Traverse::push;
			behavior.target = ScanPage::Id;
		}
		return behavior;
	case MenuInstance::Default::custom + 15:  /* greater-equal-scan */
		behavior.response = instance->scanInitial(Datatype::Operation::greaterEqual);
		if (instance->scanSize > 0) {
			behavior.traverse = menu::Traverse::push;
			behavior.target = ScanPage::Id;
		}
		return behavior;
	case MenuInstance::Default::custom + 16:  /* greater-scan */
		behavior.response = instance->scanInitial(Datatype::Operation::greater);
		if (instance->scanSize > 0) {
			behavior.traverse = menu::Traverse::push;
			behavior.target = ScanPage::Id;
		}
		return behavior;
	case MenuInstance::Default::custom + 17:  /* manage scan */
		behavior.target = ScanPage::Id;
		behavior.traverse = menu::Traverse::push;
		return behavior;
	case MenuInstance::Default::custom + 18:
		menu::Page::host()->setUseCtrl(!menu::Page::host()->getUseCtrl());
		return behavior;
	}
	return behavior;
}
void RootPage::teardown() {
	delete this;
}



/* implement the exit-verify page */
ExitVerifyPage* ExitVerifyPage::acquire() {
	return new ExitVerifyPage(ExitVerifyPage::Id, ExitVerifyPage::Title);
}
menu::Layout ExitVerifyPage::construct() {
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* setup the page-menu */
	menu::Layout layout(false, "Are you sure you want to exit the program?");

	/* add the menu-entries */
	layout.add(MenuInstance::Default::returnMenu, "no");
	layout.add(MenuInstance::Default::custom, "yes");
	return layout;
}
menu::Behavior ExitVerifyPage::evaluate(menu::EntryId selected) {
	if (selected == MenuInstance::Default::returnMenu)
		return menu::Behavior("", menu::Traverse::pop);
	return menu::Behavior("", menu::Traverse::exit);
}
void ExitVerifyPage::teardown() {
	delete this;
}



/* implement the process page */
ProcessPage* ProcessPage::acquire() {
	return new ProcessPage(ProcessPage::Id, ProcessPage::Title);
}
void ProcessPage::unload() {
	pWindows.clear();
}
menu::Layout ProcessPage::construct() {
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* setup the page-menu */
	menu::Layout layout = instance->layout(Id, false);

	/* enumerate the windows */
	pWindows.clear();
	EnumWindows(ProcessPage::fEnumWindowsProc, (LPARAM)this);

	/* add the menu-entries */
	layout.add(MenuInstance::Default::custom, "refresh");

	/* add all of the windows */
	char buffer[80];
	for (size_t i = 0; i < pWindows.size(); i++) {
		if (GetWindowTextA(pWindows[i], buffer, 80) == 0)
			pWindows.erase(pWindows.begin() + (i--));
		else
			layout.add(MenuInstance::Default::custom + 1 + i, std::string("process: ").append(buffer));
	}
	return layout;
}
menu::Behavior ProcessPage::evaluate(menu::EntryId selected) {
	/* extract the local & global data */
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* setup the default-pageeval */
	menu::Behavior behavior("", menu::Traverse::stay);
	if (instance->eval(selected, &behavior))
		return behavior;

	/* handle the selection */
	if (selected == MenuInstance::Default::custom)
		return behavior;

	/* lock the mutex */
	EnterCriticalSection(&instance->mutex);

	/* extract the process-name */
	selected -= 1 + MenuInstance::Default::custom;
	char buffer[80];
	if (GetWindowTextA(pWindows[selected], buffer, 80) == 0) {
		behavior.response = "failed to extract the name of the process!";
		LeaveCriticalSection(&instance->mutex);
		return behavior;
	}

	/* try to open the process */
	DWORD id = 0;
	GetWindowThreadProcessId(pWindows[selected], &id);
	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, false, id);
	if (process == INVALID_HANDLE_VALUE) {
		behavior.response = "failed to open the process!";
		LeaveCriticalSection(&instance->mutex);
		return behavior;
	}

	/* close the old process */
	if (instance->process != INVALID_HANDLE_VALUE)
		CloseHandle(instance->process);
	instance->process = process;

	/* release the mutex */
	LeaveCriticalSection(&instance->mutex);

	/* set the name */
	instance->processName = std::string(buffer);

	/* clear the variables */
	uint64_t batch_count = 0;
	uint64_t total_memory = 0;

	/* loop through the memory of the process and extract the memory */
	uint64_t nextaddress = 0;
	MEMORY_BASIC_INFORMATION memInfo = { 0 };
	while (VirtualQueryEx(instance->process, (void*)nextaddress, (PMEMORY_BASIC_INFORMATION)&memInfo, sizeof(MEMORY_BASIC_INFORMATION)) != 0) {
		/* check if the memory is writeable as readonly is not of any interest */
		if ((memInfo.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE)) && (memInfo.State & MEM_COMMIT) && (memInfo.Protect & (PAGE_GUARD | PAGE_NOACCESS)) == 0) {
			batch_count++;
			total_memory += memInfo.RegionSize;
		}

		/* adjust the nextaddress */
		nextaddress = (uint64_t)memInfo.BaseAddress + memInfo.RegionSize;
	}

	/* add the data to the message */
	std::stringstream sstr;
	sstr << "process successfully opened!" << std::endl;
	sstr << std::dec << "total-batches: " << batch_count << std::endl;
	sstr << std::dec << "total-memory : " << total_memory << std::endl;
	behavior.traverse = menu::Traverse::root;
	instance->moduleOffset = (uint64_t)-1;

	/* get the module-offset */
	HMODULE modOffset;
	DWORD maxSize;
	if (EnumProcessModules(instance->process, &modOffset, sizeof(HMODULE), &maxSize) == 0) {
		sstr << "static-memory: none (failed to obtain module-offset)";
		behavior.response = sstr.str();
		return behavior;
	}
	sstr << "module-offset: 0x" << (void*)modOffset << std::endl;
	sstr << "static-memory: ";
	instance->staticSectionStart = (uint64_t)modOffset;
	instance->moduleOffset = (uint64_t)modOffset;

	/* find the file and open it (to extract the static addresses) */
	char nameBuffer[1024];
	memset(nameBuffer, 0, 1024);
	if (GetModuleFileNameExA(instance->process, 0, nameBuffer, 1024) == 0) {
		sstr << "none (failed to obtain module-path)";
		instance->moduleOffset = (uint64_t)-1;
		behavior.response = sstr.str();
		return behavior;
	}

	/* open the file */
	HANDLE fileHandle = CreateFileA(nameBuffer, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	if (fileHandle == INVALID_HANDLE_VALUE) {
		sstr << "none (failed to open module-file)";
		instance->moduleOffset = (uint64_t)-1;
		behavior.response = sstr.str();
		return behavior;
	}

	/* extract the offset to the dos-header */
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (ReadFile(fileHandle, &dosHeader, sizeof(IMAGE_DOS_HEADER), 0, 0) == 0) {
		CloseHandle(fileHandle);
		sstr << "none (failed to read DOS-Header)";
		instance->moduleOffset = (uint64_t)-1;
		behavior.response = sstr.str();
		return behavior;
	}

	/* extract the nt-header */
	IMAGE_NT_HEADERS32 imageHeader = { 0 };
	if (SetFilePointer(fileHandle, dosHeader.e_lfanew, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
		CloseHandle(fileHandle);
		sstr << "none (failed to read PE-Header)";
		instance->moduleOffset = (uint64_t)-1;
		behavior.response = sstr.str();
		return behavior;
	}
	if (ReadFile(fileHandle, &imageHeader, sizeof(IMAGE_NT_HEADERS32), 0, 0) == 0) {
		CloseHandle(fileHandle);
		sstr << "none (failed to read PE-Header)";
		instance->moduleOffset = (uint64_t)-1;
		behavior.response = sstr.str();
		return behavior;
	}

	/* check if its a 32 or 64-bit file */
	if (imageHeader.OptionalHeader.Magic == 0x10b) {
		/* extract the section-headers (and look for the .data-header) */
		IMAGE_SECTION_HEADER sectionHeader = { 0 };
		if (imageHeader.FileHeader.NumberOfSections == 0) {
			CloseHandle(fileHandle);
			sstr << "none (.data section not found [x86])";
			instance->moduleOffset = (uint64_t)-1;
			behavior.response = sstr.str();
			return behavior;
		}
		for (uint64_t i = 0; i < imageHeader.FileHeader.NumberOfSections; i++) {
			/* read the header */
			if (ReadFile(fileHandle, &sectionHeader, sizeof(IMAGE_SECTION_HEADER), 0, 0) == 0) {
				CloseHandle(fileHandle);
				sstr << "none (failed to read SECTION-Header [x86])";
				instance->moduleOffset = (uint64_t)-1;
				behavior.response = sstr.str();
				return behavior;
			}

			/* check if its the .data-header */
			if (memcmp(sectionHeader.Name, ".data", 6) == 0) {
				CloseHandle(fileHandle);
				instance->staticSectionStart += sectionHeader.VirtualAddress;
				instance->staticSectionEnd = sectionHeader.Misc.VirtualSize + instance->staticSectionStart;
				break;
			}
			else if (i + 1 == imageHeader.FileHeader.NumberOfSections) {
				/* setup the final string */
				CloseHandle(fileHandle);
				sstr << "none (.data section not found [x86])";
				instance->moduleOffset = (uint64_t)-1;
				behavior.response = sstr.str();
				return behavior;
			}
		}
	}
	else {
		/* extract the 64nt-header */
		IMAGE_NT_HEADERS64 imageHeader64 = { 0 };
		if (SetFilePointer(fileHandle, dosHeader.e_lfanew, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
			CloseHandle(fileHandle);
			sstr << "none (failed to read PE-Header [x86_64])";
			instance->moduleOffset = (uint64_t)-1;
			behavior.response = sstr.str();
			return behavior;
		}
		if (ReadFile(fileHandle, &imageHeader64, sizeof(IMAGE_NT_HEADERS64), 0, 0) == 0) {
			CloseHandle(fileHandle);
			sstr << "none (failed to read PE-Header [x86_64])";
			instance->moduleOffset = (uint64_t)-1;
			behavior.response = sstr.str();
			return behavior;
		}

		/* extract the section-headers (and look for the .data-header) */
		IMAGE_SECTION_HEADER sectionHeader = { 0 };
		if (imageHeader64.FileHeader.NumberOfSections == 0) {
			CloseHandle(fileHandle);
			sstr << "none (.data section not found [x86_64])";
			instance->moduleOffset = (uint64_t)-1;
			behavior.response = sstr.str();
			return behavior;
		}
		for (uint64_t i = 0; i < imageHeader64.FileHeader.NumberOfSections; i++) {
			/* read the header */
			if (ReadFile(fileHandle, &sectionHeader, sizeof(IMAGE_SECTION_HEADER), 0, 0) == 0) {
				CloseHandle(fileHandle);
				sstr << "none (failed to read SECTION-Header [x86_64])";
				instance->moduleOffset = (uint64_t)-1;
				behavior.response = sstr.str();
				return behavior;
			}

			/* check if its the .data-header */
			if (memcmp(sectionHeader.Name, ".data", 6) == 0) {
				CloseHandle(fileHandle);
				instance->staticSectionStart += sectionHeader.VirtualAddress;
				instance->staticSectionEnd = sectionHeader.Misc.VirtualSize + instance->staticSectionStart;
				break;
			}
			else if (i + 1 == imageHeader64.FileHeader.NumberOfSections) {
				/* setup the final string */
				CloseHandle(fileHandle);
				sstr << "none (.data section not found [x86_64])";
				instance->moduleOffset = (uint64_t)-1;
				behavior.response = sstr.str();
				return behavior;
			}
		}
	}

	/* add the output-result */
	sstr << std::hex << "0x" << (void*)instance->staticSectionStart << " - 0x" << (void*)instance->staticSectionEnd;
	behavior.response = sstr.str();
	return behavior;
}
void ProcessPage::teardown() {
	delete this;
}
BOOL _stdcall ProcessPage::fEnumWindowsProc(HWND hwnd, LPARAM lParam) {
	char Text[8];
	if (IsWindowVisible(hwnd)) {
		GetWindowTextA(hwnd, Text, 8);
		if (Text[0] != '\0')
			reinterpret_cast<ProcessPage*>(lParam)->pWindows.push_back(hwnd);
	}
	return TRUE;
}



/* implement the datatype page */
DatatypePage* DatatypePage::acquire() {
	return new DatatypePage(DatatypePage::Id, DatatypePage::Title);
}
menu::Layout DatatypePage::construct() {
	/* extract the local & global data */
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* setup the page-menu */
	menu::Layout layout = instance->layout(Id, false);

	/* add all of the datatypes */
	for (size_t i = 0; i < instance->types.size(); i++)
		layout.add(MenuInstance::Default::custom + i, "datatype: " + std::string(instance->types[i]->name()));
	return layout;
}
menu::Behavior DatatypePage::evaluate(menu::EntryId selected) {
	/* extract the local & global data */
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* setup the default-pageeval */
	menu::Behavior behavior("", menu::Traverse::pop);
	if (instance->eval(selected, &behavior))
		return behavior;

	/* check if the type has changed */
	selected -= MenuInstance::Default::custom;
	if (instance->selected == instance->types[selected]) {
		behavior.response = "datatype unchanged!";
		return behavior;
	}

	/* clear the current scan */
	if (instance->scanSize > 0) {
		free(instance->scanAddr);
		free(instance->scanValue);
		instance->scanSize = 0;
	}

	/* set the datatype */
	instance->selected = instance->types[selected];
	behavior.response = "datatype changed!";
	behavior.traverse = menu::Traverse::root;

	/* update the intermediate buffer */
	if (instance->intermediate != 0)
		free(instance->intermediate);
	instance->intermediate = (uint8_t*)malloc(instance->selected->size());
	return behavior;
}
void DatatypePage::teardown() {
	delete this;
}



/* implement the datatype-verify page */
DatatypeVerifyPage* DatatypeVerifyPage::acquire() {
	return new DatatypeVerifyPage(DatatypeVerifyPage::Id, DatatypeVerifyPage::Title);
}
menu::Layout DatatypeVerifyPage::construct() {
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* setup the page-menu */
	menu::Layout layout(false, "Are you sure you want to change the datatype?\nThe current scan will be lost.");

	/* add the menu-entries */
	layout.add(MenuInstance::Default::returnMenu, "no");
	layout.add(MenuInstance::Default::custom, "yes");
	return layout;
}
menu::Behavior DatatypeVerifyPage::evaluate(menu::EntryId selected) {
	if (selected == MenuInstance::Default::returnMenu)
		return menu::Behavior("", menu::Traverse::pop);
	return menu::Behavior("", menu::Traverse::move, DatatypePage::Id);
}
void DatatypeVerifyPage::teardown() {
	delete this;
}



/* implement the restart-verify page */
RestartVerifyPage* RestartVerifyPage::acquire() {
	return new RestartVerifyPage(RestartVerifyPage::Id, RestartVerifyPage::Title);
}
menu::Layout RestartVerifyPage::construct() {
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* setup the page-menu */
	menu::Layout layout(false, "Are you sure you want to restart the scan?\nThe current scan will be lost.");

	/* add the menu-entries */
	layout.add(MenuInstance::Default::returnMenu, "no");
	layout.add(MenuInstance::Default::custom, "yes");
	return layout;
}
menu::Behavior RestartVerifyPage::evaluate(menu::EntryId selected) {
	/* check if a restart has been selected */
	if (selected == MenuInstance::Default::returnMenu)
		return menu::Behavior("", menu::Traverse::pop);
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* clear the scan */
	free(instance->scanAddr);
	free(instance->scanValue);
	instance->scanSize = 0;
	return menu::Behavior("", menu::Traverse::root);
}
void RestartVerifyPage::teardown() {
	delete this;
}



/* implement the scan page */
ScanPage* ScanPage::acquire() {
	return new ScanPage(ScanPage::Id, ScanPage::Title);
}
menu::Layout ScanPage::construct() {
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* setup the page-menu */
	menu::Layout layout = instance->layout(Id, false);

	/* add the menu-entries */
	if (instance->scanSize > 0) {
		if (!menu::Page::host()->isLoaded(ConstantPage::Id))
			layout.add(MenuInstance::Default::custom + 1, "filter out fluctuations");
		if (!menu::Page::host()->isLoaded(PrintPage::Id))
			layout.add(MenuInstance::Default::custom + 2, "scan: print");
		layout.add(MenuInstance::Default::custom + 3, "scan: restart");
		layout.add(MenuInstance::Default::custom + 4, "scan: refresh");
		layout.add(MenuInstance::Default::custom + 5, "scan: unchanged");
		layout.add(MenuInstance::Default::custom + 6, "scan: changed");
		if (!instance->selected->restricted()) {
			layout.add(MenuInstance::Default::custom + 7, "scan: decreased");
			layout.add(MenuInstance::Default::custom + 8, "scan: increased");
		}
		layout.add(MenuInstance::Default::custom + 9, "scan: equal to");
		layout.add(MenuInstance::Default::custom + 10, "scan: unequal to");
		if (!instance->selected->restricted()) {
			layout.add(MenuInstance::Default::custom + 11, "scan: less than");
			layout.add(MenuInstance::Default::custom + 12, "scan: less-equal than");
			layout.add(MenuInstance::Default::custom + 13, "scan: greater-equal than");
			layout.add(MenuInstance::Default::custom + 14, "scan: greater than");
		}
	}
	else
		layout.header.append("\n\nThe current scan is empty!");
	return layout;
}
menu::Behavior ScanPage::evaluate(menu::EntryId selected) {
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* setup the default-pageeval */
	menu::Behavior behavior("", menu::Traverse::stay);
	if (instance->eval(selected, &behavior))
		return behavior;

	/* handle the selection */
	switch (selected) {
	case MenuInstance::Default::custom + 1:  /* constant-filter */
		behavior.target = ConstantPage::Id;
		behavior.traverse = menu::Traverse::push;
		break;
	case MenuInstance::Default::custom + 2:  /* print-scan */
		behavior.target = PrintPage::Id;
		behavior.traverse = menu::Traverse::push;
		break;
	case MenuInstance::Default::custom + 3:  /* restart-scan */
		behavior.target = RestartVerifyPage::Id;
		behavior.traverse = menu::Traverse::push;
		break;
	case MenuInstance::Default::custom + 4:  /* refresh */
		behavior.response = instance->scanData(Datatype::Operation::validate, false);
		break;
	case MenuInstance::Default::custom + 5:  /* unchanged */
		behavior.response = instance->scanData(Datatype::Operation::equal, true);
		break;
	case MenuInstance::Default::custom + 6:  /* changed */
		behavior.response = instance->scanData(Datatype::Operation::unequal, true);
		break;
	case MenuInstance::Default::custom + 7:  /* decreased */
		behavior.response = instance->scanData(Datatype::Operation::less, true);
		break;
	case MenuInstance::Default::custom + 8:  /* increased */
		behavior.response = instance->scanData(Datatype::Operation::greater, true);
		break;

	case MenuInstance::Default::custom + 9:  /* equal */
		behavior.response = instance->scanData(Datatype::Operation::equal, false);
		break;
	case MenuInstance::Default::custom + 10:  /* unequal */
		behavior.response = instance->scanData(Datatype::Operation::unequal, false);
		break;
	case MenuInstance::Default::custom + 11:  /* less */
		behavior.response = instance->scanData(Datatype::Operation::less, false);
		break;
	case MenuInstance::Default::custom + 12:  /* less-equal */
		behavior.response = instance->scanData(Datatype::Operation::lessEqual, false);
		break;
	case MenuInstance::Default::custom + 13:  /* greater-equal */
		behavior.response = instance->scanData(Datatype::Operation::greaterEqual, false);
		break;
	default:  /* greater */
		behavior.response = instance->scanData(Datatype::Operation::greater, false);
		break;
	}
	return behavior;
}
void ScanPage::teardown() {
	delete this;
}



/* implement the constant page */
ConstantPage* ConstantPage::acquire() {
	return new ConstantPage(ConstantPage::Id, ConstantPage::Title);
}
bool ConstantPage::update() {
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* refresh the data */
	size_t size = instance->scanSize;
	instance->scanData(Datatype::Operation::equal, true);

	/* check if the screen should be refreshed */
	if (size != instance->scanSize)
		pRefresh = true;
	return !pRefresh || GetTickCount64() - pStamp < 125;
}
menu::Layout ConstantPage::construct() {
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* reset the refresh-data */
	pRefresh = false;
	pStamp = GetTickCount64();

	/* setup the page-menu */
	menu::Layout layout = instance->layout(Id, false);

	/* return the input-type */
	if (instance->scanSize == 0)
		layout.header.append("\n\nThe current scan is empty!");
	else
		layout.updateLoop = true;
	return layout;
}
menu::Behavior ConstantPage::evaluate(menu::EntryId selected) {
	/* setup the page-menu */
	menu::Behavior behavior("", menu::Traverse::pop);
	((MenuInstance*)menu::Page::instance())->eval(selected, &behavior);
	return behavior;
}
void ConstantPage::teardown() {
	delete this;
}



/* implement the print page */
PrintPage* PrintPage::acquire() {
	return new PrintPage(PrintPage::Id, PrintPage::Title);
}
bool PrintPage::update() {
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* check if the screen should refresh */
	if (pRefresh && GetTickCount64() - pStamp >= 125)
		return false;

	/* cache the current data */
	uint8_t* buffer = (uint8_t*)malloc(instance->scanSize * instance->selected->size());
	std::memcpy(buffer, instance->scanValue, instance->scanSize * instance->selected->size());

	/* refresh the data */
	size_t size = instance->scanSize;
	instance->scanData(Datatype::Operation::validate, false);
	if (size != instance->scanSize)
		pRefresh = true;

	/* check if any of the data have changed */
	else {
		for (size_t i = 0; i < instance->scanSize; i++) {
			uint64_t offset = i * instance->selected->size();
			if (instance->selected->test(buffer + offset, instance->scanValue + offset, Datatype::Operation::equal))
				continue;
			pRefresh = true;
			break;
		}
	}

	/* release the resources and check if the screen should be refreshed */
	free(buffer);
	return !pRefresh || GetTickCount64() - pStamp < 125;
}
menu::Layout PrintPage::construct() {
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* create the page-object */
	menu::Layout layout = instance->layout(Id, true);
	uint64_t size = instance->scanSize;
	if (instance->scanSize > 0) {
		/* refresh the data */
		instance->scanData(Datatype::Operation::validate, false);

		/* write the current scan to the output */
		std::stringstream sstr;
		for (size_t i = 0; i < instance->scanSize; i++) {
			/* check if the address is static */
			if (instance->moduleOffset != (uint64_t)-1) {
				if (instance->scanAddr[i] >= instance->staticSectionStart && instance->scanAddr[i] < instance->staticSectionEnd)
					instance->printIndex(sstr, i) << " - " << "[module+0x" << std::hex << (void*)(instance->scanAddr[i] - instance->moduleOffset) << "] -> ";
				else
					instance->printIndex(sstr, i) << " - " << " [base+0x" << std::hex << (void*)instance->scanAddr[i] << "]  -> ";
			}
			else
				instance->printIndex(sstr, i) << " - [0x" << std::hex << (void*)instance->scanAddr[i] << "] -> ";
			sstr << instance->selected->toString(instance->scanValue + i * instance->selected->size()) << std::endl;
		}
		layout.header.append("\n\n").append(sstr.str());

		/* add the menu-entries */
		layout.add(MenuInstance::Default::custom + 1, "remove");
		if (!menu::Page::host()->isLoaded(DumpPage::Id))
			layout.add(MenuInstance::Default::custom + 4, "view in memory");
		if (!menu::Page::host()->isLoaded(EntriesPage::Id))
			layout.add(MenuInstance::Default::custom + 6, "saved entries");
		layout.add(MenuInstance::Default::custom + 2, "save");
		layout.add(MenuInstance::Default::custom + 3, "save all");
		layout.add(MenuInstance::Default::custom + 5, "change value");
	}
	else {
		layout.header.append("\n\nThe current scan is empty!");
		layout.updateLoop = false;
	}

	/* reset the refresh-data */
	pRefresh = size != instance->scanSize;
	pStamp = GetTickCount64();

	/* return the page-structure */
	return layout;
}
menu::Behavior PrintPage::evaluate(menu::EntryId selected) {
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* setup the default-pageeval */
	menu::Behavior behavior("", menu::Traverse::stay);
	if (instance->eval(selected, &behavior))
		return behavior;

	/* handle the selection */
	if (selected == MenuInstance::Default::custom + 1) {
		size_t index = 0;

		/* receive the index */
		if (const char* result = instance->queryIndex("enter the index", index, instance->scanSize)) {
			behavior.response = result;
			return behavior;
		}

		/* remove the entry */
		for (size_t i = index + 1; i < instance->scanSize; i++) {
			instance->scanAddr[i - 1] = instance->scanAddr[i];
			memcpy((void*)((uint64_t)instance->scanValue + (i - 1) * instance->selected->size()), (void*)((uint64_t)instance->scanValue + i * instance->selected->size()), instance->selected->size());
		}

		/* resize the array */
		instance->scanSize--;
		if (instance->scanSize == 0) {
			free(instance->scanAddr);
			free(instance->scanValue);
		}
		else {
			instance->scanAddr = (uint64_t*)realloc(instance->scanAddr, instance->scanSize * sizeof(uint64_t));
			instance->scanValue = (uint8_t*)realloc(instance->scanValue, instance->scanSize * instance->selected->size());
		}
		behavior.response = "entry successfully removed!";
	}
	else if (selected == MenuInstance::Default::custom + 2) {
		size_t index = 0;

		/* receive the index */
		if (const char* result = instance->queryIndex("enter the index", index, instance->scanSize)) {
			behavior.response = result;
			return behavior;
		}

		/* create the new entry */
		instance->entry(instance->scanAddr[index], instance->selected, instance->scanValue + index * instance->selected->size());
		behavior.response = "entry successfully added to list!";
	}
	else if (selected == MenuInstance::Default::custom + 3) {
		std::string name;

		/* receive the name */
		while (name.empty()) {
			std::cout << "enter the name: ";
			std::getline(std::cin, name);
			if (name.empty())
				std::cout << "invalid name!" << std::endl;
		}

		/* loop through the entries and add them */
		for (size_t i = 0; i < instance->scanSize; i++)
			instance->entry(instance->scanAddr[i], instance->selected, instance->scanValue + i * instance->selected->size(), name + '_' + std::to_string(i));
		behavior.response = "entries successfully added to list!";
	}
	else if (selected == MenuInstance::Default::custom + 4) {
		size_t index = 0;

		/* receive the index */
		if (const char* result = instance->queryIndex("enter the index", index, instance->scanSize)) {
			behavior.response = result;
			return behavior;
		}

		/* set the address */
		instance->dumpAddress = instance->scanAddr[index];
		if (instance->dumpAddress > 64)
			instance->dumpAddress -= 64;
		else
			instance->dumpAddress = 0;
		if (instance->selected->size() == instance->selected->align()) {
			bool isMultiple = false;
			for (size_t i = 1; i <= 64; i <<= 1) {
				if (instance->selected->size() == i) {
					isMultiple = true;
					break;
				}
			}
			if (isMultiple) {
				instance->dumpType = instance->selected;
				behavior.response = "address & type of memory-view set!";
			}
			else {
				instance->dumpType = 0;
				behavior.response = "address of memory-view set (type is not suited)!";
			}
		}
		else
			behavior.response = "address of memory-view set (type is not suited)!";
		behavior.target = DumpPage::Id;
		behavior.traverse = menu::Traverse::push;
	}
	else if (selected == MenuInstance::Default::custom + 5) {
		size_t index = 0;

		/* receive the index */
		if (const char* result = instance->queryIndex("enter the index", index, instance->scanSize)) {
			behavior.response = result;
			return behavior;
		}

		/* receive the new value to write */
		if (!instance->selected->readInput(instance->intermediate, false))
			behavior.response = "invalid value!";
		else {
			/* try to write the value to memory */
			if (WriteProcessMemory(instance->process, (void*)instance->scanAddr[index], instance->intermediate, instance->selected->size(), 0))
				behavior.response = "value written!";
			else
				behavior.response = "writing failed!";
		}
	}
	else {
		behavior.traverse = menu::Traverse::push;
		behavior.target = EntriesPage::Id;
	}

	return behavior;
}
void PrintPage::teardown() {
	delete this;
}



/* implement the entries page */
EntriesPage* EntriesPage::acquire() {
	return new EntriesPage(EntriesPage::Id, EntriesPage::Title);
}
bool EntriesPage::update() {
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* check if the screen should refresh */
	if (pRefresh && GetTickCount64() - pStamp >= 125)
		return false;

	/* iterate through the entries and check for changes */
	for (size_t i = 0; i < instance->entries.size(); i++) {
		MenuInstance::Entry& entry = instance->entries[i];
		if (entry.overwrite && instance->overwrite)
			continue;

		/* read the data into a temporary buffer */
		uint8_t* buffer = (uint8_t*)malloc(entry.datatype->size());
		if (ReadProcessMemory(instance->process, (void*)entry.address, buffer, entry.datatype->size(), 0)) {
			if (!entry.datatype->test(buffer, entry.buffer, Datatype::Operation::equal))
				pRefresh = true;
		}
		free(buffer);
		if (pRefresh)
			break;
	}

	/* check if the screen should be refreshed */
	return !pRefresh || GetTickCount64() - pStamp < 125;
}
menu::Layout EntriesPage::construct() {
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* create the page-object */
	menu::Layout layout = instance->layout(Id, true);
	if (instance->process == INVALID_HANDLE_VALUE || instance->entries.size() == 0)
		layout.updateLoop = false;

	/* create the strings */
	if (instance->entries.size() > 0) {
		/* refresh the values and write the current scan to the output */
		std::stringstream sstr;
		for (size_t i = 0; i < instance->entries.size(); i++) {
			/* read the current value */
			if (instance->process != INVALID_HANDLE_VALUE) {
				if (!instance->entries[i].overwrite || !instance->overwrite)
					ReadProcessMemory(instance->process, (void*)instance->entries[i].address, instance->entries[i].buffer, instance->entries[i].datatype->size(), 0);
				else
					memcpy(instance->entries[i].buffer, instance->entries[i].writeBuffer, instance->entries[i].datatype->size());
			}

			/* add the entry to the result-string */
			if (instance->entries[i].overwrite)
				instance->printIndex(sstr, i) << " - w - ";
			else
				instance->printIndex(sstr, i) << " - r - ";

			/* write the name to the string */
			for (uint32_t j = 0; j < 16; j++) {
				if (j < instance->entries[i].name.size())
					sstr << instance->entries[i].name[j];
				else
					sstr << ' ';
			}

			/* write the address to the string */
			if (instance->moduleOffset != (uint64_t)-1) {
				if (instance->entries[i].address >= instance->staticSectionStart && instance->entries[i].address < instance->staticSectionEnd)
					sstr << " [module+0x" << std::hex << (void*)(instance->entries[i].address - instance->moduleOffset) << "] -> ";
				else
					sstr << "  [base+0x" << std::hex << (void*)instance->entries[i].address << "]  -> ";
			}
			else
				sstr << " [0x" << std::hex << (void*)instance->scanAddr[i] << "] -> ";

			/* write the address and the value to the string */
			sstr << instance->entries[i].datatype->toString(instance->entries[i].buffer) << std::endl;
		}
		layout.header.append("\n\n").append(sstr.str());

		/* add the menu-entries */
		layout.add(MenuInstance::Default::custom + 1, "remove");
		if (!menu::Page::host()->isLoaded(DumpPage::Id))
			layout.add(MenuInstance::Default::custom + 5, "view in memory");
		layout.add(MenuInstance::Default::custom + 3, "change value");
		if (instance->overwrite)
			layout.add(MenuInstance::Default::custom + 7, "overwriting: disable");
		else
			layout.add(MenuInstance::Default::custom + 7, "overwriting: enable");
		layout.add(MenuInstance::Default::custom + 2, "toggle overwrite");
		layout.add(MenuInstance::Default::custom + 4, "rename");
	}
	else
		layout.header.append("\n\nThere are no entries!");
	layout.add(MenuInstance::Default::custom + 6, "add value by address");

	/* reset the refresh-data */
	pStamp = GetTickCount64();
	pRefresh = false;

	/* return the page-structure */
	return layout;
}
menu::Behavior EntriesPage::evaluate(menu::EntryId selected) {
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* setup the default-pageeval */
	menu::Behavior behavior("", menu::Traverse::stay);
	if (instance->eval(selected, &behavior))
		return behavior;

	/* handle the selection */
	if (selected == MenuInstance::Default::custom + 1) {
		size_t index = 0;

		/* receive the index */
		if (const char* result = instance->queryIndex("enter the index", index, instance->entries.size())) {
			behavior.response = result;
			return behavior;
		}

		/* remove the entry */
		EnterCriticalSection(&instance->mutex);
		free(instance->entries[index].buffer);
		free(instance->entries[index].writeBuffer);
		instance->entries.erase(instance->entries.begin() + index);
		behavior.response = "entry removed!";
		LeaveCriticalSection(&instance->mutex);
	}
	else if (selected == MenuInstance::Default::custom + 2) {
		size_t index = 0;

		/* receive the index */
		if (const char* result = instance->queryIndex("enter the index", index, instance->entries.size())) {
			behavior.response = result;
			return behavior;
		}

		/* check if overwriting is being enabled or disabled */
		EnterCriticalSection(&instance->mutex);
		if (instance->entries[index].overwrite) {
			instance->entries[index].overwrite = false;
			behavior.response = "entry overwriting disabled!";
		}
		else {
			memcpy(instance->entries[index].writeBuffer, instance->entries[index].buffer, instance->entries[index].datatype->size());
			instance->entries[index].overwrite = true;
			behavior.response = "entry overwriting enabled!";
		}
		LeaveCriticalSection(&instance->mutex);
	}
	else if (selected == MenuInstance::Default::custom + 3) {
		size_t index = 0;

		/* receive the index */
		if (const char* result = instance->queryIndex("enter the index", index, instance->entries.size())) {
			behavior.response = result;
			return behavior;
		}

		/* lock the mutex */
		EnterCriticalSection(&instance->mutex);

		/* receive the new value to write */
		if (!instance->entries[index].datatype->readInput(instance->entries[index].writeBuffer, false))
			behavior.response = "invalid value!";
		else {
			/* try to write the value to memory */
			if (instance->overwrite && instance->entries[index].overwrite) {
				memcpy(instance->entries[index].buffer, instance->entries[index].writeBuffer, instance->entries[index].datatype->size());
				behavior.response = "value written!";
			}
			else if (WriteProcessMemory(instance->process, (void*)instance->entries[index].address, instance->entries[index].writeBuffer, instance->entries[index].datatype->size(), 0)) {
				memcpy(instance->entries[index].buffer, instance->entries[index].writeBuffer, instance->entries[index].datatype->size());
				behavior.response = "value written!";
			}
			else
				behavior.response = "writing failed!";
		}
		LeaveCriticalSection(&instance->mutex);
	}
	else if (selected == MenuInstance::Default::custom + 4) {
		size_t index = 0;

		/* receive the index */
		if (const char* result = instance->queryIndex("enter the index", index, instance->entries.size())) {
			behavior.response = result;
			return behavior;
		}

		/* receive the name */
		std::cout << "enter the new name: ";
		std::getline(std::cin, instance->entries[index].name);
		behavior.response = "entry renamed!";
	}
	else if (selected == MenuInstance::Default::custom + 5) {
		size_t index = 0;

		/* receive the index */
		if (const char* result = instance->queryIndex("enter the index", index, instance->entries.size())) {
			behavior.response = result;
			return behavior;
		}

		/* set the address */
		instance->dumpAddress = instance->entries[index].address;
		if (instance->dumpAddress > 64)
			instance->dumpAddress -= 64;
		else
			instance->dumpAddress = 0;
		if (instance->entries[index].datatype->size() == instance->entries[index].datatype->align()) {
			bool isMultiple = false;
			for (size_t i = 1; i <= 64; i <<= 1) {
				if (instance->entries[index].datatype->size() == i) {
					isMultiple = true;
					break;
				}
			}
			if (isMultiple) {
				instance->dumpType = instance->entries[index].datatype;
				behavior.response = "address & type of memory-view set!";
			}
			else {
				instance->dumpType = 0;
				behavior.response = "address of memory-view set (type is not suited)!";
			}
		}
		else
			behavior.response = "address of memory-view set (type is not suited)!";
		behavior.target = DumpPage::Id;
		behavior.traverse = menu::Traverse::push;
	}
	else if (selected == MenuInstance::Default::custom + 6) {
		/* receive the address */
		uint64_t address = 0;
		if (!menu::Page::host()->getNumber("enter the address", address, true)) {
			behavior.response = "invalid number!";
			return behavior;
		}

		/* receive the datatype */
		for (size_t i = 0; i < instance->types.size(); i++)
			std::cout << "[" << (instance->types.size() >= 10 && i < 10 ? "0" : "") << std::dec << i << "] - " << instance->types[i]->name() << std::endl;

		/* receive the index */
		size_t index = 0;
		if (const char* result = instance->queryIndex("select the datatype", index, instance->types.size())) {
			behavior.response = result;
			return behavior;
		}



		/* add the entry */
		instance->entry(address, instance->types[index], 0);
		behavior.response = "entry successfully added to list!";
	}
	else {
		EnterCriticalSection(&instance->mutex);
		instance->overwrite = !instance->overwrite;
		behavior.response = "overwrite toggled!";
		LeaveCriticalSection(&instance->mutex);
	}
	return behavior;
}
void EntriesPage::teardown() {
	delete this;
}



/* implement the dump page */
DumpPage* DumpPage::acquire() {
	return new DumpPage(DumpPage::Id, DumpPage::Title);
}
bool DumpPage::update() {
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* check if the screen should refresh */
	if (pRefresh && GetTickCount64() - pStamp >= 125)
		return false;

	/* read the memory and check if the data have changed */
	uint8_t buffer[256] = { 0 };
	if (ReadProcessMemory(instance->process, (void*)instance->dumpAddress, buffer, 256, 0)) {
		if (!pScanValid)
			pRefresh = (pScanValid = true);
		else if (instance->dumpType == 0) {
			if (memcmp(buffer, pBuffer, 256) != 0)
				pRefresh = true;
		}

		/* check if any of the values have changed */
		else {
			for (size_t i = 0; i < 256; i += instance->dumpType->size()) {
				if (instance->dumpType->test(buffer + i, pBuffer + i, Datatype::Operation::equal))
					continue;
				pRefresh = true;
				break;
			}
		}
	}

	/* check if the buffer should be updated */
	if (pRefresh)
		memcpy(pBuffer, buffer, 256);

	/* check if the screen should be refreshed */
	return !pRefresh || GetTickCount64() - pStamp < 125;
}
menu::Layout DumpPage::construct() {
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* create the page-object */
	menu::Layout layout = instance->layout(Id, true);

	/* add the default-entries */
	layout.add(MenuInstance::Default::custom + 8, "select datatype");
	if (instance->dumpType != 0)
		layout.add(MenuInstance::Default::custom + 9, "change value");
	layout.add(MenuInstance::Default::custom + 2, "decrease address");
	layout.add(MenuInstance::Default::custom + 3, "previous chunk (-256)");
	layout.add(MenuInstance::Default::custom + 4, "previous chunk (-128)");
	layout.add(MenuInstance::Default::custom + 1, "set address");
	layout.add(MenuInstance::Default::custom + 5, "next chunk (+128)");
	layout.add(MenuInstance::Default::custom + 6, "next chunk (+256)");
	layout.add(MenuInstance::Default::custom + 7, "increase address");
	if (instance->moduleOffset != (uint64_t)-1)
		layout.add(MenuInstance::Default::custom + 11, "dump static memory to file");

	/* align the address to the currently selected type */
	if (instance->dumpType != 0)
		instance->dumpAddress -= (instance->dumpAddress % instance->dumpType->size());
	else
		instance->dumpAddress -= (instance->dumpAddress % 8);

	/* read the memory */
	pScanValid = ReadProcessMemory(instance->process, (void*)instance->dumpAddress, pBuffer, 256, 0);

	/* create the header-string */
	std::stringstream sstr;
	if (instance->moduleOffset != (uint64_t)-1)
		sstr << " ";
	sstr << "address: 0x" << std::hex << (void*)instance->dumpAddress << std::endl;
	layout.header.append("\n\n").append(sstr.str());

	/* add the entries */
	if (instance->dumpType == 0) {

		/* create the hex-dump */
		sstr.str("");
		sstr.clear();
		if (instance->moduleOffset != (uint64_t)-1)
			sstr << " ";

		/* print the header */
		if (instance->dumpTypeSpecific == 0)
			sstr << "hex | 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f |" << std::endl;
		else if (instance->dumpTypeSpecific == 1)
			sstr << "hex | 00       04       08       0c       |" << std::endl;
		else
			sstr << "hex | 00               08               |" << std::endl;

		/* print the separation-line */
		if (instance->moduleOffset != (uint64_t)-1)
			sstr << "-";
		sstr << "----+-------------------------------------------------+-----------------" << std::endl;

		/* print the separate rows */
		for (size_t i = 0; i < 16; i++) {
			/* create the static-data */
			if (instance->moduleOffset != (uint64_t)-1) {
				if ((i << 4) + instance->dumpAddress >= instance->staticSectionStart && (i << 4) + instance->dumpAddress < instance->staticSectionEnd)
					sstr << "s";
				else
					sstr << "v";
			}

			/* create the initial string */
			sstr << ' ' << std::dec << (char)(i < 10 ? ('0' + i) : ('a' + (i - 10))) << '0' << " | ";

			/* add the hex-data */
			if (instance->dumpTypeSpecific == 0) {
				for (size_t j = 0; j < 16; j++) {
					if (pScanValid) {
						if (pBuffer[(i << 4) + j] >= 0xa0)
							sstr << (char)('a' + ((pBuffer[(i << 4) + j] >> 4) - 10));
						else
							sstr << (char)('0' + (pBuffer[(i << 4) + j] >> 4));
						if ((pBuffer[(i << 4) + j] & 0x0f) >= 0x0a)
							sstr << (char)('a' + ((pBuffer[(i << 4) + j] & 0x0f) - 10));
						else
							sstr << (char)('0' + (pBuffer[(i << 4) + j] & 0x0f));
						sstr << ' ';
					}
					else
						sstr << "?? ";
				}
			}
			else if (instance->dumpTypeSpecific == 1) {
				for (size_t j = 0; j < 16; j += 4) {
					if (pScanValid) {
						for (size_t k = 0; k < 4; k++) {
							if (pBuffer[(i << 4) + j + (3 - k)] >= 0xa0)
								sstr << (char)('a' + ((pBuffer[(i << 4) + j + (3 - k)] >> 4) - 10));
							else
								sstr << (char)('0' + (pBuffer[(i << 4) + j + (3 - k)] >> 4));
							if ((pBuffer[(i << 4) + j + (3 - k)] & 0x0f) >= 0x0a)
								sstr << (char)('a' + ((pBuffer[(i << 4) + j + (3 - k)] & 0x0f) - 10));
							else
								sstr << (char)('0' + (pBuffer[(i << 4) + j + (3 - k)] & 0x0f));
						}
						sstr << ' ';
					}
					else
						sstr << "???????? ";
				}
			}
			else {
				for (size_t j = 0; j < 16; j += 8) {
					if (pScanValid) {
						for (size_t k = 0; k < 8; k++) {
							if (pBuffer[(i << 4) + j + (7 - k)] >= 0xa0)
								sstr << (char)('a' + ((pBuffer[(i << 4) + j + (7 - k)] >> 4) - 10));
							else
								sstr << (char)('0' + (pBuffer[(i << 4) + j + (7 - k)] >> 4));
							if ((pBuffer[(i << 4) + j + (7 - k)] & 0x0f) >= 0x0a)
								sstr << (char)('a' + ((pBuffer[(i << 4) + j + (7 - k)] & 0x0f) - 10));
							else
								sstr << (char)('0' + (pBuffer[(i << 4) + j + (7 - k)] & 0x0f));
						}
						sstr << ' ';
					}
					else
						sstr << "???????????????? ";
				}
			}
			sstr << "| ";

			/* add the characters */
			for (size_t j = 0; j < 16; j++) {
				if (pScanValid && pBuffer[(i << 4) + j] >= 0x20 && pBuffer[(i << 4) + j] != 0x7f)
					sstr << pBuffer[(i << 4) + j];
				else
					sstr << '.';
			}
			sstr << std::endl;
		}
		layout.header.append("\n").append(sstr.str());
	}
	else {
		/* create the header */
		std::stringstream sstr;
		size_t entriesPerLine = (8 / (instance->dumpType->size() > 8 ? 8 : instance->dumpType->size()));
		size_t charsPerEntry = 48 / entriesPerLine;
		std::string str = instance->dumpType->name();
		if (str.size() > 10)
			str.resize(10);
		else {
			while (str.size() < 10)
				str.append(" ");
		}
		sstr << str << "|";
		for (size_t i = 0; i < entriesPerLine; i++) {
			sstr << "+" << std::dec << i * instance->dumpType->size();
			while (sstr.str().size() < 9 + (i + 1) * (charsPerEntry + 1))
				sstr << " ";
			sstr << (i + 1 == entriesPerLine ? "\n" : "|");
		}
		sstr << "----------";
		for (size_t i = 0; i < (charsPerEntry + 1) * entriesPerLine; i++) {
			if (i % (charsPerEntry + 1) == 0)
				sstr << "+";
			else
				sstr << "-";
		}
		sstr << std::endl;

		/* write the memory to the screen */
		for (size_t i = 0; i < 256; i += entriesPerLine * instance->dumpType->size()) {
			/* create the static-data */
			uint64_t curSize = sstr.str().size();
			if (instance->moduleOffset != (uint64_t)-1) {
				if (i + instance->dumpAddress >= instance->staticSectionStart && i + instance->dumpAddress < instance->staticSectionEnd)
					sstr << "s ";
				else
					sstr << "v ";
			}

			/* add the row-header */
			sstr << "+" << std::dec << i;
			while (sstr.str().size() - curSize < 10)
				sstr << " ";
			sstr << "|";

			/* add the values */
			for (size_t j = 0; j < entriesPerLine; j++) {
				str = "";

				/* create the string */
				if (!pScanValid || !instance->dumpType->validate(pBuffer + i + j * instance->dumpType->size()))
					str = "?";
				else
					str = instance->dumpType->toString(pBuffer + i + j * instance->dumpType->size());

				/* clip the string and add it to the result */
				if (str.size() > charsPerEntry)
					str.resize(charsPerEntry);
				else {
					while (str.size() < charsPerEntry)
						str.append(" ");
				}
				sstr << str << (j + 1 == entriesPerLine ? "\n" : " ");
			}
		}
		layout.header.append("\n").append(sstr.str());

		/* add the menu */
		layout.add(MenuInstance::Default::custom + 10, "add to entries");
	}

	/* reset the refresh-data */
	pStamp = GetTickCount64();
	pRefresh = false;
	return layout;
}
menu::Behavior DumpPage::evaluate(menu::EntryId selected) {
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* setup the default-pageeval */
	menu::Behavior behavior("", menu::Traverse::stay);
	if (instance->eval(selected, &behavior))
		return behavior;

	/* handle the selection */
	if (selected == MenuInstance::Default::custom + 1) {
		uint64_t addr;
		if (!menu::Page::host()->getNumber("enter a new address", addr, true))
			behavior.response = "invalid address!";
		else {
			instance->dumpAddress = addr;
			behavior.response = "address successfully changed!";
		}
	}
	else if (selected == MenuInstance::Default::custom + 2) {
		uint64_t offset;
		if (!menu::Page::host()->getNumber("enter the offset", offset, false))
			behavior.response = "invalid offset!";
		else {
			behavior.response = "address successfully decreased!";
			instance->dumpAddress -= offset;
		}
	}
	else if (selected == MenuInstance::Default::custom + 3) {
		behavior.response = "address successfully decreased!";
		instance->dumpAddress -= 256;
	}
	else if (selected == MenuInstance::Default::custom + 4) {
		behavior.response = "address successfully decreased!";
		instance->dumpAddress -= 128;
	}
	else if (selected == MenuInstance::Default::custom + 5) {
		behavior.response = "address successfully increased!";
		instance->dumpAddress += 128;
	}
	else if (selected == MenuInstance::Default::custom + 6) {
		behavior.response = "address successfully increased!";
		instance->dumpAddress += 256;
	}
	else if (selected == MenuInstance::Default::custom + 7) {
		uint64_t offset;
		if (!menu::Page::host()->getNumber("enter the offset", offset, false))
			behavior.response = "invalid offset!";
		else {
			behavior.response = "address successfully increased!";
			instance->dumpAddress += offset;
		}
	}
	else if (selected == MenuInstance::Default::custom + 8) {
		behavior.traverse = menu::Traverse::push;
		behavior.target = DumptypePage::Id;
	}
	else if (selected == MenuInstance::Default::custom + 9) {
		uint64_t offset;
		if (!menu::Page::host()->getNumber("enter the offset in the table", offset, false))
			behavior.response = "invalid offset!";
		else {
			if (offset >= 256)
				behavior.response = "offset out of range!";
			else {
				/* align the index */
				offset -= (offset % instance->dumpType->size());

				/* receive the new value to write */
				uint8_t* buffer = (uint8_t*)malloc(instance->dumpType->size());
				if (!instance->dumpType->readInput(buffer, false))
					behavior.response = "invalid value!";
				else {
					/* try to write the value to memory */
					if (WriteProcessMemory(instance->process, (void*)(instance->dumpAddress + offset), buffer, instance->dumpType->size(), 0))
						behavior.response = "value written!";
					else
						behavior.response = "writing failed!";
				}
				free(buffer);
			}
		}
	}
	else if (selected == MenuInstance::Default::custom + 10) {
		uint64_t offset;
		if (!menu::Page::host()->getNumber("enter the offset in the table", offset, false))
			behavior.response = "invalid offset!";
		else {
			if (offset >= 256)
				behavior.response = "offset out of range!";
			else {
				/* align the index */
				offset -= (offset % instance->dumpType->size());

				/* add the entry */
				instance->entry(instance->dumpAddress + offset, instance->dumpType, pBuffer + offset);
				behavior.response = "entry successfully added to list!";
			}
		}
	}
	else {
		/* get the filepath */
		std::cout << "enter the filepath: ";
		std::string path;
		std::getline(std::cin, path);
		if (path.empty())
			behavior.response = "invalid path!";
		else {
			/* create and open the file */
			std::ofstream file = std::ofstream(path, std::ios::out | std::ios::trunc);
			if (!file.is_open())
				behavior.response = "failed to open the file!";
			else {

				/* read the memory */
				uint8_t* buffer = (uint8_t*)malloc(instance->staticSectionEnd - instance->staticSectionStart);
				if (ReadProcessMemory(instance->process, (void*)instance->staticSectionStart, buffer, instance->staticSectionEnd - instance->staticSectionStart, 0) == 0) {
					behavior.response = "failed to read the memory!";
				}
				else {
					/* print the data to the file */
					uint64_t typeSize = (instance->dumpType == 0 ? (instance->dumpTypeSpecific == 0 ? 1 : (instance->dumpTypeSpecific == 1 ? 4 : 8)) : instance->dumpType->size());
					uint64_t limit = instance->staticSectionEnd - instance->staticSectionStart - typeSize;
					for (uint64_t i = 0; i <= limit; i += typeSize) {
						if ((i & 0x1f) == 0) {
							if (i)
								file << std::endl;
							file << "[module+0x" << (void*)(i + instance->staticSectionStart - instance->moduleOffset) << "] - ";
						}
						std::stringstream sstr;
						if (instance->dumpType != 0) {
							if (!instance->dumpType->validate(buffer + i))
								sstr << "?";
							else
								sstr << instance->dumpType->toString(buffer + i);
							if (std::string str = sstr.str(); str.size() > 12) {
								str.resize(12);
								sstr.str(str);
								sstr.clear();
							}
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

				/* clsoe the resources */
				free(buffer);
				file.close();
				behavior.response = "dump successfully written!";
			}
		}
	}

	return behavior;
}
void DumpPage::teardown() {
	delete this;
}



/* implement the dumptype page */
DumptypePage* DumptypePage::acquire() {
	return new DumptypePage(DumptypePage::Id, DumptypePage::Title);
}
menu::Layout DumptypePage::construct() {
	/* extract the local & global data */
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* create the page-object */
	menu::Layout layout = instance->layout(Id, false);

	/* add the default-entry */
	layout.add(MenuInstance::Default::custom + 1, "hex-dump");
	layout.add(MenuInstance::Default::custom + 2, "Ptr32");
	layout.add(MenuInstance::Default::custom + 3, "Ptr64");

	/* add the datatypes */
	for (size_t i = 0; i < instance->types.size(); i++) {
		/* check if the type is valid */
		if (instance->types[i]->align() == instance->types[i]->size()) {
			/* check if the size is a multiple of two */
			bool isMultiple = false;
			for (size_t j = 1; j <= 64; j <<= 1) {
				if (instance->types[i]->size() == j) {
					isMultiple = true;
					break;
				}
			}

			/* add the type */
			if (isMultiple)
				layout.add(MenuInstance::Default::custom + 4 + i, std::string("datatype: ").append(instance->types[i]->name()));
		}
	}

	return layout;
}
menu::Behavior DumptypePage::evaluate(menu::EntryId selected) {
	/* extract the local & global data */
	MenuInstance* instance = (MenuInstance*)menu::Page::instance();

	/* setup the default-pageeval */
	menu::Behavior behavior("", menu::Traverse::pop);
	if (instance->eval(selected, &behavior))
		return behavior;

	/* handle the selection */
	if (selected == MenuInstance::Default::custom + 1) {
		instance->dumpType = 0;
		instance->dumpTypeSpecific = 0;
	}
	else if (selected == MenuInstance::Default::custom + 2) {
		instance->dumpType = 0;
		instance->dumpTypeSpecific = 1;
	}
	else if (selected == MenuInstance::Default::custom + 3) {
		instance->dumpType = 0;
		instance->dumpTypeSpecific = 2;
	}
	else
		instance->dumpType = instance->types[selected - MenuInstance::Default::custom - 4];
	behavior.response = "datatype successfully changed!";
	return behavior;
}
void DumptypePage::teardown() {
	delete this;
}