/** @file individual.h
 */

#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <vector>
#include <iostream>

class Individual {

 public:
  Individual() {};
  ~Individual() {};

  void add_cut(double v) { m_cuts.push_back(v); }
  double get_cut(int idx) { return m_cuts[idx]; }
  void set_cut(int idx, double v) { m_cuts[idx] = v; }

  void set_fitness(double f) { m_fitness = f; };
  double get_fitness() { return m_fitness; };

  Individual* copy();

  void print();

  friend bool operator< (const Individual& lhs, const Individual& rhs){ return lhs.m_fitness < rhs.m_fitness; }
  friend bool operator> (const Individual& lhs, const Individual& rhs){ return rhs < lhs; }
  friend bool operator<=(const Individual& lhs, const Individual& rhs){ return !(lhs > rhs); }
  friend bool operator>=(const Individual& lhs, const Individual& rhs){ return !(lhs < rhs); }

 private:
  std::vector<double> m_cuts;
  double m_fitness;
};

#endif
