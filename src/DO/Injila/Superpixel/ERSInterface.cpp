#include <exception>
#include <queue>

#include <DO/Sara/Graphics.hpp>

#include <ERS/MERCLazyGreedy.h>
#include <ERS/MERCInputImage.h>
#include <ERS/MERCOutputImage.h>
#include <ERS/Image.h>
#include <ERS/ImageIO.h>

#include "DO/Injila/Superpixel/ERSInterface.hpp"


namespace DO { namespace Sara {

  Vector3d SuperPixel::mean_color() const
  {
    return colors().rowwise().sum()/colors().cols();
  }

  Vector2d SuperPixel::center() const
  {
    return points().rowwise().sum()/points().cols();
  }

  Map<const Matrix3Xd> SuperPixel::colors() const
  {
    return Map<const Matrix3Xd>(&_colors[0], 3, _colors.size()/3);
  }

  Map<const Matrix2Xd> SuperPixel::points() const
  {
    return Map<const Matrix2Xd>(&_points[0], 2, _points.size()/2);
  }

  Map<Point2d> SuperPixel::point(unsigned i)
  {
    if (i >= num_points())
      throw std::range_error("index out of range");
    return Map<Point2d>(&_points[i*2]);
  }

  Map<const Point2d> SuperPixel::point(unsigned i) const
  {
    if (i >= num_points())
      throw std::range_error("index out of range");
    return Map<const Point2d>(&_points[i*2]);
  }

  Matrix3d SuperPixel::color_covariance() const
  {
    Map<const Matrix3Xd> X(colors());
    return X*X.transpose();
  }

  Matrix2d SuperPixel::spatial_covariance() const
  {
    Map<const Matrix2Xd> X(points());
    return X*X.transpose();
  }

  void SuperPixel::draw(const Color3ub& c) const
  {
    if (2*_colors.size() != 3*_points.size())
      throw std::exception();

    for (unsigned i = 0; i < num_points(); ++i)
      draw_point(point(i).cast<float>(), c);
  }

  void SuperPixel::draw_center(const Color3ub& c, int r) const
  {
    fill_circle(center().cast<int>(), r, c);
  }

  SuperPixelGraph::SuperPixelGraph(int n)
    : v(n), a(n,n), e(n,n)
  {
    a.fill(0);
    e.fill(0.);
  }

  void SuperPixelGraph::draw_vertex(int i) const
  {
    Rgb8 m;
    convert_channel(Rgb64f(v[i].mean_color()), m);
    v[i].draw(m);
  }

  void SuperPixelGraph::draw_edge(int i, int j, int w) const
  {
    draw_line(v[i].center(), v[j].center(), Black8, w);
  }

  void SuperPixelGraph::draw(double lineWidthMax) const
  {
    for (unsigned i = 0; i < v.size(); ++i)
      draw_vertex(i);

    for (unsigned i = 0; i < v.size(); ++i)
      for(unsigned j = i+1; j < v.size(); ++j)
        if (a(i,j) == 1.)
        {
          double w = e(i,j)*lineWidthMax;
          w = w < 1. ? 1 : w;
          draw_edge(i,j, int(w+0.5));
        }
  }

  void SuperPixelGraph::draw_vertices(const Color3ub& centerColor, int r) const
  {
    for (unsigned i = 0; i < v.size(); ++i)
      draw_vertex(i);
    for (unsigned i = 0; i < v.size(); ++i)
      v[i].draw_center(centerColor, r);
  }

  double SuperPixelGraph::compute_edge_weight(int i, int j, double sigma) const
  {
    if (a(i,j) == 0)
      return 0.;

    Rgb64f rgbi(v[i].mean_color());
    Rgb64f rgbj(v[j].mean_color());

    Yuv64f ci, cj;
    convert_color(ci, rgbi);
    convert_color(cj, rgbj);

    double sd = (ci - cj).squaredNorm();
    double sqSigma = sigma*sigma;
    double w = exp( - sd / (2.*sqSigma) );
    return w;
  }

  Image<int> compute_superpixel_map(const Image<Rgb8>& I,
                                    int nC,
                                    const Image<unsigned char> *edgeMap,
                                    double lambda,
                                    double sigma,
                                    int kernel,
                                    int conn8)
  {
    MERCLazyGreedy merc;
    ::Image<RGBMap> inputImage;
    ::Image<unsigned char> edgeMapCopy;
    MERCInputImage<RGBMap> input;

    // Create image.
    inputImage.Resize(I.width(), I.height(),false);
    edgeMapCopy.Resize(I.width(), I.height(),false);

    for(int y = 0; y < I.height(); ++y)
    {
      for (int x = 0; x < I.width(); ++x)
      {
        RGBMap color(I(x,y)(0), I(x,y)(1), I(x,y)(2));
        inputImage.Access(x,y) = color;

        // If we want to adjust the weight of the edges.
        if (edgeMap)
          edgeMapCopy.Access(x,y) = (*edgeMap)(x,y);
      }
    }

    // Adjust the  the edge weight.
    ::Image<unsigned char> *edgeMapCopyPtr = edgeMap ? &edgeMapCopy : 0;

    // Read the image for segmentation.
    input.ReadImage(&inputImage,conn8, edgeMapCopyPtr);

    // Entropy rate super-pixel segmentation
    merc.ClusteringTreeIF(
      input.nNodes_,input,
      kernel,
      sigma*3,
      lambda*1.0*nC,
      nC);

    // Get the labels of each pixel.
    vector<int> data = MERCOutputImage::DisjointSetToLabel(merc.disjointSet_);
    Sara::Image<int> labels(I.sizes());
    std::copy(data.begin(), data.end(), labels.data());
    return labels;
  }


