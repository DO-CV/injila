#pragma once

#include <QMainWindow>


class QTreeView;

class AnnotationWidget;
class FileBrowser;
class FileSystemModel;
class GraphicsAnnotator;
class LabelEditor;


class AnnotatorMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  AnnotatorMainWindow(QWidget *parent = 0);

private: /* utility methods to construct the main window. */
  void createCentralGraphicsView();
  void createDockableLabelEditor();
  void createFileActions();
  void createAlgorithmActions();
  void createHelpActions();
  void createConnections();

private slots:
  void openImage();
  void updateSelectedLabels(int changedCellRow, int changedCellColumn);
  void about();


private slots: /* algorithms*/
  void runNearestNeighbors();

private:
  QString m_currentFilePath;
  FileSystemModel *m_fileSystem;
  QTreeView *m_treeView;
  GraphicsAnnotator *m_annotator;
  LabelEditor *m_labelEditor;
};
