#include <fstream>

#include <QtWidgets>

#include "DO/Injila/FileSystem/FileSystem.hpp"

#include "DO/Injila/Service/HttpRequests.hpp"
#include "DO/Injila/Service/SegmentationIO.hpp"

#include "DO/Injila/Utilities/Logging.hpp"

#include "DO/Injila/ImageAnnotator/Utilities.hpp"

#include "DO/Injila/ImageAnnotator/Models/Annotation.hpp"

#include "DO/Injila/ImageAnnotator/Controllers/AnnotationController.hpp"


using namespace std;


namespace DO { namespace Injila {

  void AnnotationController::updateLabels(int labelIndex, QString labels)
  {
    if (!labels.isEmpty())
    {
      auto labelsList = labels.split(",", QString::SkipEmptyParts);
      auto& labels = m_annotation->labels()[labelIndex];
      labels.clear();
      for (const auto& label : labelsList)
      {
        auto l = label.trimmed().toStdString();
        if (l.empty())
          continue;
        labels.insert(move(l));
      }
    }
  }

  void AnnotationController::loadAnnotation(const QString& annotationOutFolder,
                                            const QString& annotationOutBasename)
  {
    m_annotationFolder = annotationOutFolder;
    m_annotationBasename = annotationOutBasename;
    INJILA_LOG() << "Registered annotation folder:" << m_annotationFolder;
    INJILA_LOG() << "Registered annotation basename:" << m_annotationBasename;

    auto segmentationFilePath = QString{
      m_annotationFolder + "/" + m_annotationBasename + ".bin"
    }.toStdString();
    auto annotationIdFilePath = QString{
      m_annotationFolder + "/" + m_annotationBasename + ".json"
    }.toStdString();

    INJILA_LOG() << "Checking if" << segmentationFilePath.c_str() << "exists...";
    INJILA_LOG() << "Checking if" << annotationIdFilePath.c_str() << "exists...";

    // Load the segmentation if it exists.
    if (file_exists(segmentationFilePath))
    {
      INJILA_LOG() << "Loading segmentation map...";
      m_annotation->segmentation().map() =
        move(read_segmentation(segmentationFilePath));
      m_annotation->segmentation().updateFromMap(m_annotation->image());
    }
    else
      m_annotation->segmentation() = move(Segmentation{});
    m_annotation->labels().resize(
      m_annotation->segmentation().graph().num_vertices());
    emit segmentationUpdated(m_annotation->segmentation());

    // Load the labels if they exist.
    if (file_exists(annotationIdFilePath))
    {
      INJILA_LOG() << "Loading annotations...";
      ifstream annotationIDFile{ annotationIdFilePath };
      if (!annotationIDFile)
      {
        INJILA_ERR() << "Annotation ID file does not exist";
        return;
      }

      stringstream buffer{};
      buffer << annotationIDFile.rdbuf();
      auto annotationIDFileContents  = buffer.str();
      INJILA_LOG() << "Read annotation file contents:" << annotationIDFileContents.c_str();
      auto j = json::parse(annotationIDFileContents);
      auto annotationID = j["id"].get<string>();
      INJILA_LOG() << "Read annotation ID:" << annotationID.c_str();

      INJILA_LOG() << "GETting labels...";
      {
        auto raw_labels_data = get_labels(annotationID);
        CHECK(raw_labels_data);
        if (raw_labels_data.empty())
          return;
        for (const auto& l : raw_labels_data)
        {
          auto labelIndex = l["index"].get<int>();
          auto labels = l["labels"].get<set<string>>();
          m_annotation->labels()[labelIndex] = move(labels);
        }
      }

      {
        QList<QStringList> labels;
        labels.reserve(m_annotation->labels().size());
        for (const auto& labelSet : m_annotation->labels())
        {
          labels.push_back({});
          for (const auto& label : labelSet)
            labels.back() << QString::fromStdString(label);
        }
        emit labelsUpdated(move(labels));
      }
    }
  }

