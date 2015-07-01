#pragma once

#include <DO/Sara/Core.hpp>


namespace DO { namespace Sara {

  /*!
    Given $k$ cluster means encoded in matrix 'kMeans', find the index of the 
    closest mean for the feature 'f'.
   */
  template <typename Feature>
  int nearest_neighbors(const Feature& f, const MatrixXd& means)
  {
    int clusterId;
    (means.colwise() - f.template cast<double>()).colwise().squaredNorm().minCoeff(&clusterId);
    return clusterId;
  }

  /*!
    Random k-Means initialization using:
    - Mersenne-Twister pseudo random generator (robust)
    - Coefficient of mean vectors are initialized with the uniform law.
   */
  MatrixXd random_init_kmeans(int feature_dimension, int num_clusters,
                              double min_coeff = 0., double max_coeff = 1.);

  /*!
    Greedy optimization of k-Means.
   */
  void optimize_kmeans(const MatrixXd& data,
                       MatrixXd& means,
                       int iter_max = 1,
                       VectorXi&& assignment_vector = VectorXi(),
                       VectorXi&& count_vector = VectorXi(),
                       bool verbose = false);

}
}
