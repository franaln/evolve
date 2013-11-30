/** @file population.h
 */

#ifndef POPULATION_H
#define POPULATION_H

#include <vector> // std::vector

class Chromosome;
class Random;

class Population {

 public:
  Population(float, float);
  ~Population();

  void evolve();
  void step();

 private:
  long pop_size;
  std::vector<std::vector<int>*> chromosomes;
  float prob_crossover;
  float prob_mutation;
  long generation;

  Random *rnd;


  void select_parents();
  void crossover();
  void mutate();
  void update();




};
#endif
