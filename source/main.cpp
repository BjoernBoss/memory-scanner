/* SPDX-License-Identifier: BSD-3-Clause */
/* Copyright (c) 2021 Bjoern Boss Henrichsen */
#include <common/menu.h>

int main() {
	/* create the menu object */
	menu::Host* host = menu::Host::acquire(true);
	
	/* run the menu-instance */
	MenuInstance instance;
	host->run(&instance);

	/* release the menu */
	host->release();
	return 0;
}