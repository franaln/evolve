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
  m_fitness = 0.;
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

double* Individual::get_cuts()
{
  double *cuts = new double[m_cuts.size()];
  for (unsigned int i=0; i<m_cuts.size(); i++) {
    cuts[i] = m_cuts[i];
  }

  return cuts;
}
