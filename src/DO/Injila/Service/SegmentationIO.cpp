#include <cstdint>
#include <fstream>

#include <DO/Sara/Core/StringFormat.hpp>
#include <DO/Sara/ImageIO.hpp>

#include "DO/Injila/FileSystem/FileSystem.hpp"

#include "DO/Injila/Service/SegmentationIO.hpp"


using namespace std;


namespace DO { namespace Injila {

  Image<int> read_segmentation(const string& segmentation_path)
  {
    ifstream segmentation_file(segmentation_path.c_str());
    if (!segmentation_file.is_open())
      throw std::runtime_error(
        DO::Sara::format(
          "Cannot open segmentation mask file:\n %s",
          segmentation_path.c_str() ).c_str());

    Image<int> segmentation;

    // Resize the image.
    Vector2i segmentation_sizes;
    segmentation_file.read(
      reinterpret_cast<char *>(segmentation_sizes.data()),
      sizeof(int) * segmentation_sizes.size());
    segmentation.resize(segmentation_sizes);

    // Fill the data.
    segmentation_file.read(
      reinterpret_cast<char *>(segmentation.data()),
      sizeof(int) * segmentation.size());

    return segmentation;
  }

  bool write_segmentation(const Image<int>& segmentation,
                          const string& segmentation_path)
  {
    ofstream segmentation_file(segmentation_path.c_str(),
                               std::fstream::out | std::fstream::binary);
    if (!segmentation_file)
      return false;

    // Store the segmentation mask sizes
    const auto *sizes = segmentation.sizes().data();
    segmentation_file.write(reinterpret_cast<const char *>(sizes),
                                 sizeof(int) * segmentation.sizes().size());

    // Store the data of the segmentation mask.
    const auto *data = segmentation.data();
    segmentation_file.write(reinterpret_cast<const char *>(data),
                            sizeof(int) * segmentation.size());

    segmentation_file.close();

    return true;
  }

  bool save_segmentation_to_disk(const Annotation& annotation,
                                 const string& annotation_basename,
                                 const string& annotation_folder)
  {
    create_directory(annotation_folder);

    auto image_path = string{
      annotation_folder + "/" + annotation_basename + ".png" };
    auto segmentation_path = string{
      annotation_folder + "/" + annotation_basename + ".bin" };
    return
      imwrite(annotation.image(), image_path) &&
      write_segmentation(annotation.segmentation().map(), segmentation_path);
  }

} /* namespace Injila */
} /* namespace DO */
