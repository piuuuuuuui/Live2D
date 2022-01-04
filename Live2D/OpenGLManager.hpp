#pragma once

#include <GL/glew.h>
#include <QOpenGLFunctions>

class OpenGLManager {
 public:
  inline static QOpenGLFunctions* get() {
    if (s_instance == nullptr) s_instance = new QOpenGLFunctions;
    return s_instance;
  }
  inline static void release() {
    if (s_instance != nullptr) delete s_instance;
    s_instance = nullptr;
  }

 private:
  inline OpenGLManager() {}
  inline ~OpenGLManager() {}
  inline static QOpenGLFunctions* s_instance = nullptr;
};