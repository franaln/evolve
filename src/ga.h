/** @file ga.h
 */

#ifndef GA_H
#define GA_H

#include <string> // std::string
#include <vector> // std::vector

class Individual;

typedef std::vector<Individual*> pop_vector;

class GA {

 public:
  GA();
  ~GA();

  void evolve();

 private:
  unsigned long m_population_size;
  unsigned long m_generation;
  float m_prob_crossover;
  float m_prob_mutation;


  double m_total_fitness;

  pop_vector m_population;

  void step();
  void select_parents();
  int roulette();
  void crossover(Individual*, Individual*, pop_vector&);
  void mutate(pop_vector&);
  void update(pop_vector&);

  void print();

};
#endif
