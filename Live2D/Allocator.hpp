/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at
 * https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <GL/glew.h>
#include <CubismFramework.hpp>
#include <ICubismAllocator.hpp>

class Allocator : public Csm::ICubismAllocator {
  void* Allocate(const Csm::csmSizeType size);
  void Deallocate(void* memory);
  void* AllocateAligned(const Csm::csmSizeType size,
                        const Csm::csmUint32 alignment);
  void DeallocateAligned(void* alignedMemory);
};
