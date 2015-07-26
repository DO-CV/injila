#pragma once

#include <DO/Sara/Core.hpp>

#include "DO/Injila/ImageAnnotator/Models/Annotation.hpp"


namespace DO { namespace Injila {

  using namespace DO::Sara;

  Image<int> read_segmentation(const std::string& segmentation_path);

  bool write_segmentation(const Image<int>& segmentation,
                          const std::string& segmentation_path);

  bool save_segmentation_to_disk(const Annotation& annotation,
                                 const std::string& annotation_basename,
                                 const std::string& annotation_path);

} /* namespace Injila */
} /* namespace DO */
