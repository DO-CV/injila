#pragma once

#include <memory>
#include <set>

#include <QMainWindow>

#include <DO/Sara/Core.hpp>

#include "DO/Injila/ImageAnnotator/Models/Annotation.hpp"


namespace DO { namespace Injila {

  using namespace DO::Sara;

  class AnnotationController;

  class AnnotationView;
  class LabelEditor;
  class ProjectTreeView;


  class AnnotatorMainWindow : public QMainWindow
  {
    Q_OBJECT

  public:
    AnnotatorMainWindow(QWidget *parent = 0);

  private: /* utility methods to construct the main window. */
    void createDockableFileBrowser();
    void createCentralGraphicsView();
    void createDockableLabelEditor();
    void createFileActions();
    void createViewActions();
    void createAlgorithmActions();
    void createHelpActions();
    void createToolBar();
    void createConnections();

  signals:
    void imageRead(const Image<Rgb8>& image);
    void annotationOutPathRead(const QString&, const QString&);

  private slots:
    void about();
    void openImage();
    void openFolder();

  private:
    //! @{
    //! Models.
    std::unique_ptr<Annotation> m_annotation;
    //! @}

    //! Controllers.
    AnnotationController *m_annotationController;
    //! @}

    //! @{
    //! Widgets.
    ProjectTreeView *m_projectTreeView;
    AnnotationView *m_annotationView;
    LabelEditor *m_labelEditor;
    //! @}
  };


} /* namespace Injila */
} /* namespace DO */
