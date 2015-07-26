#pragma once

#include <QGraphicsView>

#include <DO/Sara/Core.hpp>

#include "DO/Injila/Superpixel/ERSInterface.hpp"

#include "DO/Injila/ImageAnnotator/Models/Annotation.hpp"


class QGraphicsPixmapItem;


namespace DO { namespace Injila {

  class AnnotationView : public QGraphicsView
  {
    Q_OBJECT

  public:
    AnnotationView(QWidget* parent = 0);

    QGraphicsPixmapItem *image()
    {
      return m_image;
    }

    QList<QGraphicsItem *>& segments()
    {
      return m_segments;
    }

  signals:
    //! @{
    //! Image-related signals.
    void gotoPreviousPosterFrame();
    void gotoNextPosterFrame();
    void play();
    void stop();
    //! @}

    void segmentsUpdated(const QList<QGraphicsItem *>&);

  public slots:
    //! @{
    //! Interaction with AnnotationController.
    void setImage(const QImage& image);
    void setSegmentation(const Segmentation& segmentation);
    void toggleSegmentationDisplay();
    //! @}

    //! @{
    //! Interaction with LabelEditor class.
    void selectSuperpixel(int id);
    //! @}

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
    //! @{
    //! View-related data member.
    QGraphicsPixmapItem *m_image;
    QList<QGraphicsItem *> m_segments;
    bool m_displaySegments;
    //! @}
  };

} /* namespace Injila */
} /* namespace DO */
