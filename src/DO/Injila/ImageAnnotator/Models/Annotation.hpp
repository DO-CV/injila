#pragma once

#include <set>

#include <DO/Sara/Core.hpp>

#include "DO/Injila/ImageAnnotator/Models/Segmentation.hpp"


namespace DO { namespace Injila {

  using namespace Sara;

  class Annotation
  {
  public:
    //! @{
    //! Constructors.
    Annotation() = default;

    Annotation(Image<Rgb8> image,
               Segmentation segmentation,
               std::vector<std::set<std::string> > labels)
     : _image{ std::move(image) }
     , _segmentation{ std::move(segmentation) }
     , _labels{ std::move(labels) }
    {
    }
    //! @}

    //! @{
    //! Return reference to data member.
    Image<Rgb8>& image()
    {
      return _image;
    }

    Segmentation& segmentation()
    {
      return _segmentation;
    }

    std::vector<std::set<std::string>>& labels()
    {
      return _labels;
    }

    const Image<Rgb8>& image() const
    {
      return _image;
    }

    const Segmentation& segmentation() const
    {
      return _segmentation;
    }

    const std::vector<std::set<std::string>>& labels() const
    {
      return _labels;
    }
    //! @}

  private:
    Image<Rgb8> _image;
    Segmentation _segmentation;
    std::vector<std::set<std::string> > _labels;
  };

} /* namespace Injila */
} /* namespace DO */
