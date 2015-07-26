#pragma once

#include <QGraphicsItem>
#include <QList>


class QPainter;
class QTableWidgetItem;


namespace DO { namespace Injila {

  class PolygonItem : public QGraphicsPolygonItem
  {
  public:
    PolygonItem(const QPolygonF& polygon, int id = -1);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0) override;

    int id() const { return m_id; }
    QTableWidgetItem * labelItem() const { return m_labelItem; }
    bool selected() const { return m_selected; }

    void setId(int id);
    void setLabelItem(QTableWidgetItem *item);
    void setSelected(bool value) { m_selected = value; }
    void setLabelColor(const QColor& color, bool labeled = true)
    {
      m_labeled = labeled;
      m_labeledColor = color;
    }

  protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

  private:
    bool m_labeled { false };
    bool m_selected { false };
    bool m_hovered { false };
    QColor m_hoverColor { QColor(255, 255, 0) };
    QColor m_selectColor { QColor(0, 0, 255) };
    QColor m_contourColor { QColor(0, 0, 0) };
    QColor m_labeledColor { QColor(0, 0, 0) };
    int m_id { -1 };
    QTableWidgetItem *m_labelItem { nullptr };
  };

} /* namespace Injila */
} /* namespace DO */
