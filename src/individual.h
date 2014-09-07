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

  void add_cut(float v) { m_cuts.push_back(v); }
  float get_cut(int idx) { return m_cuts[idx]; }
  void set_cut(int idx,float v) { m_cuts[idx] = v; }

  void set_fitness(float f) { m_fitness = f; };
  float get_fitness() { return m_fitness; };

  Individual* copy();

  void print();

  friend bool operator< (const Individual& lhs, const Individual& rhs){ return lhs.m_fitness < rhs.m_fitness; }
  friend bool operator> (const Individual& lhs, const Individual& rhs){ return rhs < lhs; }
  friend bool operator<=(const Individual& lhs, const Individual& rhs){ return !(lhs > rhs); }
  friend bool operator>=(const Individual& lhs, const Individual& rhs){ return !(lhs < rhs); }

 private:
  std::vector<float> m_cuts;
  double m_fitness;
};

#endif
