#include <queue>

#include <DO/Sara/Graphics.hpp>
#include <DO/Sara/ImageProcessing.hpp>

#include "DO/Injila/Superpixel/ERSInterface.hpp"


using namespace std;
using namespace DO::Sara;


Image<Rgb64f> blend(const Image<Rgb64f>& I, const Image<Rgb64f>& I2, double alpha = 0.5)
{
  Image<Rgb64f> blend(I.sizes());
  for (int y = 0; y < blend.height(); ++y)
    for (int x = 0; x < blend.width(); ++x)  
      blend(x,y) = alpha*I(x,y) + (1-alpha)*I2(x,y);
    return blend;
}

Image<Rgb64f> mean_pixel_map(const SuperPixelGraph& g, const Image<int>& labels)
{
  vector<Rgb64f> meanColors(g.num_vertices());
  for (unsigned i = 0; i < g.num_vertices(); ++i)
    meanColors[i] = g.v[i].mean_color();

  Image<Rgb64f> meanPixelMap(labels.sizes());
  for (int y = 0; y < meanPixelMap.height(); ++y)
    for (int x = 0; x < meanPixelMap.width(); ++x)
      meanPixelMap(x,y) = meanColors[labels(x,y)];
  return meanPixelMap;
}

void show_blending(const SuperPixelGraph& g, const Image<Rgb8>& I, const Image<int>& labels)
{
  Image<Rgb64f> m(mean_pixel_map(g, labels));
  display(blend(I.convert<Rgb64f>(), m, 0.25));
  get_key();
}

GRAPHICS_MAIN()
{
  // Load the image.
  string image_path{ src_path("monge_1.jpg") };
  Image<Rgb8> image;
  if (!load(image, image_path))
  {
    cerr << "Could not load image: " << image_path << endl;
    return 1;
  }

  create_window(image.width(), image.height());

  // Wrap the data as a 2D image.
  int nC { 500 };
  Image<int> labels { compute_superpixel_map(image, nC) };

  // Construct the graph of super-pixels.
  SuperPixelGraph graph { build_graph(nC, labels, image.convert<Rgb64f>()) };
  graph.draw();
  get_key();

  Image<Rgb64f> mean { mean_pixel_map(graph, labels) };
  vector<vector<Point2i>> boundaries {
    trace_superpixels_inner_boundaries(graph, labels)
  };
  display(mean);
  for (const auto& boundary : boundaries)
    for (const auto& point : boundary)
      draw_point(point.x(), point.y(), Black8);
  get_key();


  return 0;
}
