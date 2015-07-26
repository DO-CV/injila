#pragma once

#include <random>


namespace DO { namespace Sara {

  class UniformRandom
  {
  public:
    UniformRandom(double a = 0., double b = 1.) : rand_(a, b) {}
    double operator()() { return rand_(rng_); }

  private:
    std::mt19937 rng_;
    std::uniform_real_distribution<double> rand_;
  };

} /* namespace Sara */
} /* namespace DO */
