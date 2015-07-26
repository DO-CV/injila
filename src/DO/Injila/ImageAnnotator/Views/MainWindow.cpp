#include <algorithm>
#include <string>

#include <QtGui>
#include <QtNetwork>
#include <QtWidgets>

#include <DO/Sara/ImageIO.hpp>
#include <DO/Sara/ImageProcessing.hpp>

#include "DO/Injila/FileSystem/FileSystem.hpp"

#include "DO/Injila/KMeans/KMeans.hpp"

#include "DO/Injila/Superpixel/ERSInterface.hpp"

#include "DO/Injila/ImageAnnotator/Utilities.hpp"

#include "DO/Injila/ImageAnnotator/Controllers/AnnotationController.hpp"

#include "DO/Injila/ImageAnnotator/Views/AnnotationView.hpp"
#include "DO/Injila/ImageAnnotator/Views/GraphicsItems.hpp"
#include "DO/Injila/ImageAnnotator/Views/LabelEditor.hpp"
#include "DO/Injila/ImageAnnotator/Views/MainWindow.hpp"
#include "DO/Injila/ImageAnnotator/Views/FolderView.hpp"


using namespace std;
using namespace DO::Sara;


namespace DO { namespace Injila {

  AnnotatorMainWindow::AnnotatorMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_annotation{ new Annotation{} }
  {
    m_annotationController = new AnnotationController{
      m_annotation.get(), this
    };

    createDockableFileBrowser();
    createCentralGraphicsView();
    createDockableLabelEditor();
    createFileActions();
    createViewActions();
    createAlgorithmActions();
    createHelpActions();
    createConnections();

    resize(800, 600);
    setWindowTitle(tr("Injila Video Annotator"));
    statusBar()->showMessage(tr("Ready"));
  }

  void AnnotatorMainWindow::about()
  {
    QMessageBox::about(
      this, tr("About Injila Video Annotator"),
      tr("<p>The <b>Injila Video Annotator</b> is a small program that aims at "
         "quickly annotating videos.</p>"

         "<p>Wait and see: more features will be added on the fly...</p>")
    );
  }

  void AnnotatorMainWindow::createDockableFileBrowser()
  {
    // Dock file browser
    auto browserDock = new QDockWidget{
      tr("Image File Browser"), this
    };
    addDockWidget(Qt::LeftDockWidgetArea, browserDock);

    m_projectTreeView = new ProjectTreeView{ this };
    browserDock->setWidget(m_projectTreeView);
  }

  void AnnotatorMainWindow::createCentralGraphicsView()
  {
    m_annotationView = new AnnotationView{ this };
    setCentralWidget(m_annotationView);
  }

  void AnnotatorMainWindow::createDockableLabelEditor()
  {
    auto labelEditorDock = new QDockWidget{
      tr("SuperPixel Label Editor"), this
    };
    addDockWidget(Qt::RightDockWidgetArea, labelEditorDock);

    m_labelEditor = new LabelEditor;
    labelEditorDock->setWidget(m_labelEditor);
  }

