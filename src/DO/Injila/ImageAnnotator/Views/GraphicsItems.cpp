#include <QtCore>
#include <QtWidgets>
#include <QTableWidget>

#include "DO/Injila/ImageAnnotator/Views/GraphicsItems.hpp"


namespace DO { namespace Injila {

  PolygonItem::PolygonItem(const QPolygonF& polygon, int id)
    : QGraphicsPolygonItem(polygon)
    , m_id(id)
  {
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);
    setZValue(1.);
  }

  void PolygonItem::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *,
                          QWidget *)
  {
    painter->setPen(m_contourColor);
    painter->setOpacity(0.5);
    if (m_hovered && !m_selected)
      painter->fillPath(shape(), m_hoverColor);
    else if (m_selected)
      painter->fillPath(shape(), m_selectColor);
    else if (m_labeled)
    {
      painter->setOpacity(0.8);
      painter->fillPath(shape(), m_labeledColor);
    }
    painter->drawPath(shape());
  }

  void PolygonItem::setId(int id)
  {
    m_id = id;
  }

  void PolygonItem::setLabelItem(QTableWidgetItem *item)
  {
    m_labelItem = item;
  }

  void PolygonItem::hoverEnterEvent(QGraphicsSceneHoverEvent *)
  {
    m_hovered = true;
    update();
  }

  void PolygonItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
  {
    m_hovered = false;
    update();
  }

  void PolygonItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
  {
    m_selected = !m_selected;

    QTableWidget *tableWidget = m_labelItem->tableWidget();
    if (m_labelItem && m_selected)
    {
      // Deselect all superpixels if the Ctrl key is not pressed.
      if (!(event->modifiers() && Qt::ControlModifier))
      {
        // Deselect the cells on the label editor.
        QList<QTableWidgetItem *> selectedItems { tableWidget->selectedItems() };
        for (auto selectedItem : selectedItems)
          if (selectedItem != m_labelItem)
            selectedItem->setSelected(false);

        // Deselect the superpixels polygons on the annotator widget.
        QList<QGraphicsItem *> items { scene()->items() };
        for (auto item : items)
        {
          PolygonItem *poly { qgraphicsitem_cast<PolygonItem *>(item) };
          if (poly && poly != this)
            poly->setSelected(false);
        }
      }

      // Focus on the concerned label.
      tableWidget->setFocus();
      tableWidget->setCurrentCell(m_id, 0, QItemSelectionModel::Select);
    }

    update();
    QGraphicsPolygonItem::mousePressEvent(event);
  }

} /* namespace Injila */
} /* namespace DO */
