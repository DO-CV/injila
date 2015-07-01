#include "kmeans.hpp"
#include "random.hpp"


namespace DO { namespace Sara {

  MatrixXd random_init_kmeans(int feature_dimension, int num_clusters,
                              double min_coeff, double max_coeff)
  {
    UniformRandom random_coeff(min_coeff, max_coeff);
    MatrixXd means(feature_dimension, num_clusters);
    for (int j = 0; j < num_clusters; ++j)
      for (int i = 0; i < feature_dimension; ++i)
        means(i,j) =  random_coeff();
    return means;
  }

  void optimize_kmeans(const MatrixXd& data, MatrixXd& means, int iter_max,
                       VectorXi& assignment_vector,
                       VectorXi& count_vector,
                       bool verbose)
  {
    // ======================================================================== //
    // Shorten the names of the input data.
    // N = number of data points
    const int N = data.cols();
    // K = number of clusters
    const int K = means.cols();
    // X = data matrix
    const MatrixXd& X = data;
    // M = mean matrix
    MatrixXd& M = means;
    // D = distance matrix where D(i,j) = < M.col(i), X.col(j) >
    MatrixXd D(K, N);


    // ======================================================================== //
    // Shorten the names of the work data.
    // Z = assignment vector.
    VectorXi& Z = assignment_vector;
    // C = count vector, i.e., the i-th entry C(i) is assigned the number of 
    // data points X(j) closest to mean vector of cluster $M(i)$, 
    VectorXi& C = count_vector;
    if (Z.size() != N)
      Z.resize(N);
    if (C.size() != K)
      C.resize(K);

    // ======================================================================== //
    // Minimize the energy involved in K-Means.
    double E = std::numeric_limits<double>::max();
    double dE_rate = std::numeric_limits<double>::max();
    const double eps = std::numeric_limits<double>::epsilon();
    int iter = 0;
    while (iter < iter_max && dE_rate > eps)
    {
      // Update the distance matrix D.
      for (int i = 0; i < N; ++i)
      {
        // Compute the distance vector
        D.col(i) = 
          // For each column vector 'M.col(j)', subtract 'X.col(i)'
          (M.colwise() - X.col(i))
          // We obtain a matrix where each column vector is 'M.col(j) - X.col(i)'
          // For each column vector, compute its squared norm
          .colwise().squaredNorm()
          // We obtain a row vector, so transpose it.
          .transpose();
      }

      // Update the assignment vector, count vector and energy.
      double E1 = 0;
      C.setZero();
      for (int i = 0; i < Z.size(); ++i)
      {
        E1 += D.col(i).minCoeff(&Z(i));
        ++C(Z(i));
      }

      // Update the mean vector for each cluster.
      M.setZero();
      // Accumulate
      for (int i = 0; i < N; ++i)
        M.col(Z(i)) += X.col(i);
      // Normalize
      for (int k = 0; k < K; ++k)
        if (C(k) != 0)
          M.col(k) /= C(k);

      //
      ++iter;
      dE_rate = abs(E-E1) / E;
      E = E1;

      if (verbose)
      {
        CHECK(iter);
        CHECK(dE_rate);
        CHECK(E);
      }
    }
  }

}
}
