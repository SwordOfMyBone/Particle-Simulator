#include "partB/timeseries.h"
#include <cstdlib>
#include <iostream>
#include <ompt.h>
#include <vector>

void alterParticle(std::vector<int> &p, int i );
void print(const std::vector<std::vector<int>> &v, int i);
void store(const std::vector<std::vector<int>> &v, int i,
           struct timeSeries &tS);

int main() {
  // set the particle alteration var to 2 for the first loop.
  // creates a bool which will be used to randomly decrement/increment.
  int pAlter{2};
  srand(time(NULL));

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
  for (size_t i = 0; i < 50; ++i) {
    if (i == 0) {
      if (iterArr[0] == 1)
        store(v, i + 1, ts); // i+1 is there to make it the first iteration.
    } else {
      pAlter += rand() % 5 + 1;
    }

    // iterate over vector containing particles and alter X/Y/Z values.
#pragma omp parallel for shared(pAlter) schedule(auto)
    for (size_t j = 0; j < v.size(); ++j) {
      alterParticle(v[j], pAlter);
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
      if (iterArr[j] == 50 && i == 50 - 1)
        store(v, i + 1, ts);
    }
  } // implicit omp barrier here.

  // print out results based on iteration (outside of parallel region)
  for (size_t i = 0; i < 3; ++i) {
    print(ts.v[i], ts.i[i]);
  }

  // part C
  // calculating the middle of each point.
  int mX = 0, mY = 0, mZ = 0;
  std::vector<int> midVect{0, 0, 0};
  // parallel loop to sum the value of all the particles coord into
  // seperate X, Y, Z variables. Reduction clause is used to avoid
  // race conditions.
#pragma omp parallel for reduction(+ : mX, mY, mZ)
  for (size_t i = 0; i < v.size(); ++i) {
    mX += v[i][0]; // add x value
    mY += v[i][1]; // add y value
    mZ += v[i][2]; // add z value
  }

  int vSize = v.size(); // do a (possibly) narrowing conversion into an int to
                        // avoid bugs.
  mX /= vSize;
  mY /= vSize;
  mZ /= vSize;

  midVect[0] = mX;
  midVect[1] = mY;
  midVect[2] = mZ;

  std::cout << midVect[0] << std::endl;
  std::cout << midVect[1] << std::endl;
  std::cout << midVect[2] << std::endl;

  std::cout << mX << std::endl;
  std::cout << mY << std::endl;
  std::cout << mZ << std::endl;

  pAlter = 2; // reset the value of pAlter to 2.
              // do next 50 iterations to move it towards center
  for (size_t i = 50; i < 100; ++i) {
    if (i != 50) {
      pAlter += rand() % 5 + 1;
    }

    // for each particle try to move it closer to the middle coordinates
    // stored in midVect. Each particles X,Y,Z value is compared and if it's
    // greater then we add if it's less then we subtract to try and get it to
    // the middle. Each axis is done independently.
#pragma omp parallel for 
    for (size_t j = 0; j < v.size(); ++j) {
      for (size_t k = 0; k < 3; ++k) { // for each coordinate
        if (midVect[k] > v[j][k])
          v[j][k] += pAlter; // add if mid coord is greater than it.
        else
          v[j][k] -= pAlter; // subtract
      }
    }

    iterArr = {75, 100};
    // save the current iteration values
#pragma omp parallel for
    for (size_t j = 0; j < iterArr.size(); ++j) {
      // handle the case where iteration is equal to the value (excluding the
      // first iteration).
      if (iterArr[j] == i) {
        store(v, i , ts);
      }
      // handle when we want to save the last iteration.
      if (iterArr[j] == 100 && i == 100 - 1)
        store(v, i + 1 , ts);
    }
  }

  // print out iterations past 50.
  for (size_t i = 2; i < 5; ++i)
    print(ts.v[i], ts.i[i]);
}

// takes p and inc/dec (randomly) both X and Y coord by the a parameter
void alterParticle(std::vector<int> &p, int a) {
  bool rBool;
  for (int i = 0; i < 3; ++i) {
    rBool = rand() % 2; // generate a random number between 1-2 into a bool
                        // which will be used to decide if it's + or -.

    if (rBool) { // if rBool is true increment
      p[i] += a; // adds to X, Y and Z coordinates
    } else {     // else decrement
      p[i] -= a; // decrements from X, Y, Z coordinates
    }
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
