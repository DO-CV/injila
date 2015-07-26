#include <QDebug>
#include <QGraphicsItem>

#include "DO/Injila/Utilities/Logging.hpp"

#include "DO/Injila/ImageAnnotator/Views/GraphicsItems.hpp"
#include "DO/Injila/ImageAnnotator/Views/LabelEditor.hpp"


using namespace std;


namespace DO { namespace Injila {

  LabelEditor::LabelEditor(QWidget *parent)
    : QTableWidget(parent)
  {
    setMouseTracking(true);
    setColumnCount(1);

    connect(this, SIGNAL(itemChanged(QTableWidgetItem *)),
            this, SLOT(notifyUpdatedLabels(QTableWidgetItem *)));
    connect(this, SIGNAL(itemChanged(QTableWidgetItem *)),
            this, SLOT(updateSelectedLabels(QTableWidgetItem *)));
  }

  void LabelEditor::clearTable()
  {
    clear();
    setRowCount(0);
  }

  void LabelEditor::setSegments(const QList<QGraphicsItem *>& segments)
  {
    // Delete all the table cells
    clearTable();

    // Repopulate the label editor.
    setRowCount(segments.size());
    for (int s = 0; s < segments.size(); ++s)
    {
      auto *segment = qgraphicsitem_cast<PolygonItem *>(segments[s]);
      if (!segment)
        qFatal("Error: invalid segment!");
      setItem(segment->id(), 0, new QTableWidgetItem(""));

      auto *labelItem = item(segment->id(), 0);
      labelItem->setFlags(
        Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
      segment->setLabelItem(labelItem);
    }
  }

  void LabelEditor::updateLabels(QList<QStringList> labels)
  {
    qDebug() << "Updating labels";
    for (size_t l = 0; l != labels.size(); ++l)
      item(l, 0)->setText(labels.at(l).join(", "));
  }

  void LabelEditor::notifyUpdatedLabels(QTableWidgetItem *updatedItem)
  {
    auto row = updatedItem->row();
    auto labels = updatedItem->text();

    INJILA_LOG() << "Update segment labels" << row << ":" << labels;
    emit labelsUpdated(row, std::move(labels));
  }

  void LabelEditor::updateSelectedLabels(QTableWidgetItem *updatedItem)
  {
    auto selection = selectedItems();
    for (auto item : selection)
    {
      if (item == updatedItem)
        continue;
      item->setText(updatedItem->text());
    }
  }

} /* namespace Injila */
} /* namespace DO */
