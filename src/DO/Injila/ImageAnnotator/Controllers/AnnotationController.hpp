#pragma once

#include <set>

#include <QObject>
#include <QList>

#include <DO/Sara/Core.hpp>

#include "DO/Injila/ImageAnnotator/Models/Segmentation.hpp"


Q_DECLARE_METATYPE(std::vector<std::set<std::string>>)


class QGraphicsItem;


namespace DO { namespace Injila {

  using namespace DO::Sara;

  class Annotation;

  class AnnotationController : public QObject
  {
    Q_OBJECT

  public:
    //! \brief Constructor.
    AnnotationController(Annotation *annotation, QObject *parent = 0)
      : QObject{ parent }
      , m_annotation{ annotation }
    {
    }

    //! \brief Return annotation.
    Annotation * annotation() const
    {
      return m_annotation;
    }

  public slots:
    void loadAnnotation(const QString& annotationOutFolder,
                        const QString& annotationOutBasename);

    //! \brief Save annotation to disk and database.
    void saveAnnotation();

    //! \brief Update annotation data from GUI interaction.
    void updateImage(const Image<Rgb8>& image);
    void updateSegmentation();
    void updateLabels(int labelsIndex, QString labels);

  signals:
    void imageUpdated(const QImage& image);
    void segmentationUpdated(const Segmentation& segmentation);
    void labelsUpdated(QList<QStringList> segmentation);

  protected:
    //! \brief Model.
    Annotation *m_annotation;

    //! \brief Where to store annotations.
    QString m_annotationFolder;
    QString m_annotationBasename;

    //! \brief Segmentation parameters
    int m_numSuperpixels{ 200 };

    bool m_labelsUpdated;
    bool m_segmentationUpdated;
  };

} /* namespace Injila */
} /* namespace DO */
