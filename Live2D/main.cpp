#include <GL/glew.h>
#include <QtWidgets/QApplication>

#include "Live2DWidget.hpp"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  Live2DWidget w;
  return a.exec();
}
