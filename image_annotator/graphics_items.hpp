#pragma once

#include <QGraphicsItem>
#include <QList>


class QPainter;
class QTableWidgetItem;

class Line;
class Point;
class Quad;


class Point : public QGraphicsItem
{
public:
  enum { Type = UserType + 1 };
  int type() const { return Type; }

  Point(const QPointF& pos);
  Quad *quad() const { return qgraphicsitem_cast<Quad *>(parentItem()); }

  void addLine(Line *line);
  QList<Line *> lines() const;

  QRectF boundingRect() const;
  QPainterPath shape() const;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
  QVariant itemChange(GraphicsItemChange change, const QVariant &value);

  void mousePressEvent(QGraphicsSceneMouseEvent *event);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
  QList<Line *> lineList;
};

class Line : public QGraphicsItem
{
public:
  enum { Type = UserType + 2 };
  int type() const { return Type; }

  Line(Point *sourceNode, Point *destNode);

  Point *sourceNode() const;
  Point *destNode() const;

  void adjust();

protected:
  QRectF boundingRect() const;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
  Point *source, *dest;
  QPointF sourcePoint;
  QPointF destPoint;
};

class Quad : public QGraphicsPolygonItem
{
public:
  Quad(Point *points[4]);
  ~Quad();
  bool operator==(const Quad& quad) const;
  Point * const *points() const { return p; }
  void adjust();
  void print() const;

  enum { Type = UserType + 3 };
  int type() const { return Type; }

  QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
  Point *p[4];
};

class PixmapItem : public QGraphicsPixmapItem
{
public:
  PixmapItem();

protected:
  void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
};

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