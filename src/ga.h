/** @file ga.h
 */

#ifndef GA_H
#define GA_H

#include <string> // std::string
#include <vector> // std::vector

#include <TString.h>
#include <TChain.h>
#include <THnSparse.h>

class Individual;

typedef std::vector<Individual*> pop_vector;

struct Variable {
  std::string name;
  std::string type;
  double min, max, step;
  int bins;
};

class GA {

 public:
  GA(std::string);
  ~GA();

  void evolve();

 private:
  // options
  unsigned long m_population_size;
  unsigned long m_generation;
  double m_prob_crossover;
  double m_prob_mutation;
  double m_elitism_rate;
  unsigned long m_steps;

  unsigned int  m_nvars;
  TString m_weight;
  std::vector<Variable> m_variables;

  TString m_signal_file, m_signal_treename;
  TString m_background_file, m_background_treename;
  double m_background_syst;

  // data
  pop_vector m_population;
  double m_total_fitness;
  TChain *m_signal_chain, *m_background_chain;
  THnSparseD *m_hist_s, *m_hist_b, *m_hist_sig;

  // fns
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

};
#endif
