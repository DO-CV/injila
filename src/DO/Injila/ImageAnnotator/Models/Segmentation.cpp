#include "DO/Injila/Superpixel/ERSInterface.hpp"

#include "DO/Injila/ImageAnnotator/Models/Segmentation.hpp"


namespace DO { namespace Injila {

  void Segmentation::update(const Image<Rgb8>& in_image,
                            int num_superpixels)
  {
    if (!in_image.data())
      return;

    _map = compute_superpixel_map(in_image, num_superpixels);
    _superpixel_graph = build_graph(num_superpixels, _map,
                                    in_image.convert<Rgb64f>());
    _superpixel_boundaries = trace_superpixels_inner_boundaries(
      _superpixel_graph, _map);
  }

  void Segmentation::updateFromMap(const Image<Rgb8>& in_image)
  {
    auto num_superpixels = _map.array().maxCoeff() + 1;
    _superpixel_graph = build_graph(num_superpixels, _map,
                                    in_image.convert<Rgb64f>());
    _superpixel_boundaries = trace_superpixels_inner_boundaries(
      _superpixel_graph, _map);
  }

} /* namespace Injila */
} /* namespace DO */
