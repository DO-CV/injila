#pragma once

#include <QList>
#include <QTableWidget>

#include "DO/Injila/ImageAnnotator/Models/Segmentation.hpp"


class QGraphicsItem;


namespace DO { namespace Injila {

  class LabelEditor : public QTableWidget
  {
    Q_OBJECT

  public:
    LabelEditor(QWidget *parent = 0);

  signals:
    void labelsUpdated(int labelsIndex, QString labels);

  public slots:
    void clearTable();
    void setSegments(const QList<QGraphicsItem *>&);
    void updateLabels(QList<QStringList> labels);

  private slots:
    void notifyUpdatedLabels(QTableWidgetItem *updatedItem);
    void updateSelectedLabels(QTableWidgetItem *updatedItem);
  };

} /* namespace Injila */
} /* namespace DO */
