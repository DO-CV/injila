#pragma once

#include <QDebug>
#include <QImage>
#include <QStandardPaths>

#include <DO/Sara/Core.hpp>

#include "DO/Injila/Superpixel/ERSInterface.hpp"


namespace DO { namespace Injila {

  using namespace DO::Sara;

  inline
  QString getDesktopPath()
  {
    QStringList desktopDirPathsList{
      QStandardPaths::standardLocations(QStandardPaths::DesktopLocation)
    };
    return desktopDirPathsList.empty() ? QString() : desktopDirPathsList.front();
  }

  //! Wrap the image object as a QImage object.
  inline
  QImage toQImage(const Image<Rgb8>& image)
  {
    return QImage{
      reinterpret_cast<const unsigned char *>(image.data()),
        image.width(), image.height(), image.width() * 3,
        QImage::Format_RGB888
    };
  }

} /* namespace Injila */
} /* namespace DO */
