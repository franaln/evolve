/** @file random.cpp
 */


#include <chrono>
#include "random.h"

Random::Random(int seed)
{

  if(seed<0){
    // obtain a seed from the system clock:
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    rg.seed(seed);
  }
  else {
    rg.seed(seed);
  }

  // std::mt19937_64 generator (seed); // mersenne twister 64 bits
  // std::uniform_int_distribution<int> distribution_int(0, 10);
  // std::uniform_real_distribution<double> distribution_real(0.0, 1.0);

  //  auto random = std::bind(distribution, generator); // bind fn with arguments

}

int Random::next(int lower, int upper)
{
  std::uniform_int_distribution<int> distribution(lower, upper);
  return distribution(rg);
}



// int Random::rnd()
// {
//   return distribution_int(generator);
// }

// float Random::rnd()
// {
//   return distribution_real
// }
