#include <algorithm>
#include <string>

#include <QtGui>
#include <QtWidgets>

#include <DO/Sara/ImageIO.hpp>
#include <DO/Sara/ImageProcessing.hpp>

#include "kmeans/kmeans.hpp"
#include "superpixel/ERSInterface.hpp"

#include "graphics_annotator.hpp"
#include "graphics_items.hpp"
#include "label_editor.hpp"
#include "main_window.hpp"


using namespace std;
using namespace DO::Sara;


QString getDesktopPath()
{
  QStringList desktopDirPathsList{
    QStandardPaths::standardLocations(QStandardPaths::DesktopLocation)
  };
  return desktopDirPathsList.empty() ? QString() : desktopDirPathsList.front();
};


AnnotatorMainWindow::AnnotatorMainWindow(QWidget *parent)
  : QMainWindow(parent)
{
  createCentralGraphicsView();
  createDockableLabelEditor();
  createFileActions();
  createAlgorithmActions();
  createHelpActions();
  createConnections();

  resize(800, 600);
  setWindowTitle(tr("Injila Image Annotator"));
  statusBar()->showMessage(tr("Ready"));
}

void AnnotatorMainWindow::about()
{
  QMessageBox::about(
    this, tr("About Injila"),
    tr("<p>The <b>Injila Image Annotator</b> is a small program "
       "that aims at quickly annotating videos.\n"

       "Wait and see: more features will be added on the fly...</p>")
  );
}

void AnnotatorMainWindow::createCentralGraphicsView()
{
  m_annotator = new GraphicsAnnotator;
  m_annotator->setBackgroundRole(QPalette::Base);
  m_annotator->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  setCentralWidget(m_annotator);
}

void AnnotatorMainWindow::createDockableLabelEditor()
{
  QDockWidget *labelEditorDock {
    new QDockWidget(tr("SuperPixel Label Editor"), this)
  };
  addDockWidget(Qt::RightDockWidgetArea, labelEditorDock);

  m_labelEditor = new LabelEditor;
  labelEditorDock->setWidget(m_labelEditor);
}

void AnnotatorMainWindow::createFileActions()
{
  QMenu *fileMenu { new QMenu(tr("&File"), this) };

  // Open image.
  QAction *openImageAct { new QAction(tr("Open image..."), this) };
  openImageAct->setShortcut(tr("Ctrl+O"));
  connect(openImageAct, SIGNAL(triggered()), this, SLOT(openImage()));
  fileMenu->addAction(openImageAct);

  // Separator
  fileMenu->addSeparator();

  // Quit.
  QAction *quitAct { new QAction(tr("Quit..."), this) };
  quitAct->setShortcut(tr("Alt+F4"));
  connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
  fileMenu->addAction(quitAct);

  // Register menus
  menuBar()->addMenu(fileMenu);
}

void AnnotatorMainWindow::createAlgorithmActions()
{
  QMenu *algorithmMenu { new QMenu(tr("&Algorithms"), this) };

  QAction *nearestNeighborAct { new QAction{tr("&Nearest neighbor"), this} };
  connect(nearestNeighborAct, SIGNAL(triggered()), this, SLOT(runNearestNeighbors()));
  nearestNeighborAct->setShortcut(tr("Ctrl+R"));
  algorithmMenu->addAction(nearestNeighborAct);

  menuBar()->addMenu(algorithmMenu);
}

void AnnotatorMainWindow::createHelpActions()
{
  QMenu *helpMenu { new QMenu(tr("Help"), this) };
  // About act
  QAction *aboutAct = new QAction(tr("About"), this);
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
  helpMenu->addAction(aboutAct);
  // About Qt act
  QAction *aboutQtAct = new QAction(tr("About Qt"), this);
  connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
  helpMenu->addAction(aboutQtAct);

  // Register menu
  menuBar()->addMenu(helpMenu);
}

void AnnotatorMainWindow::createConnections()
{
  connect(m_labelEditor, SIGNAL(cellChanged(int, int)),
          this, SLOT(updateSelectedLabels(int, int)));
  connect(m_labelEditor, SIGNAL(cellPressed(int, int)),
          m_annotator, SLOT(selectSuperpixel(int)));
}

