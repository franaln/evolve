/** @file population.h
 */

#ifndef POPULATION_H
#define POPULATION_H

#include <string> // std::string
#include <vector> // std::vector

class Individual;

typedef std::vector<Individual*> pop_vector;

class Population {

 public:
  Population(long, float, float);
  ~Population();

  void evolve();
  void step();

 private:
  long m_size;
  long m_generation;
  float m_prob_crossover;
  float m_prob_mutation;


  double m_total_fitness;

  pop_vector m_population;

  void select_parents();
  int roulette();
  void crossover(Individual*, Individual*, pop_vector&);
  void mutate(pop_vector&);
  void update(pop_vector&);

  void print();

};
#endif