#include "int-types.h"

/* implement the int type constructors */
types::Int<uint8_t>::Int() : Datatype(sizeof(uint8_t), sizeof(uint8_t), "uint8", false) {}
types::Int<int8_t>::Int() : Datatype(sizeof(int8_t), sizeof(int8_t), "int8", false) {}
types::Int<uint16_t>::Int() : Datatype(sizeof(uint16_t), sizeof(uint16_t), "uint16", false) {}
types::Int<int16_t>::Int() : Datatype(sizeof(int16_t), sizeof(int16_t), "int16", false) {}
types::Int<uint32_t>::Int() : Datatype(sizeof(uint32_t), sizeof(uint32_t), "uint32", false) {}
types::Int<int32_t>::Int() : Datatype(sizeof(int32_t), sizeof(int32_t), "int32", false) {}
types::Int<uint64_t>::Int() : Datatype(sizeof(uint64_t), sizeof(uint64_t), "uint64", false) {}
types::Int<int64_t>::Int() : Datatype(sizeof(int64_t), sizeof(int64_t), "int64", false) {}