void AnnotatorMainWindow::openImage()
{
  // Load the image.
  QString imageFilePath {
    QFileDialog::getOpenFileName(
    this, tr("Open image..."), getDesktopPath(), "*.jpg *.png *.tiff")
  };

  // Resize the image if needed.
  string image_file_path { imageFilePath.toStdString() };
  Image<Rgb8> rgb_image;
  if (!imread(rgb_image, image_file_path))
    return;
  if (rgb_image.width() > 1000)
    rgb_image = reduce(rgb_image, rgb_image.width() / 1000.);

  QImage image {
    reinterpret_cast<unsigned char *>(rgb_image.data()),
    rgb_image.width(), rgb_image.height(), rgb_image.width() * 3,
    QImage::Format_RGB888
  };
  if (image.isNull())
    return;

  // Run the superpixel algorithm.
  m_annotator->setPixmap(QPixmap::fromImage(image));
  m_annotator->runSuperPixelSegmentation();

  // Update the label editor to display the labels.
  auto& superpixels = m_annotator->superpixelItems();

  // Delete all the table cells
  m_labelEditor->clear();
  m_labelEditor->setRowCount(0);

  // Repopulate the label editor.
  m_labelEditor->setRowCount(superpixels.size());
  for (int row = 0; row < superpixels.size(); ++row)
  {
    // Get the superpixel ID.
    const auto& superpixelPolygon = superpixels[row];
    PolygonItem *poly = qgraphicsitem_cast<PolygonItem *>(superpixelPolygon);
    if (!poly)
      throw std::runtime_error("Superpixel polygon is null!");
    int superpixelId = poly->id();

    // Create a new item for editing superpixel labels.
    m_labelEditor->setItem(superpixelId, 0, new QTableWidgetItem(""));
    m_labelEditor->item(superpixelId, 0)->setFlags(
      Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
    poly->setLabelItem(m_labelEditor->item(superpixelId, 0));

  }
}

void AnnotatorMainWindow::updateSelectedLabels(int changedCellRow, int changedCellColumn)
{
  const QString label {
    m_labelEditor->item(changedCellRow, changedCellColumn)->text()
  };

  QList<QTableWidgetItem *> selectedItems { m_labelEditor->selectedItems() };
  for (auto item : selectedItems)
    item->setText(label);
}

void AnnotatorMainWindow::runNearestNeighbors()
{
  using namespace std;
  using namespace DO::Sara;

  // Sanity check.
  if (m_annotator->superpixelItems().isEmpty())
    return;

  // Check there is at least one label.
  {
    bool contains_one_label_at_least { false };
    for (int row = 0; row < m_labelEditor->rowCount(); ++row)
    {
      if (!m_labelEditor->item(row, 0)->text().isEmpty())
      {
        contains_one_label_at_least = true;
        break;
      }
    }
    if (!contains_one_label_at_least)
    {
      QMessageBox::warning(
        this, "Check the labels",
        "The label editor does not contain any annotations!");
      return;
    }
  }

  // Get all the labels;
  vector<string> labels;
  {
    for (int row = 0; row < m_labelEditor->rowCount(); ++row)
    {
      QString label{ m_labelEditor->item(row, 0)->text() };
      if (!label.isEmpty())
        labels.push_back(label.toStdString());
    }
    sort(labels.begin(), labels.end());
    labels.resize(unique(labels.begin(), labels.end()) - labels.begin());
  }

  // Algorithm parameters
  const int num_bins{ 8 };
  const int feature_dimension{ int(pow(num_bins, 3)) };

  // Compute the color distributions for each superpixel.
  MatrixXd features {
    compute_color_distributions(m_annotator->superpixelGraph(), num_bins)
  };

  // Input data.
  MatrixXd means{ feature_dimension, labels.size() };
  VectorXi cluster_sizes{ static_cast<int>(labels.size()) };

  // Compute the means.
  means.setZero();
  cluster_sizes.setZero();

  for (int row = 0; row < m_labelEditor->rowCount(); ++row)
  {
    auto label = m_labelEditor->item(row, 0)->text().toStdString();
    auto it = lower_bound(labels.begin(), labels.end(), label);
    if (it == labels.end() || *it != label)
      continue;

    int index = it - labels.begin();

    means.col(index) += features.col(row);
    ++cluster_sizes(index);
  }
  means.array().rowwise() /= cluster_sizes.transpose().array().cast<double>();

  // Output data;
  VectorXi assignment{ features.cols() };
  for (int f = 0; f < features.cols(); ++f)
    assignment(f) = nearest_neighbors(features.col(f), means);

  QVector<QColor> label_colors(labels.size());
  for (auto& color : label_colors)
  {
    auto r = rand() % 255;
    auto g = rand() % 255;
    auto b = rand() % 255;
    color = QColor(r, g, b);
  }

  // Update the label in the table editor.
  for (int row = 0; row < m_labelEditor->rowCount(); ++row)
  {
    auto label = QString::fromStdString(labels[assignment(row)]);
    m_labelEditor->item(row, 0)->setText(label);
  }

  // Update the color of the superpixel polygon.
  auto& superpixels = m_annotator->superpixelItems();
  for (auto superpixel : superpixels)
  {
    auto poly = qgraphicsitem_cast<PolygonItem *>(superpixel);
    auto id = poly->id();
    QString label { QString::fromStdString(labels[assignment(id)]) };
    poly->setLabelColor(label_colors[assignment(id)]);
  }
}
