/** @file individual.cpp
 */

#include <string>

#include "individual.h"

Individual* Individual::copy()
{
  Individual *twin = new Individual();

  for(auto &cut : m_cuts)
    twin->add_cut(cut);

  twin->set_fitness(m_fitness);

  return twin;
}

void Individual::print()
{
  for(auto &cut : m_cuts)
    std::cout << cut << " | ";

  std::cout << std::endl;
}
