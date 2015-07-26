#pragma once

#include <map>
#include <set>

#include <QNetworkAccessManager>

#include <DO/Sara/Core.hpp>

#include <json.hpp>

#include "DO/Injila/ImageAnnotator/Models/Annotation.hpp"


namespace DO { namespace Injila {

  using json = nlohmann::json;

  json to_json(const Annotation& annotation,
               const std::string& annotation_basename = std::string{},
               const std::string& annotation_folder = std::string{});

  std::vector<Annotation> get_annotations();

  std::string post_annotation(const Annotation& annotation,
                              const std::string& annotation_basename,
                              const std::string& folder_path);

  bool put_annotation(const std::string& annotation_uuid,
                      const Annotation& annotation,
                      const std::string& annotation_basename,
                      const std::string& folder_path);

  json get_labels(const std::string& annotation_id);

} /* namespace Injila */
} /* namespace DO */
