#pragma once

#include <DO/Sara/Core.hpp>

#include "DO/Injila/Superpixel/ERSInterface.hpp"


namespace DO { namespace Injila {

  using namespace DO::Sara;

  class Segmentation
  {
  public:
    using boundaries_type = std::vector<Point2i>;

    //! @{
    //! Constructors.
    Segmentation() = default;

    Segmentation(const Segmentation&) = default;

    Segmentation(Segmentation&& other)
      : _map{ std::move(other._map) }
      , _superpixel_graph{ std::move(other._superpixel_graph) }
      , _superpixel_boundaries{ std::move(other._superpixel_boundaries) }
    {
    }
    //! @}

    //! Copy the contents of the Segmentation object.
    Segmentation& operator=(Segmentation other)
    {
      swap(*this, other);
      return *this;
    }

    //! Swap contents of segmentation.
    friend void swap(Segmentation& s1, Segmentation& s2)
    {
      std::swap(s1._map, s2._map);
      swap(s1._superpixel_graph, s2._superpixel_graph);
      swap(s1._superpixel_boundaries, s2._superpixel_boundaries);
    }

    //! @{
    //! Return reference to the segmentation results.
    Image<int>& map()
    {
      return _map;
    }

    SuperPixelGraph& graph()
    {
      return _superpixel_graph;
    }

    std::vector<boundaries_type>& segment_boundaries()
    {
      return _superpixel_boundaries;
    }
    //! @}

    //! @{
    //! Return constant reference to the segmentation results.
    const Image<int>& map() const
    {
      return _map;
    }

    const SuperPixelGraph& graph() const
    {
      return _superpixel_graph;
    }

    const std::vector<boundaries_type>& segment_boundaries() const
    {
      return _superpixel_boundaries;
    }
    //! @}

    //! Compute the segmentation of an input image.
    void update(const Image<Rgb8>& in_image, int num_superpixels);

    void updateFromMap(const Image<Rgb8>& in_image);

  private:
    Image<int> _map;
    SuperPixelGraph _superpixel_graph;
    std::vector<std::vector<Point2i>> _superpixel_boundaries;
  };

} /* namespace Injila */
} /* namespace DO */
