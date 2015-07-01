#pragma once

#include <QGraphicsView>

#include <DO/Sara/Core.hpp>

#include "superpixel/ERSInterface.hpp"


class QGraphicsPixmapItem;

class Point;
class Line;
class Quad;

class GraphicsAnnotator : public QGraphicsView
{
  Q_OBJECT

public:
  GraphicsAnnotator(QWidget* parent = 0);

  void setPixmap(const QPixmap& pixmap);
  void runSuperPixelSegmentation();

  DO::Sara::SuperPixelGraph& superpixelGraph() { return m_superpixelGraph; }
  QList<QGraphicsItem *>& superpixelItems() { return m_superpixelPolygons; }

public slots:
  void selectSuperpixel(int id);

private:
  // Zoom in/out the scene view.
  void scaleView(qreal scaleFactor);

protected:
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);
  void keyPressEvent(QKeyEvent *event);

private:
  QGraphicsPixmapItem *m_videoFrameItem;
  QList<QGraphicsItem *> m_superpixelPolygons;

  // Need to create a SuperPixelSegmenter functor.
  int m_numSuperpixels = 200;

  // Should be encapsulated in a SuperPixelSegmentation class.
  DO::Sara::SuperPixelGraph m_superpixelGraph;
  DO::Sara::Image<int> m_superpixelsMap;
};
