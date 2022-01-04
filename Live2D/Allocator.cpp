/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at
 * https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "Allocator.hpp"

using namespace Csm;

void* Allocator::Allocate(const csmSizeType size) { return malloc(size); }

void Allocator::Deallocate(void* memory) { free(memory); }

void* Allocator::AllocateAligned(const csmSizeType size,
                                 const csmUint32 alignment) {
  size_t offset, shift, alignedAddress;
  void* allocation;
  void** preamble;
  offset = alignment - 1 + sizeof(void*);
  allocation = Allocate(size + static_cast<csmUint32>(offset));
  alignedAddress = reinterpret_cast<size_t>(allocation) + sizeof(void*);
  shift = alignedAddress % alignment;
  if (shift) {
    alignedAddress += (alignment - shift);
  }
  preamble = reinterpret_cast<void**>(alignedAddress);
  preamble[-1] = allocation;
  return reinterpret_cast<void*>(alignedAddress);
}

void Allocator::DeallocateAligned(void* alignedMemory) {
  void** preamble;
  preamble = static_cast<void**>(alignedMemory);
  Deallocate(preamble[-1]);
}
