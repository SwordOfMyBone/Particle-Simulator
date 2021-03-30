#include "partB/timeseries.h"
#include <cstdlib>
#include <iostream>
#include <ompt.h>
#include <vector>

void alterParticle(std::vector<int> &p, int i, bool rBool);
void print(const std::vector<std::vector<int>> &v, int i);
void store(const std::vector<std::vector<int>> &v, int i,
           struct timeSeries &tS);

int main() {
  // set the particle alteration var to 2 for the first loop.
  int pAlter{2};
  // creates a bool which will be used to randomly decrement/increment. 
  bool rBool;
  size_t iterations{50};

  // iterArr takes the value of the iteration to be saved, it starts from 1.
  // and must be placed in ascending order.
  // eg: to save results of iteration 0 we place 1 in the array.
  // we then create a time series object to store results.
  std::vector<size_t> iterArr{1, 25, 50};
  struct timeSeries ts;

  // create vector of int vectors holding initial values for particles.
  std::vector<std::vector<int>> v{
      {5, 14, 10}, {7, -8, -14}, {-2, 9, 8},  {15, -6, 3}, {12, 4, -5},
      {4, 20, 17}, {-16, 5, -1}, {-11, 3, 6}, {3, 10, 19}, {-16, 7, 4}};

  // loop 50 times to create time series
  // if it's the first iteration save the state of the particles to the
  // timeseries. If it's not the first iteration choose a random
  // number from 1-5 and assign that to pAlter, which will act as the variable
  // to increment/decrement with.
  for (size_t i = 0; i < iterations; ++i) {
    if (i == 0) {
      if (iterArr[0] == 1)
        store(v, i + 1, ts);
    } else {
      pAlter = rand() % 5 + 1;
      rBool =
          rand() % 2; // generate a random number between 1-2 into a bool
                      // which will be used to decide if it's + or -.
    }

    // iterate over vector containing particles and alter X/Y values.
#pragma omp parallel for shared(pAlter) schedule(auto)
    for (size_t j = 0; j < v.size(); ++j) {
      alterParticle(v[j], pAlter, rBool);
    }

    // check for iterations after 0 and store the particles coord
    // at that point of the iteration.
#pragma omp parallel for
    for (size_t j = 1; j < iterArr.size(); ++j) {
      // handle the case where iteration is equal to the value (excluding the
      // first iteration).
      if (iterArr[j] == i) {
        store(v, i, ts);
      }
      // handle when we want to save the last iteration.
      if (iterArr[j] == iterations && i == iterations - 1)
        store(v, i + 1, ts);
    }
  } // implicit omp barrier here.

  // print out results based on iteration
  for (size_t i = 0; i < iterArr.size(); ++i) {
    print(ts.v[i], ts.i[i]);
  }
}

// takes p and inc/dec (randomly) both X and Y coord by the a parameter
void alterParticle(std::vector<int> &p, int a, bool rBool) {
  if (rBool) { // if rBool is true increment
    p[0] += a;
    p[1] += a;
  } else { // else decrement
    p[0] -= a;
    p[1] -= a;
  }
}

// Prints out a Time Series
// Takes in the main vector holding the initial values
// along with int i which represents the current iteration
// to be printed out
// iterates over the size of the time series (50 iterations)
// then prints out each particles x,y,z coords.
void print(const std::vector<std::vector<int>> &v, int iter) {
  std::cout << "Iteration " << iter << ":" << std::endl;
  for (size_t i = 0; i < v.size(); ++i) {
    std::cout << "\t"
              << "Particle " << i + 1 << ": ";
    for (size_t j = 0; j < 3; ++j) {
      std::cout << v[i][j] << " ";
    }
    std::cout << std::endl;
  }
}

// Takes the main vector containing the particles, along with the iteration i
// and stores it in a time series.
void store(const std::vector<std::vector<int>> &v, int i,
           struct timeSeries &tS) {
  tS.i.push_back(i);
  tS.v.push_back(v);
}