  // 8-connectivity
  const Vector2i dir8[] = {
    Vector2i(1,0), Vector2i(1,1), Vector2i(0,1), Vector2i(-1,1), Vector2i(-1,0),
    Vector2i(-1,-1), Vector2i(0,-1), Vector2i(1,-1)
  };

  SuperPixelGraph build_graph(int nC, const Image<int>& labels,
                              const Image<Rgb64f>& colors)
  {
    SuperPixelGraph g(nC);

    const int h = colors.height();
    const int w = colors.width();
    for (int y = 0; y < h; ++y)
    {
      for (int x = 0; x < w; ++x)
      {
        // My label
        int li = labels(x,y);

        // Push the colors.
        for (int c =  0; c < 3; ++c)
          g.v[li]._colors.push_back(colors(x,y)(c));
        g.v[li]._points.push_back(x);
        g.v[li]._points.push_back(y);

        // Create the edges between super-pixels.
        for (int d = 0; d < 8; ++d)
        {
          // Neighbor.
          Point2i n(x,y); n += dir8[d];

          // Out of bounds?
          if (n.x() <  0 || n.x() >= w || n.y() < 0 || n.y() >= h)
            continue;

          // Neighbor's label.
          int lj = labels(n);

          if (li == lj)
            continue;

          // Update adjacency matrix.
          if (g.a(li,lj) == 0)
            g.a(li,lj) = g.a(lj, li) = 1.0;
        }
      }
    }

    for (unsigned i = 0; i < g.num_vertices(); ++i)
      for(unsigned j = 0; j < g.num_vertices(); ++j)
        if (g.a(i,j) == 1.)
          g.e(i,j) = g.e(j,i) = g.compute_edge_weight(i,j);

    return g;
  }

  vector<vector<Point2i>>
  trace_superpixels_inner_boundaries(const SuperPixelGraph& g, const Image<int>& labels)
  {
#ifndef CONNECTIVITY_4
    const Vector2i dirs[] = {
      Vector2i(1, 0),
      Vector2i(1, 1),
      Vector2i(0, 1),
      Vector2i(-1, 1),
      Vector2i(-1, 0),
      Vector2i(-1, -1),
      Vector2i(0, -1),
      Vector2i(1, -1)
    };
#else
    const Vector2i dirs[] = {
      Vector2i(1, 0),
      Vector2i(0, 1),
      Vector2i(-1, 0),
      Vector2i(0, -1),
    };
#endif
    const int num_dirs = sizeof(dirs) / sizeof(Vector2i);

    std::vector<std::vector<Point2i> > superpixel_boundaries(g.num_vertices());
    for (unsigned label = 0; label < g.num_vertices(); ++label)
    {
      auto& boundary = superpixel_boundaries[label];

      if (g.num_vertices() == 1)
      {
        boundary.push_back(g.v[0].point(0).cast<int>());
        continue;
      }

      // Find the starting point.
      Point2i start_point{
        [&]()
        {
          for (int y = 0; y < labels.height(); ++y)
            for (int x = 0; x < labels.width(); ++x)
              if (labels(x, y) == label)
                return Point2i(x, y);
          throw std::runtime_error("Invalid superpixel!");
        }()
      };
      boundary.push_back(start_point);

#ifndef CONNECTIVITY_4
      int dir = 7;
#else
      int dir = 0;
#endif
      do {
        const Point2i& current_point{ boundary.back() };

#ifndef CONNECTIVITY_4
        dir = (dir % 2) == 0 ? (dir + 7) % 8 : (dir + 6) % 8;
#else
        dir = (dir + 3) % 4;
#endif
        for (int d = 0; d < num_dirs; ++d)
        {
          Point2i next_point{ current_point + dirs[(dir + d) % num_dirs] };

          if (next_point.minCoeff() < 0 ||
            (labels.sizes() - next_point).minCoeff() <= 0)
            continue;

          if (labels(next_point) == label)
          {
            superpixel_boundaries[label].push_back(next_point);
            dir = (dir + d) % num_dirs;
            break;
          }
        }
      } while (boundary.back() != start_point);
      boundary.pop_back();
    }

    return superpixel_boundaries;
  }

  Image<double, 3>
  compute_color_distribution(const SuperPixel& superpixel, int num_bins)
  {
    Image<double, 3> distribution(num_bins, num_bins, num_bins);
    distribution.array().setZero();
    for (unsigned i = 0; i < superpixel.num_points(); ++i)
    {
      Vector3i color {
        (superpixel.colors().col(i)*float(num_bins)).cast<int>()
      };
      distribution(color) += 1;
    }
    distribution.array() /= superpixel.num_points();
    return distribution;
  }

  MatrixXd
  compute_color_distributions(const SuperPixelGraph& g, int num_bins)
  {
    const int distribution_vector_size{ int(pow(num_bins, 3)) };
    MatrixXd distributions{ distribution_vector_size, g.num_vertices() };
    for (unsigned i = 0; i < g.num_vertices(); ++i)
      distributions.col(i).array() = compute_color_distribution(g.v[i], num_bins).array();
    return distributions;
  }

}
}
