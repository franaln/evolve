/** @file chromosome.h
 */

#ifndef CHROMOSOME_H
#define CHROMOSOME_H

#include <vector>
#include <iostream>

class Gene;

class Chromosome {

 public:
  Chromosome(int);
  virtual ~Chromosome();

  /* void set_gen(int index, int value) { genes[index]->set(value); }; */
  /* void set_gen(int index, float value) { genes[index]->set(value); }; */

  //virtual void eval_fitness(); /** compute fitness */
  //void mutate(); /** randomly flip a gen */
  //  int get_gen(int i){ return genes[i]; };

  //Chromosome* copy();

  /* inline bool operator< (const Chromosome& lhs, const Chromosome& rhs){ /\* do actual comparison *\/ } */
  /* inline bool operator> (const Chromosome& lhs, const Chromosome& rhs){ return rhs < lhs; } */
  /* inline bool operator<=(const Chromosome& lhs, const Chromosome& rhs){ return !(lhs > rhs); } */
  /* inline bool operator>=(const Chromosome& lhs, const Chromosome& rhs){ return !(lhs < rhs); } */

  friend std::ostream& operator << (std::ostream &out, const Chromosome &c);

 private:
  std::vector<Gene*> genes;
  //double fitness;

};

/* std::ostream& operator << (std::ostream& out, const Chromosome &c) */
/* { */
/*   out << "( "; */
/*   for(const auto& g : c.genes){ */
/*     out << g << ", "; */
/*   } */
/*   out << ")"; */
/*   return out; */
/* } */

#endif
