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
#include <ICubismModelSetting.hpp>
#include <Model/CubismUserModel.hpp>
#include <QOpenGLFunctions>
#include <Rendering/OpenGL/CubismOffscreenSurface_OpenGLES2.hpp>
#include <Type/csmRectF.hpp>

#include "WavFileHandler.hpp"

class Model : public Csm::CubismUserModel {
 public:
  Model();
  virtual ~Model();
  void LoadAssets(const Csm::csmChar* dir, const Csm::csmChar* fileName);
  void ReloadRenderer();
  void Update();
  void Draw(const Csm::CubismMatrix44& matrix);
  void Play(const Csm::csmString& filePath);
  Csm::CubismMotionQueueEntryHandle StartMotion(
      const Csm::csmChar* group, Csm::csmInt32 no, Csm::csmInt32 priority,
      Csm::ACubismMotion::FinishedMotionCallback onFinishedMotionHandler =
          nullptr);
  Csm::CubismMotionQueueEntryHandle StartRandomMotion(
      const Csm::csmChar* group, Csm::csmInt32 priority,
      Csm::ACubismMotion::FinishedMotionCallback onFinishedMotionHandler =
          nullptr);
  void SetExpression(const Csm::csmChar* expressionID);
  void SetRandomExpression();
  virtual Csm::csmBool MouseTest(Csm::csmFloat32 x, Csm::csmFloat32 y);
  virtual Csm::csmBool HitTest(const Csm::csmChar* hitAreaName,
                               Csm::csmFloat32 x, Csm::csmFloat32 y);
  Csm::Rendering::CubismOffscreenFrame_OpenGLES2& GetRenderBuffer();

 private:
  void SetupModel(Csm::ICubismModelSetting* setting);
  void SetupTextures();
  void PreloadMotionGroup(const Csm::csmChar* group);
  void ReleaseMotionGroup(const Csm::csmChar* group) const;
  void ReleaseMotions();
  void ReleaseExpressions();

  Csm::ICubismModelSetting* _modelSetting;
  Csm::csmString _modelHomeDir;
  Csm::csmFloat32 _userTimeSeconds;
  Csm::csmVector<Csm::CubismIdHandle> _eyeBlinkIds;
  Csm::csmVector<Csm::CubismIdHandle> _lipSyncIds;
  Csm::csmMap<Csm::csmString, Csm::ACubismMotion*> _motions;
  Csm::csmMap<Csm::csmString, Csm::ACubismMotion*> _expressions;
  Csm::csmVector<Csm::csmRectF> _hitArea;
  Csm::csmVector<Csm::csmRectF> _userArea;
  const Csm::CubismId* _idParamAngleX;
  const Csm::CubismId* _idParamAngleY;
  const Csm::CubismId* _idParamAngleZ;
  const Csm::CubismId* _idParamBodyAngleX;
  const Csm::CubismId* _idParamEyeBallX;
  const Csm::CubismId* _idParamEyeBallY;

  WavFileHandler _wavFileHandler;

  Csm::Rendering::CubismOffscreenFrame_OpenGLES2 _renderBuffer;
};