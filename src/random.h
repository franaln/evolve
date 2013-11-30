/** @file random.h
 */

#ifndef RANDOM_H
#define RANDOM_H

#include <random> // random generators

class Random {

 public:
  Random(int seed=-1);

  typedef std::mt19937 generator_t;

  int next(int lower, int upper);

 private:
  generator_t rg;
};
#endif
