#include <memory>

#include <QApplication>
#include <QtGui>

#include "DO/Injila/ImageAnnotator/Views/MainWindow.hpp"


using namespace DO::Sara;
using namespace DO::Injila;


int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  std::unique_ptr<AnnotatorMainWindow> annotatorMainWindow {
    new AnnotatorMainWindow{}
  };
  annotatorMainWindow->showMaximized();

  return app.exec();
}
