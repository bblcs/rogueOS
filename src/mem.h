#pragma once

#include "types.h"

void* memzero(void* dst, size_t n);
void* memmove(void* dst, const void* src, size_t len);
void* memcpy(void* dst, const void* src, size_t len);