  void AnnotatorMainWindow::createFileActions()
  {
    auto fileMenu = new QMenu{ tr("&File"), this };

    // Open image.
    auto openImageAct = new QAction{ tr("Open image..."), this };
    openImageAct->setShortcut(tr("Ctrl+O"));
    connect(openImageAct, SIGNAL(triggered()), this, SLOT(openImage()));
    fileMenu->addAction(openImageAct);

    // Open image folder.
    auto openFolderAct = new QAction{ tr("Open folder..."), this };
    openFolderAct->setShortcut(tr("Ctrl+Shift+O"));
    connect(openFolderAct, SIGNAL(triggered()), this, SLOT(openFolder()));
    fileMenu->addAction(openFolderAct);

    // Save annotation.
    auto saveAnnotationAct = new QAction{ tr("Save annotation"), this };
    saveAnnotationAct->setShortcut(tr("Ctrl+S"));
    connect(saveAnnotationAct, SIGNAL(triggered()),
            m_annotationController, SLOT(saveAnnotation()));
    fileMenu->addAction(saveAnnotationAct);

    // Separator.
    fileMenu->addSeparator();

    // Quit.
    auto quitAct = new QAction(tr("Quit..."), this);
    quitAct->setShortcut(tr("Alt+F4"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
    fileMenu->addAction(quitAct);

    // Register menus
    menuBar()->addMenu(fileMenu);
  }

  void AnnotatorMainWindow::createViewActions()
  {
    auto viewMenu = new QMenu{ tr("View"), this };
    auto toolBar = new QToolBar{ tr("Visualization Tool Bar"), this };

    auto toggleSegmentationDisplayAct = new QAction{
      tr("Show/Hide Segmentation"), this
    };
    toggleSegmentationDisplayAct->setShortcut(tr("Ctrl+W"));
    connect(toggleSegmentationDisplayAct, SIGNAL(triggered()),
            m_annotationView, SLOT(toggleSegmentationDisplay()));
    viewMenu->addAction(toggleSegmentationDisplayAct);
    toolBar->addAction(toggleSegmentationDisplayAct);

    menuBar()->addMenu(viewMenu);
    addToolBar(toolBar);
  }

  void AnnotatorMainWindow::createAlgorithmActions()
  {
    auto algorithmMenu = new QMenu{ tr("&Algorithms"), this };

    auto runEntropyRateSegmentationAct = new QAction{
      tr("&Entropy Rate Segmentation"), this
    };
    connect(runEntropyRateSegmentationAct, SIGNAL(triggered()),
            m_annotationController, SLOT(updateSegmentation()));
    runEntropyRateSegmentationAct->setShortcut(tr("Ctrl+E"));
    algorithmMenu->addAction(runEntropyRateSegmentationAct);

    menuBar()->addMenu(algorithmMenu);
  }

  void AnnotatorMainWindow::createHelpActions()
  {
    auto helpMenu = new QMenu{ tr("Help"), this };

    // About act.
    auto aboutAct = new QAction{ tr("About"), this };
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
    helpMenu->addAction(aboutAct);

    // About Qt act.
    auto aboutQtAct = new QAction{ tr("About Qt"), this };
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    helpMenu->addAction(aboutQtAct);

    // Register menu
    menuBar()->addMenu(helpMenu);
  }

  void AnnotatorMainWindow::createConnections()
  {
    // Signals from AnnotatorMainWindow.
    connect(this, SIGNAL(imageRead(const Image<Rgb8>&)),
            m_annotationController, SLOT(updateImage(const Image<Rgb8>&)));
    connect(this, SIGNAL(annotationOutPathRead(const QString&,
                                               const QString&)),
            m_annotationController,SLOT(loadAnnotation(const QString&,
                                                       const QString&)));

    // Signals from AnnotatorController.
    connect(m_annotationController, SIGNAL(imageUpdated(const QImage&)),
            m_annotationView, SLOT(setImage(const QImage&)));
    connect(m_annotationController, SIGNAL(segmentationUpdated(const Segmentation&)),
            m_annotationView, SLOT(setSegmentation(const Segmentation&)));
    connect(m_annotationController, SIGNAL(imageUpdated(const QImage&)),
            m_labelEditor, SLOT(clearTable()));
    connect(m_annotationController, SIGNAL(labelsUpdated(QList<QStringList>)),
            m_labelEditor, SLOT(updateLabels(QList<QStringList>)));

    // Signals from AnnotationView.
    connect(m_annotationView, SIGNAL(segmentsUpdated(const QList<QGraphicsItem *>&)),
            m_labelEditor, SLOT(setSegments(const QList<QGraphicsItem *>&)));

    // Signals from LabelEditor.
    connect(m_labelEditor, SIGNAL(cellPressed(int, int)),
            m_annotationView, SLOT(selectSuperpixel(int)));
    connect(m_labelEditor, SIGNAL(labelsUpdated(int, QString)),
            m_annotationController, SLOT(updateLabels(int, QString)));
  }

  void AnnotatorMainWindow::openImage()
  {
    // Load the image.
    auto imageFilePath = QFileDialog::getOpenFileName(
      this, tr("Open image..."), getDesktopPath(), "*.jpg *.png *.tiff");

    if (imageFilePath.isEmpty())
      return;

    Image<Rgb8> image;
    if (!imread(image, imageFilePath.toStdString()))
    {
      statusBar()->showMessage("Cannot read image");
      return;
    }
    emit imageRead(image);

    auto imageFilename = imageFilePath.mid(
      imageFilePath.lastIndexOf("/") + 1);
    auto annotationOutFolder = imageFilePath.left(
      imageFilePath.lastIndexOf("/"));
    auto annotationOutBasename = imageFilename.left(
      imageFilename.lastIndexOf("."));
    emit annotationOutPathRead(annotationOutFolder, annotationOutBasename);
  }

  void AnnotatorMainWindow::openFolder()
  {
  }

} /* namespace Injila */
} /* namespace DO */
