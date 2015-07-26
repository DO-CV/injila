#include <QtCore>
#include <QtOpenGL>

#include "DO/Injila/ImageAnnotator/Views/AnnotationView.hpp"
#include "DO/Injila/ImageAnnotator/Views/GraphicsItems.hpp"


using namespace std;


namespace DO { namespace Injila {

  AnnotationView::AnnotationView(QWidget* parent)
    : QGraphicsView(parent)
  {
    setBackgroundRole(QPalette::Base);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
    setTransformationAnchor(AnchorUnderMouse);
    setRenderHints(QPainter::Antialiasing);
    setDragMode(RubberBandDrag);

    setScene(new QGraphicsScene);

    m_image = new QGraphicsPixmapItem;
    scene()->addItem(m_image);

    m_displaySegments = true;
  }

  void AnnotationView::setImage(const QImage& image)
  {
    m_image->setPixmap(QPixmap::fromImage(image));
    setSceneRect(m_image->pixmap().rect());

    foreach(QGraphicsItem *polygon, m_segments)
      delete polygon;
    m_segments.clear();
  }

  void AnnotationView::setSegmentation(const Segmentation& segmentation)
  {
    foreach(QGraphicsItem *polygon, m_segments)
      delete polygon;

    m_segments.clear();

    const auto& segment_boundaries = segmentation.segment_boundaries();
    for (size_t id = 0; id < segment_boundaries.size(); ++id)
    {
      const auto& segment_boundary = segment_boundaries[id];
      QPolygonF segment_polygon;
      for (const auto& point : segment_boundary)
        segment_polygon << QPointF(point.x(), point.y());
      m_segments.push_back(new PolygonItem(segment_polygon, int(id)));
    }

    foreach(QGraphicsItem *segment, m_segments)
      scene()->addItem(segment);

    emit segmentsUpdated(m_segments);
  }

  void AnnotationView::toggleSegmentationDisplay()
  {
    m_displaySegments = !m_displaySegments;

    auto mainWindow = qobject_cast<QMainWindow *>(parent());
    if (m_displaySegments)
      mainWindow->statusBar()->showMessage(tr("Showing segmentation..."));
    else
      mainWindow->statusBar()->showMessage(tr("Hiding segmentation..."));

    for (auto *segment : m_segments)
      segment->setVisible(m_displaySegments);
  }

  void AnnotationView::selectSuperpixel(int id)
  {
    for (QGraphicsItem *segment : m_segments)
    {
      PolygonItem *s { qgraphicsitem_cast<PolygonItem *>(segment) };
      if (s && s->id() == id)
        s->setSelected(true);
      else
        s->setSelected(false);
      s->update();
    }
  }

  void AnnotationView::scaleView(qreal scaleFactor)
  {
    scale(scaleFactor, scaleFactor);
  }

  void AnnotationView::mousePressEvent(QMouseEvent *event)
  {
    QGraphicsView::mousePressEvent(event);
  }

  void AnnotationView::mouseReleaseEvent(QMouseEvent *event)
  {
    QGraphicsView::mouseReleaseEvent(event);
  }

  void AnnotationView::mouseMoveEvent(QMouseEvent *event)
  {
    QGraphicsView::mouseMoveEvent(event);
  }

  void AnnotationView::wheelEvent(QWheelEvent *event)
  {
    if (event->modifiers() == Qt::ControlModifier)
      scaleView(pow(double(2), event->delta() / 240.0));
    QGraphicsView::wheelEvent(event);
  }

  void AnnotationView::keyPressEvent(QKeyEvent *event)
  {
    if (event->key() == Qt::Key_F)
      fitInView(sceneRect(), Qt::KeepAspectRatio);

    if (event->key() == Qt::Key_Space)
      emit play();

    QGraphicsView::keyPressEvent(event);
  }

} /* namespace Injila */
} /* namespace DO */
