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
  double get_fitness() { return m_fitness ? m_fitness : 0.0; };

  void set_signal(int s) { m_signal = s; };
  void set_background(int b) { m_background = b; };

  int get_signal() { return m_signal; };
  int get_background() { return m_background; };

  Individual* copy();

 private:
  std::vector<double> m_cuts;

  int m_signal;
  int m_background;
  double m_fitness;
};

bool sort_fitness(Individual*, Individual*);

#endif