  void AnnotationController::saveAnnotation()
  {
    auto mainWindow = qobject_cast<QMainWindow *>(parent());
    mainWindow->statusBar()->showMessage(tr("Saving annotation..."));

    // Save segmentation to disk.
    if (!save_segmentation_to_disk(*m_annotation,
                                   m_annotationBasename.toStdString(),
                                   m_annotationFolder.toStdString()))
    {
      INJILA_ERR() << "Failed to save segmentation!";
      QMessageBox::warning(mainWindow,
                           tr("Error saving segmentation"),
                           tr("Failed to save segmentation result!"));
    }
    else
      mainWindow->statusBar()->showMessage(
                              tr("Saved segmentation results successfully!"));


    // Save labels to database.
    auto annotationIDFilePath = m_annotationFolder + "/" + m_annotationBasename + ".json";
    if (!file_exists(annotationIDFilePath.toStdString()))
    {
      INJILA_LOG() << "POSTing annotation";
      auto annotationID = post_annotation(*m_annotation,
                                            m_annotationBasename.toStdString(),
                                            m_annotationFolder.toStdString());

      INJILA_LOG() << "POSTed annotation ID:" << annotationID.c_str();
      INJILA_LOG() << "Saving annotation ID to:" << annotationIDFilePath;
      auto j = json { { "id", annotationID } };
      ofstream annotationIDFile{
        annotationIDFilePath.toStdString().c_str()
      };

      if (!annotationIDFile)
      {
        INJILA_LOG() << "Failed to create annotation ID file";
        return;
      }
      annotationIDFile << j.dump();
      annotationIDFile.close();
    }
    else
    {
      INJILA_LOG() << "Read annotation ID from:" << annotationIDFilePath;
      ifstream annotationIDFile{
        annotationIDFilePath.toStdString().c_str()
      };
      if (!annotationIDFile)
      {
        INJILA_ERR() << "Annotation ID file does not exist";
        return;
      }

      stringstream buffer{};
      buffer << annotationIDFile.rdbuf();
      auto annotationIDFileContents  = buffer.str();
      INJILA_LOG() << "Read annotation file contents:" << annotationIDFileContents.c_str();
      auto j = json::parse(annotationIDFileContents);
      auto annotationID = j["id"].get<string>();
      INJILA_LOG() << "Read annotation ID:" << annotationID.c_str();

      INJILA_LOG() << "PUTting annotation ID from:" << annotationIDFilePath;
      put_annotation(annotationID,
                     *m_annotation,
                     m_annotationBasename.toStdString(),
                     m_annotationFolder.toStdString());
    }
  }

  void AnnotationController::updateImage(const Image<Rgb8>& image)
  {
    m_annotation->image() = image;
    emit imageUpdated(toQImage(m_annotation->image()));

    m_annotation->segmentation() = move(Segmentation{});
    m_annotation->labels().clear();
  }

  void AnnotationController::updateSegmentation()
  {
    auto mainWindow = qobject_cast<QMainWindow *>(parent());
    mainWindow->statusBar()->showMessage(tr("Running segmentation..."));


    m_annotation->segmentation().update(m_annotation->image(),
                                        m_numSuperpixels);
    m_annotation->labels().resize(
      m_annotation->segmentation().graph().num_vertices());
    emit segmentationUpdated(m_annotation->segmentation());

    mainWindow->statusBar()->showMessage(tr("Finished segmentation!"));

    mainWindow->statusBar()->showMessage(tr("Saving segmentation results..."));
    if (!save_segmentation_to_disk(*m_annotation,
                                   m_annotationBasename.toStdString(),
                                   m_annotationFolder.toStdString()))
    {
      INJILA_ERR() << "Failed to save segmentation!";
      QMessageBox::warning(mainWindow,
                           tr("Error saving segmentation"),
                           tr("Failed to save segmentation result!"));
    }
    else
      mainWindow->statusBar()->showMessage(
        tr("Saved segmentation results successfully!"));
  }

} /* namespace Injila */
} /* namespace DO */
