/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at
 * https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <GL/glew.h>

#include <map>
#include <string>

class TextureManager {
 public:
  static GLuint CreateTextureFromPngFile(std::string fileName);

 private:
  inline static std::map<std::string, GLuint> _textures;
};
