#pragma once

#include <QTreeView>


namespace DO { namespace Injila {

  class ProjectTreeView : public QTreeView
  {
    Q_OBJECT

  public:
    ProjectTreeView(QWidget *parent = 0)
      : QTreeView{ parent }
    {
    }

  };

} /* namespace Injila */
} /* namespace DO */
