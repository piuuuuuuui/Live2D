#include "Live2DWidget.hpp"

#include <QAction>
#include <QApplication>
#include <QAudioOutput>
#include <QCursor>
#include <QFileDialog>
#include <QMediaPlayer>
#include <QMenu>
#include <QMouseEvent>
#include <QScreen>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QWidget>

#include "Allocator.hpp"
#include "Config.hpp"
#include "Model.hpp"
#include "OpenGLManager.hpp"
#include "Utilities.hpp"

Live2DWidget::Live2DWidget() : QOpenGLWidget(nullptr), _hWnd((HWND)winId()) {
  // Window
  setWindowFlag(Qt::WindowType::MSWindowsOwnDC, false);
  setWindowFlag(Qt::FramelessWindowHint);
  setWindowFlag(Qt::Tool);
  setWindowFlag(Qt::WindowStaysOnTopHint);
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_QuitOnClose);
  setGeometry(QGuiApplication::primaryScreen()->availableGeometry());
  _screenWidth = width();
  _screenHeight = height();
  resizeGL(_screenWidth, _screenHeight);
  show();
  setMouseTracking(true);
  OpenGLManager::get()->initializeOpenGLFunctions();
  OpenGLManager::get()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                                        GL_LINEAR);
  OpenGLManager::get()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                        GL_LINEAR);
  OpenGLManager::get()->glEnable(GL_BLEND);
  OpenGLManager::get()->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  makeCurrent();

  // Update
  _timer = new QTimer(this);
  _timer->setInterval(1000 / Config::Fps);
  _timer->start();
  connect(_timer, &QTimer::timeout, this, [&] { update(); });

  // Menu
  _menu = new QMenu(this);

  _actionVisible = new QAction(this);
  _actionVisible->setText("Hide");
  connect(_actionVisible, &QAction::triggered, this, [&]() {
    if (isVisible()) {
      hide();
      _actionVisible->setText("Show");
    } else {
      show();
      _actionVisible->setText("Hide");
    }
  });
  _menu->addAction(_actionVisible);

  _actionClose = new QAction(this);
  _actionClose->setText("Close");
  connect(_actionClose, &QAction::triggered, this, [&]() { close(); });
  _menu->addAction(_actionClose);

  _actionPlay = new QAction(this);
  _actionPlay->setText("Play");
  connect(_actionPlay, &QAction::triggered, this, [&]() {
    QString filePath = QFileDialog::getOpenFileName();
    if (!filePath.isEmpty()) _model->Play(filePath.toLatin1().data());
  });
  _menu->addAction(_actionPlay);

  // Tray Icon
  _trayIcon = new QSystemTrayIcon(this);
  _trayIcon->setIcon(QIcon(QString(Config::ResourcesPath) + "/icon.png"));
  _trayIcon->setContextMenu(_menu);
  _trayIcon->show();

  // Cubism
  _cubismAllocator = new Allocator();
  _cubismOption = new Csm::CubismFramework::Option();
  _cubismOption->LogFunction = Utilities::PrintMessage;
  _cubismOption->LoggingLevel = Csm::CubismFramework::Option::LogLevel_Verbose;
  Csm::CubismFramework::StartUp(_cubismAllocator, _cubismOption);
  Csm::CubismFramework::Initialize();

  // Model
  std::string fileName = std::string(Config::ModelDir);
  std::string dir = std::string(Config::ResourcesPath) + '/' + fileName + '/';
  std::string jsonName = fileName + ".model3.json";
  if (_model) delete _model;
  _model = new Model();
  _model->LoadAssets(dir.c_str(), jsonName.c_str());
  _viewMatrix.Scale(Config::ModelSize / _screenWidth,
                    Config::ModelSize / _screenHeight);
  _viewMatrix.Translate(Config::OffsetX, Config::OffsetY);
  _toScreenMatrix.Scale(_screenWidth / 2, -_screenHeight / 2);
  _toScreenMatrix.Translate(_screenWidth / 2, _screenHeight / 2);
}

Live2DWidget::~Live2DWidget() {
  _trayIcon->deleteLater();
  _menu->deleteLater();
  _actionClose->deleteLater();
}

void Live2DWidget::paintGL() {
  if (NULL == _model) return;
  {
    _mouseX = _toScreenMatrix.InvertTransformX(QCursor::pos().x());
    _mouseY = _toScreenMatrix.InvertTransformY(QCursor::pos().y());
    _model->SetDragging(_mouseX - Config::OffsetX, _mouseY - Config::OffsetY);
    res = _model->MouseTest(_viewMatrix.InvertTransformX(_mouseX),
                            _viewMatrix.InvertTransformY(_mouseY));
    if (res)
      SetWindowLong(_hWnd, GWL_EXSTYLE,
                    GetWindowLongW(_hWnd, GWL_EXSTYLE) & (~WS_EX_TRANSPARENT));
    else
      SetWindowLong(_hWnd, GWL_EXSTYLE,
                    GetWindowLongW(_hWnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT);
  }
  Utilities::UpdateTime();
  _model->Update();
  _model->Draw(_viewMatrix);
}

void Live2DWidget::mousePressEvent(QMouseEvent* e) {
  if (!res) return;
  if (e->button() == Qt::LeftButton) {
    leftPressed = true;
    _distX = _mouseX - Config::OffsetX;
    _distY = _mouseY - Config::OffsetY;
  } else if (e->button() == Qt::RightButton) {
    rightPressed = true;
  }
}

void Live2DWidget::mouseReleaseEvent(QMouseEvent* e) {
  if (!res) return;
  if (e->button() == Qt::LeftButton) {
    leftPressed = false;
    float x = _viewMatrix.InvertTransformX(_mouseX);
    float y = _viewMatrix.InvertTransformY(_mouseY);
    if (_model->HitTest("Head", x, y)) _model->SetRandomExpression();
    if (_model->HitTest("Body", x, y))
      _model->StartRandomMotion("TapBody", Config::PriorityNormal);
  } else if (e->button() == Qt::RightButton) {
    rightPressed = false;
    _menu->exec(QCursor::pos());
  }
}

void Live2DWidget::mouseMoveEvent(QMouseEvent* e) {
  if (!leftPressed) return;
  _viewMatrix.Translate(Config::OffsetX = _mouseX - _distX,
                        Config::OffsetY = _mouseY - _distY);
}