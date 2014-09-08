/** @file individual.cpp
 */

#include <string>

#include "individual.h"

bool sort_fitness (Individual *i, Individual *j)
{
  return (i->get_fitness() > j->get_fitness());
}

Individual::Individual()
{
  m_fitness = 0;
  m_cuts.clear();
}

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

  std::cout << " sig= " << m_fitness << std::endl;
}

double* Individual::get_cuts()
{
  double *bins = new double[4];
  for (unsigned int i=0; i<4; i++) {
    bins[i] = m_cuts[i];
  }

  return bins;
}
