#pragma once

#include <GL/glew.h>

#include <CubismFramework.hpp>
#include <Math/CubismMatrix44.hpp>
#include <QtOpenGLWidgets/QOpenGLWidget>

class QAction;
class QAudioOutput;
class QMediaPlayer;
class QMenu;
class QMouseEvent;
class QSystemTrayIcon;
class QTimer;

class Allocator;
class Model;

class Live2DWidget : public QOpenGLWidget {
  Q_OBJECT

 public:
  Live2DWidget();
  ~Live2DWidget();

 protected:
  void paintGL();
  void mousePressEvent(QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);
  void mouseMoveEvent(QMouseEvent *);

 private:
  HWND _hWnd;

  int _screenWidth = 0;
  int _screenHeight = 0;
  float _mouseX = 0;
  float _mouseY = 0;

  QTimer *_timer;

  QMediaPlayer *_player = nullptr;
  QAudioOutput *_audioOutput = nullptr;

  QSystemTrayIcon *_trayIcon = nullptr;
  QMenu *_menu = nullptr;
  QAction *_actionVisible = nullptr;
  QAction *_actionClose = nullptr;
  QAction *_actionPlay = nullptr;

  Allocator *_cubismAllocator = nullptr;
  Csm::CubismFramework::Option *_cubismOption = nullptr;

  Model *_model = nullptr;
  Csm::CubismMatrix44 _viewMatrix;
  Csm::CubismMatrix44 _toScreenMatrix;

  bool res = false;
  bool leftPressed = false;
  bool rightPressed = false;
  float _distX = 0;
  float _distY = 0;
};
