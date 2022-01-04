/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at
 * https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "Utilities.hpp"

#include <stdarg.h>
#include <sys/stat.h>
#include <windows.h>

#include <Model/CubismMoc.hpp>
#include <cstdio>
#include <fstream>
#include <iostream>

#include "Config.hpp"

using std::endl;
using namespace Csm;
using namespace std;

double Utilities::s_currentFrame = 0.0;
double Utilities::s_lastFrame = 0.0;
double Utilities::s_deltaTime = 0.0;

csmByte* Utilities::LoadFileAsBytes(const string filePath, csmSizeInt* outSize) {
  const char* path = filePath.c_str();
  int size = 0;
  struct stat statBuf;
  if (stat(path, &statBuf) == 0) {
    size = statBuf.st_size;
  }
  std::fstream file;
  char* buf = new char[size];
  file.open(path, std::ios::in | std::ios::binary);
  if (!file.is_open()) {
    return NULL;
  }
  file.read(buf, size);
  file.close();
  *outSize = size;
  return reinterpret_cast<csmByte*>(buf);
}

void Utilities::ReleaseBytes(csmByte* byteData) { delete[] byteData; }

csmFloat32 Utilities::GetDeltaTime() {
  return static_cast<csmFloat32>(s_deltaTime);
}

void Utilities::UpdateTime() {
  s_currentFrame = clock();
  s_deltaTime = (s_currentFrame - s_lastFrame) / 1000.0;
  s_lastFrame = s_currentFrame;
}

void Utilities::PrintMessage(const csmChar* message) {
  std::cout << message << std::endl;
}
