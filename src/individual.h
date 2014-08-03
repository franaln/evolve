/** @file individual.h
 */

#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <vector>
#include <iostream>


struct CutInt {
  std::string var;
  std::string op;
  int value;
};

struct CutFloat {
  std::string var;
  std::string op;
  double value;
};


class Individual {

 public:
  Individual(int size_int, int size_float);
  ~Individual();

  /* void set_gen(int index, int value) { genes[index]->set(value); }; */
  /* void set_gen(int index, float value) { genes[index]->set(value); }; */

  //virtual void eval_fitness(); /** compute fitness */
  //void mutate(); /** randomly flip a gen */
  //  int get_gen(int i){ return genes[i]; };


  void set_cut_int(int idx, int v) { cuts_int[idx] = v; }
  void set_cut_float(int idx, double v) { cuts_float[idx] = v; }

  void set_fitness(double f) { m_fitness = f; };
  double get_fitness() { return m_fitness; };

  Individual* copy();

  void print();

  //void mutate();


  friend bool operator< (const Individual& lhs, const Individual& rhs){ return lhs.m_fitness < rhs.m_fitness; }
  friend bool operator> (const Individual& lhs, const Individual& rhs){ return rhs < lhs; }
  /* inline bool operator<=(const Individual& lhs, const Individual& rhs){ return !(lhs > rhs); } */
  /* inline bool operator>=(const Individual& lhs, const Individual& rhs){ return !(lhs < rhs); } */

  //  friend std::ostream& operator << (std::ostream &out, const Individual &c);

  std::vector<int>    cuts_int;
  std::vector<double> cuts_float;

 private:
  //vector<CutInt>

  double m_fitness;
};

/* bool operator<(const Individual& lhs, const Individual& rhs) */
/* { */
/*   return lhs.m_fitness < rhs.m_fitness; */
/* } */



#endif
