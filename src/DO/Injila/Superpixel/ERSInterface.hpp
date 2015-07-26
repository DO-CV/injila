#pragma once

#include <vector>

#include <DO/Sara/Core.hpp>


namespace DO { namespace Sara {

  //! Superpixel class.
  class SuperPixel
  {
  public:
    SuperPixel() = default;

    unsigned num_points() const { return static_cast<unsigned>(_points.size())/2; }

    double area() const { return static_cast<double>(num_points()); }

    Vector3d mean_color() const;

    Vector2d center() const;

    Map<const Matrix3Xd> colors() const;

    Map<const Matrix2Xd> points() const;

    Map<Point2d> point(unsigned i);

    Map<const Point2d> point(unsigned i) const;

    Matrix3d color_covariance() const;

    Matrix2d spatial_covariance() const;

    void draw(const Color3ub& c) const;

    void draw_center(const Color3ub& c = Black8, int r = 5) const;

  public:
    //! @{
    std::vector<double> _colors;
    std::vector<double> _points;
    //! @}
  };


  //! Graph of superpixels.
  class SuperPixelGraph
  {
  public:
    SuperPixelGraph() = default;

    SuperPixelGraph(int num_vertices);

    SuperPixelGraph(const SuperPixelGraph& other) = default;

    SuperPixelGraph(SuperPixelGraph&& other)
    {
      swap(*this, other);
    }

    SuperPixelGraph& operator=(SuperPixelGraph other)
    {
      swap(*this, other);
      return *this;
    }

    friend void swap(SuperPixelGraph& g1, SuperPixelGraph& g2)
    {
      std::swap(g1.v, g2.v);
      g1.a.swap(g2.a);
      g1.e.swap(g2.e);
    }

    unsigned num_vertices() const { return static_cast<unsigned>(v.size()); }

    void draw_vertices(const Color3ub& centerColor = Black8, int r = 5) const;

    void draw_vertex(int i) const;

    void draw_edge(int i, int j, int w) const;

    void draw(double lineWidthMax = 5) const;

    double compute_edge_weight(int i, int j, double sigma = 3./256.) const;

  public:
    //! List of superpixels viewed as vertices.
    std::vector<SuperPixel> v;
    //! Adjacency list.
    MatrixXi a;
    //! Weights for each edge.
    MatrixXd e;

  };


  //! @{
  //! Algorithms.
  Image<int> compute_superpixel_map(const Image<Rgb8>& I,
                                    int nC,               // number of clusters
                                    const Image<unsigned char> *edgeMap = 0,
                                    double lambda = 0.5,  // Balancing parameters
                                    double sigma = 5.0,   // Kernel bandwidth
                                    int kernel = 0,       // See source code
                                    int conn8 = 1);       // 8-connectivity graph.)

  SuperPixelGraph build_graph(int nC, const Image<int>& labels,
                              const Image<Rgb64f>& colors);

  std::vector<std::vector<Point2i>>
  trace_superpixels_inner_boundaries(const SuperPixelGraph& g, const Image<int>& labels);

  Image<double, 3> compute_color_distribution(const SuperPixel& s, int num_bins = 8);

  MatrixXd compute_color_distributions(const SuperPixelGraph& g, int num_bins = 8);
  //! @}

} /* namespace Sara */
} /* namespace DO */
