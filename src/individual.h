/** @file individual.h
 */

#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <vector>
#include <iostream>


class Individual {

 public:
  Individual();
  ~Individual() {};

  void add_cut(double v) { m_cuts.push_back(v); }
  void set_cut(int idx, double v) { m_cuts[idx] = v; }

  double get_cut(int idx) { return m_cuts[idx]; }
  double* get_cuts();

  void set_fitness(double f) { m_fitness = f; };
  double get_fitness() { return m_fitness>0. ? m_fitness : 0.; };

  void set_signal(double s) { m_signal = s; };
  void set_background(double b) { m_background = b; };

  double get_signal() { return m_signal; };
  double get_background() { return m_background; };

  Individual* copy();

 private:
  std::vector<double> m_cuts;

  double m_signal;
  double m_background;
  double m_fitness;
};

bool sort_fitness(Individual*, Individual*);

#endif
