#include <locale>

#include <QtTest>

#include <gtest/gtest.h>

#include <json.hpp>

#include <DO/Sara/Core.hpp>

#include "../HttpRequests.hpp"
#include "../SegmentationIO.hpp"


using namespace std;
using namespace DO;
using namespace DO::Sara;


TEST(TestService, test_read_write_segmentation)
{
  // Test write.
  auto image = Image<int>{ Vector2i{2, 2} };
  image.matrix() <<
    0, 1,
    2, 3;
  EXPECT_TRUE(Injila::write_segmentation(image, "segmentation.bin"));

  // Test read.
  Image<int> read_image{ Injila::read_segmentation("segmentation.bin") };
  EXPECT_EQ(image.matrix(), read_image.matrix());

  // Test read when file does not exist.
  EXPECT_THROW(Injila::read_segmentation("nonexistent_segmentation.bin"),
               std::runtime_error);
}

TEST(TestService, test_save_annotation_to_disk)
{
  Injila::Annotation annotation;

  auto& image = annotation.image();
  image.resize(Vector2i{ 2, 2 });
  image(0, 0) = Red8; image(1, 0) = Green8;
  image(0, 1) = Blue8; image(1, 1) = White8;

  auto& segmentation = annotation.segmentation().map();
  segmentation.resize(Vector2i{ 2, 2 });
  segmentation.matrix() <<
    0, 1,
    2, 3;

  auto& labels = annotation.labels();
  labels = {
    { "zero", "0", "alpha" },
    { "one", "1", "beta" },
    { "two", "2", "gamma" },
    { "three", "3", "delta" }
  };

  EXPECT_TRUE(Injila::save_segmentation_to_disk(
    annotation,
    "annotation_folder", "basename"));
}

TEST(TestService, test_get_annotations)
{
  Injila::get_annotations();
}

TEST(TestService, test_post_annotations)
{
  Injila::Annotation annotation;

  auto& image = annotation.image();
  image.resize(Vector2i{ 2, 2 });
  image(0, 0) = Red8; image(1, 0) = Green8;
  image(0, 1) = Blue8; image(1, 1) = White8;

  auto& segmentation = annotation.segmentation().map();
  segmentation.resize(Vector2i{ 2, 2 });
  segmentation.matrix() <<
    0, 1,
    2, 3;

  auto& labels = annotation.labels();
  labels = {
    { "zero", "0", "alpha" },
    { "one", "1", "beta" },
    { "two", "2", "gamma" },
    { "three", "3", "delta" }
  };

  EXPECT_NE(Injila::post_annotation(
              annotation,
              "annotation_folder", "basename"),
            string{});
}


TEST(TestJson, test_parse)
{
  using json = nlohmann::json;

  // N.B.: set the locale otherwise the json library will fail to parse this!
  setlocale(LC_NUMERIC, "C");

  auto j = R"(
  {
      "id": "000001",
      "imagePath" : "/path/to/image",
      "segmentationMaskPath":"/path/to/segmentation/mask",
      "segments":
      [
          {"index": 72,  "labels":["blue","cloud","sky"]},
          {"index": 74,  "labels":["blue","cloud","sky"]},
          {"index": 124, "labels":["blue","cloud","sky"]},
          {"index": 130, "labels":["blue","cloud","sky"]}
      ]
  })"_json;
}


int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  app.setAttribute(Qt::AA_Use96Dpi, true);

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
