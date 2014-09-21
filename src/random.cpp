/** @file random.cpp
 */

#include <chrono> // system_clock

#include "random.h"

generator_t generator;

// init random with seed from the system clock
void init_random()
{
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  generator.seed(seed);
}

// get random integer between lower and upper
int get_random_int(int lower, int upper)
{
  std::uniform_int_distribution<int> distribution(lower, upper);
  return distribution(generator);
}

// get random float between lower and upper
float get_random_float(float lower, float upper)
{
  std::uniform_real_distribution<double> distribution(lower, upper);
  return distribution(generator);
}

// get random probability: float between 0 and 1
float get_random_prob()
{
  return get_random_float(0.0, 1.0);
}
