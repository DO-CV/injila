#include <memory>

#include <QApplication>
#include <QtGui>

#include "main_window.hpp"


int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  std::unique_ptr<AnnotatorMainWindow> annotatorMainWindow {
    new AnnotatorMainWindow
  };
  annotatorMainWindow->showMaximized();

  return app.exec();
}
