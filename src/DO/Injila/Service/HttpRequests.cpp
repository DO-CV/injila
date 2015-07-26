#include <memory>
#include <tuple>

#include <QApplication>
#include <QtNetwork>

#include <json.hpp>

#include "DO/Injila/Service/HttpRequests.hpp"
#include "DO/Injila/Service/SegmentationIO.hpp"

#include "DO/Injila/Utilities/Logging.hpp"


using namespace std;


namespace DO { namespace Injila {

  json to_json(const Annotation& annotation,
               const string& annotation_basename,
               const string& annotation_folder)
  {
    auto image_path = [&]() {
      if (annotation_basename.empty() || annotation_folder.empty())
        return string{};
      return annotation_folder + "/" + annotation_basename + ".png";
    }();

    auto segmentation_path = [&]() {
      if (annotation_basename.empty() || annotation_folder.empty())
        return string{};
      return annotation_folder + "/" + annotation_basename + ".bin";
    }();

    auto segments = json{};
    for (size_t i = 0; i != annotation.labels().size(); ++i)
      if (!annotation.labels()[i].empty())
        segments.push_back(json
        {
          { "index", i },
          { "labels", json(annotation.labels()[i]) }
        });

    return json{
      { "imagePath", image_path },
      { "segmentationMaskPath", segmentation_path },
      { "segments", segments }
    };
  }

  vector<Annotation> get_annotations()
  {
    // Create the whole URL string.
    const auto url_string = QString{
      "http://localhost:8080/api/imageAnnotations/"
    };
    INJILA_LOG() << "Getting annotations from: " << url_string << "...";

    // Create the HTTP request.
    QNetworkRequest request;
    const QUrl url(url_string);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Send the GET request.
    QNetworkAccessManager network_manager;
    QNetworkReply *reply = network_manager.get(request);

    // Wait until the data transfer finished.
    QEventLoop event_loop;
    QObject::connect(reply, SIGNAL(finished()), &event_loop, SLOT(quit()));
    event_loop.exec();

    // Check.
    QNetworkReply::NetworkError err = reply->error();
    if (err != QNetworkReply::NoError)
      INJILA_LOG() << "GET request failed!";
    else
      INJILA_LOG() << "GET request succeeded!";

    // Now read the data.
    INJILA_LOG() << "Got JSON data:\n" << reply->readAll() << "...";


    return std::vector<Annotation>{};
  }

  std::string post_annotation(const Annotation& annotation,
                              const string& annotation_basename,
                              const string& annotation_folder)
  {
    // Create the whole URL string.
    const QString url_string{ "http://localhost:8080/api/imageAnnotations/" };
    INJILA_LOG() << "POST annotation to: " << url_string << "...";

    // Create the HTTP request.
    QNetworkRequest request;
    const QUrl url(url_string);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Form the JSON data to POST request.
    auto post_data = to_json(annotation,
                             annotation_basename,
                             annotation_folder).dump(2);
    INJILA_LOG() << "POST JSON data:\n" << post_data.c_str();

    // Send the POST request.
    QNetworkAccessManager network_manager;
    QNetworkReply *reply = network_manager.post(
      request,
      QByteArray{
        post_data.c_str(),
        static_cast<int>(post_data.size())
      }
    );

    // Wait until the data transfer finished.
    QEventLoop event_loop;
    QObject::connect(reply, SIGNAL(finished()), &event_loop, SLOT(quit()));
    event_loop.exec();

    // Check.
    QNetworkReply::NetworkError err(reply->error());
    auto success = (err == QNetworkReply::NoError);
    if (!success)
    {
      INJILA_LOG() << "POST request failed!";
      return string{};
    }

    // Extract the ID.
    auto location_header = reply->header(QNetworkRequest::LocationHeader).toString().toStdString();
    auto annotation_id = location_header.substr(location_header.find_last_of('/')+1);

    INJILA_LOG() << "POST request succeeded!";
    INJILA_LOG() << "Annotation ID created:" << annotation_id.c_str();

    return annotation_id;
  }

  bool put_annotation(const string& annotation_id,
                      const Annotation& annotation,
                      const string& annotation_basename,
                      const string& annotation_folder)
  {
    // Create the whole URL string.
    const auto url_string = QString{ "%1%2" }
      .arg("http://localhost:8080/api/imageAnnotations/")
      .arg(annotation_id.c_str());
    INJILA_LOG() << "PUT annotation to: " << url_string << "...";

    // Create the HTTP request.
    QNetworkRequest request;
    const QUrl url(url_string);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Form the JSON data to PUT request.
    auto put_json = to_json(annotation,
                            annotation_basename,
                            annotation_folder);
    auto put_data = put_json.dump(2);
    INJILA_LOG() << "PUT JSON data:\n" << put_data.c_str();

    // Send a POST request.
    QNetworkAccessManager network_manager;
    QNetworkReply *reply = network_manager.put(
      request,
      QByteArray{
        put_data.c_str(),
        static_cast<int>(put_data.size())
      }
    );

    // Wait until the data transfer finished.
    QEventLoop event_loop;
    QObject::connect(reply, SIGNAL(finished()), &event_loop, SLOT(quit()));
    event_loop.exec();

    // Check.
    QNetworkReply::NetworkError err{ reply->error() };
    auto success = (err == QNetworkReply::NoError);
    if (!success)
      INJILA_LOG() << "PUT request failed!";
    else
      INJILA_LOG() << "PUT request succeded!";

    return success;
  }

  json get_labels(const string &annotation_id)
  {
    // Create the whole URL string.
    const auto url_string = QString{ "%1%2" }
      .arg("http://localhost:8080/api/imageAnnotations/")
      .arg(annotation_id.c_str());
    INJILA_LOG() << "PUT annotation to: " << url_string << "...";

    // Create the HTTP request.
    QNetworkRequest request;
    const QUrl url(url_string);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Send the GET request.
    QNetworkAccessManager network_manager;
    QNetworkReply *reply = network_manager.get(request);

    // Wait until the data transfer finished.
    QEventLoop event_loop;
    QObject::connect(reply, SIGNAL(finished()), &event_loop, SLOT(quit()));
    event_loop.exec();

    // Check.
    QNetworkReply::NetworkError err = reply->error();
    if (err != QNetworkReply::NoError)
      INJILA_LOG() << "GET request failed!";
    else
      INJILA_LOG() << "GET request succeeded!";

    // Now read the data.
    auto reply_content = QString{ reply->readAll() }.toStdString();
    //boost::replace_all(reply_content, R"(")", R"(\")");
    cout << reply_content << endl;

    setlocale(LC_NUMERIC, "C");
    auto j = json::parse(reply_content.c_str());
    if (j.find("segments") == j.end())
      return json{};
    return j["segments"];
  }

} /* namespace Injila */
} /* namespace DO */
