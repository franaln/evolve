/** @file ga.cpp
 */

#include <iostream>

#include "ga.h"
#include "population.h"

GA::GA()
{
  pop = new Population(0.9, 0.1);

}

GA::~GA()
{
  if(pop) delete pop;
}

void GA::run()
{
  std::cout << "RUN" << std::endl;
  pop->evolve();
  return;
}
