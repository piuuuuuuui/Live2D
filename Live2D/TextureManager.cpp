/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at
 * https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#define STBI_NO_STDIO
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION

#include "TextureManager.hpp"

#include <OpenGLManager.hpp>
#include <iostream>

#include "Utilities.hpp"
#include "stb_image.h"

GLuint TextureManager::CreateTextureFromPngFile(std::string filename) {
  if (_textures.find(filename) != _textures.cend()) {
    return _textures[filename];
  }
  GLuint textureId;
  int width, height, channels;
  unsigned int size;
  unsigned char* png;
  unsigned char* address = Utilities::LoadFileAsBytes(filename, &size);
  if (NULL == address) return 0;
  png = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(address),
                              static_cast<int>(size), &width, &height,
                              &channels, STBI_rgb_alpha);

  OpenGLManager::get()->glGenTextures(1, &textureId);
  OpenGLManager::get()->glBindTexture(GL_TEXTURE_2D, textureId);
  OpenGLManager::get()->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
                                     0, GL_RGBA, GL_UNSIGNED_BYTE, png);
  OpenGLManager::get()->glGenerateMipmap(GL_TEXTURE_2D);
  OpenGLManager::get()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                        GL_LINEAR_MIPMAP_LINEAR);
  OpenGLManager::get()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                                        GL_LINEAR);
  OpenGLManager::get()->glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(png);
  Utilities::ReleaseBytes(address);

  _textures[filename] = textureId;
  return textureId;
}