#include "datatype.h"

/* implement the datatype interface */
Datatype::Datatype(uint8_t size, uint8_t align, const std::string& name, bool restricted) : pSize(size), pAlign(align), pName(name), pRestricted(restricted) {}