/** @file random.cpp
 */

#include <chrono> // system_clock

#include "random.h"

generator_t generator;


void init_random()
{
  // obtain a seed from the system clock:
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  generator.seed(seed);
}

int get_random_int(int lower, int upper)
{
  std::uniform_int_distribution<int> distribution(lower, upper);
  return distribution(generator);
}

float get_random_float(float lower, float upper)
{
  std::uniform_real_distribution<double> distribution(lower, upper);
  return distribution(generator);
}

float get_random_prob()
{
  return get_random_float(0.0, 1.0);
}
