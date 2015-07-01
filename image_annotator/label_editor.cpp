#include "label_editor.hpp"


LabelEditor::LabelEditor(QWidget *parent)
  : QTableWidget(parent)
{
  setMouseTracking(true);
  setColumnCount(1);
}