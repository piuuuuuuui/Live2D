#pragma once
#include <GL/glew.h>
#include <CubismFramework.hpp>
#include <QSettings>
#include <iostream>

class Config {
 public:
  inline static const Csm::csmChar* ResourcesPath = "Resources";
  inline static const Csm::csmChar* ModelDir = "Jiaran";
  inline static const Csm::csmInt32 ModelDirSize =
      sizeof(ModelDir) / sizeof(Csm::csmChar*);
  inline static const Csm::csmInt32 Fps = 30;
  inline static const Csm::csmFloat32 ModelSize = 398.0;
  inline static Csm::csmFloat32 OffsetX = 0.8;
  inline static Csm::csmFloat32 OffsetY = -0.6;
  inline static const Csm::csmChar* MotionGroupIdle = "Idle";
  inline static const Csm::csmInt32 PriorityNone = 0;
  inline static const Csm::csmInt32 PriorityIdle = 1;
  inline static const Csm::csmInt32 PriorityNormal = 2;
  inline static const Csm::csmInt32 PriorityForce = 3;
  inline static const Csm::CubismFramework::Option::LogLevel
      CubismLoggingLevel = Csm::CubismFramework::Option::LogLevel_Verbose;
};