#include "datatype.h"

/* implement the datatype interface */
Datatype::Datatype(uint8_t size, uint8_t align, const std::string& name, bool restricted) : pSize(size), pAlign(align), pName(name), pRestricted(restricted) {}
uint8_t Datatype::size() const {
	return pSize;
}
uint8_t Datatype::align() const {
	return pAlign;
}
const std::string& Datatype::name() const {
	return pName;
}
bool Datatype::restricted() const {
	return pRestricted;
}