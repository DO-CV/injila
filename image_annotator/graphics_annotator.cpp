#include <QtCore>
#include <QtOpenGL>

#include "graphics_annotator.hpp"
#include "graphics_items.hpp"


using namespace std;


namespace DO { namespace Sara {

  Image<Rgb64f> compute_mean_colors(const SuperPixelGraph& g, const Image<int>& labels)
  {
    std::vector<Rgb64f> mean_colors(g.num_vertices());
    for (unsigned i = 0; i < g.num_vertices(); ++i)
      mean_colors[i] = g.v[i].mean_color();

    Image<Rgb64f> superpixels_map(labels.sizes());
    for (int y = 0; y < superpixels_map.height(); ++y)
      for (int x = 0; x < superpixels_map.width(); ++x)
        superpixels_map(x, y) = mean_colors[labels(x, y)];
    return superpixels_map;
  }

  Image<Rgb64f> random_colorize_superpixels(const SuperPixelGraph& g, const Image<int>& labels)
  {
    std::vector<Rgb64f> random_colors(g.num_vertices());
    for (unsigned i = 0; i < g.num_vertices(); ++i)
    {
      random_colors[i](0) = (rand() * 255.f) / RAND_MAX;
      random_colors[i](1) = (rand() * 255.f) / RAND_MAX;
      random_colors[i](2) = (rand() * 255.f) / RAND_MAX;
    }

    Image<Rgb64f> superpixels_map(labels.sizes());
    for (int y = 0; y < superpixels_map.height(); ++y)
      for (int x = 0; x < superpixels_map.width(); ++x)
        superpixels_map(x, y) = random_colors[labels(x, y)];
    return superpixels_map;
  }

}
}


GraphicsAnnotator::GraphicsAnnotator(QWidget* parent)
  : QGraphicsView(parent)
{
  setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
  setTransformationAnchor(AnchorUnderMouse);
  setRenderHints(QPainter::Antialiasing);
  setDragMode(RubberBandDrag);

  setScene(new QGraphicsScene);

  m_videoFrameItem = new QGraphicsPixmapItem;
  scene()->addItem(m_videoFrameItem);
}

void GraphicsAnnotator::setPixmap(const QPixmap& pixmap)
{
  m_videoFrameItem->setPixmap(pixmap);
  setSceneRect(pixmap.rect());
}

void GraphicsAnnotator::runSuperPixelSegmentation()
{
  // Load the image.
  QPixmap pixmap { m_videoFrameItem->pixmap() };
  QImage qimage { pixmap.toImage().convertToFormat(QImage::Format_RGB32) };

  using namespace DO::Sara;

  Image<Rgb8> image { qimage.width(), qimage.height() };
  const int32_t *src = reinterpret_cast<const int* >(qimage.constBits());
  for (Rgb8 *dst = image.data(); dst != image.end(); ++dst, ++src)
  {
    (*dst)[0] = *src;
    (*dst)[1] = *src >> 8;
    (*dst)[2] = *src >> 16;
  }

  m_superpixelsMap = compute_superpixel_map(image, m_numSuperpixels);
  m_superpixelGraph = build_graph(m_numSuperpixels, m_superpixelsMap,
                                  image.convert<Rgb64f>());
  Image<Rgb8> superpixels_image {
    random_colorize_superpixels(m_superpixelGraph, m_superpixelsMap).
    convert<Rgb8>()
  };

  vector<vector<Point2i>> superpixels_inner_boundaries{
    trace_superpixels_inner_boundaries(m_superpixelGraph, m_superpixelsMap)
  };

  // Update the pixmap item.
  QImage q_superpixels_image (
    reinterpret_cast<const uchar *>(superpixels_image.data()),
    superpixels_image.width(), superpixels_image.height(), superpixels_image.width() * 3,
    QImage::Format_RGB888
  );

  foreach(QGraphicsItem *polygon, m_superpixelPolygons)
    delete polygon;
  m_superpixelPolygons.clear();
  for (size_t id = 0; id < superpixels_inner_boundaries.size(); ++id)
  {
    const auto& boundary = superpixels_inner_boundaries[id];
    QPolygonF polygon;
    for (const auto& point : boundary)
      polygon << QPointF(point.x(), point.y());
    m_superpixelPolygons.push_back(new PolygonItem(polygon, int(id)));
  }

  foreach(QGraphicsItem *polygon_item, m_superpixelPolygons)
    scene()->addItem(polygon_item);
}

void GraphicsAnnotator::selectSuperpixel(int id)
{
  for (QGraphicsItem *superpixel : m_superpixelPolygons)
  {
    PolygonItem *sp { qgraphicsitem_cast<PolygonItem *>(superpixel) };
    if (sp && sp->id() == id)
      sp->setSelected(true);
    else
      sp->setSelected(false);
    sp->update();
  }
}

void GraphicsAnnotator::scaleView(qreal scaleFactor)
{
  scale(scaleFactor, scaleFactor);
}

void GraphicsAnnotator::mousePressEvent(QMouseEvent *event)
{
  QGraphicsView::mousePressEvent(event);
}

void GraphicsAnnotator::mouseReleaseEvent(QMouseEvent *event)
{
  QGraphicsView::mouseReleaseEvent(event);
}

void GraphicsAnnotator::mouseMoveEvent(QMouseEvent *event)
{
  QGraphicsView::mouseMoveEvent(event);
}

void GraphicsAnnotator::wheelEvent(QWheelEvent *event)
{
  if (event->modifiers() == Qt::ControlModifier)
    scaleView(pow(double(2), event->delta() / 240.0));
  QGraphicsView::wheelEvent(event);
}

void GraphicsAnnotator::keyPressEvent(QKeyEvent *event)
{
  // Adjust view.
  if (event->key() == Qt::Key_F)
    fitInView(sceneRect(), Qt::KeepAspectRatio);

  QGraphicsView::keyPressEvent(event);
}
