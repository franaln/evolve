/** @file ga.h */

#ifndef GA_H
#define GA_H

#include <string> // std::string
#include <vector> // std::vector
#include <fstream> // std::ofstream

#include <TString.h>
#include <TChain.h>
#include <THnSparse.h>

class Individual;

typedef std::vector<Individual*> pop_vector;

struct Variable {
  TString name;
  TString type;
  TString cut;
  double min, max, step;
  int bins;
};

class GA {

 public:
  GA(TString);
  ~GA();

  void evolve();

 private:
  TString m_name;

  TString m_signal_file, m_signal_treename;
  TString m_background_file, m_background_treename;

  unsigned long m_population_size;
  unsigned long m_generation;
  double m_prob_crossover;
  double m_prob_mutation;
  double m_elitism_rate;
  unsigned long m_generation_max;
  unsigned int  m_nvars;
  TString m_weight;
  TString m_basesel;
  std::vector<Variable> m_variables;

  double m_opt_background_syst;
  double m_opt_background_min;
  double m_opt_background_max;
  double m_opt_efficiency_min;

  pop_vector m_population;
  double m_total_fitness;
  TChain *m_signal_chain, *m_background_chain;
  THnSparseD *hist_s, *hist_b, *hist_sig;
  std::vector<double> g_best, g_mean;
  std::ofstream output;

  // fns
  void step();
  void select_parents();
  int roulette();
  void crossover(Individual*, Individual*, pop_vector&);
  void mutate(pop_vector&);
  void update(pop_vector&);
  void log();
  void show_best();
  void read_configuration(TString);
  void print_configuration();
  void evaluate_fitness();
  double evaluate_individual_fitness(Individual*);
  TString get_selection(Individual*);
  double get_events(TChain*, TString);
  double get_random_cut(const Variable &var);
  void save_histograms();

};
#endif
