/** @file ga.h
 */

#ifndef GA_H
#define GA_H

#include <string> // std::string
#include <vector> // std::vector

#include <TString.h>
#include <THnSparse.h>

class TChain;

class Individual;

typedef std::vector<Individual*> pop_vector;

struct Variable {
  std::string name;
  std::string type;
  double min, max, step;
};

class GA {

 public:
  GA(std::string);
  ~GA();

  void evolve();

 private:
  unsigned long m_population_size;
  unsigned long m_generation;
  double m_prob_crossover;
  double m_prob_mutation;
  double m_elitism_rate;

  double m_total_fitness;

  int m_significance_def;
  unsigned int  m_nvariables;

  std::vector<Variable> m_variables;

  pop_vector m_population;

  void step();
  void select_parents();
  int roulette();
  void crossover(Individual*, Individual*, pop_vector&);
  void mutate(pop_vector&);
  void update(pop_vector&);

  void print();
  void read_configuration(TString);

  double evaluate_fitness(Individual*);
  TString get_selection(Individual*);

  double get_random_cut(const Variable &var);


  TString m_signal_file, m_background_file;
  TString m_signal_treename, m_background_treename;
  TChain *m_signal_chain, *m_background_chain;

  TString m_weight;

  THnSparseD *m_hist_sig;

};
#endif
