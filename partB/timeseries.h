#include <vector>

struct timeSeries {
  std::vector<int> i; // The iteration of the recorded timeseries

  // a 3d vector where the dimensions represent the iteration, particle and 
  // the particle's coordinates respectively.
  std::vector< std::vector<std::vector<int>>> v; 
};
