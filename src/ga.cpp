/** @file ga.cpp
 */

#include <string>
#include <iostream>
#include <algorithm>

#include <TEnv.h>
#include <TChain.h>
#include <THnSparse.h>

#include "ga.h"
#include "individual.h"
#include "random.h"
#include "significance.h"

#define debug(x) std::cout << "debug: " << x << std::endl;

GA::GA(std::string configfile)
{
  read_configuration(configfile);

  init_random();

  // chains
  m_signal_chain = new TChain(m_signal_treename);
  m_signal_chain->Add(m_signal_file);

  m_background_chain = new TChain(m_background_treename);
  m_background_chain->Add(m_background_file);

  // create initial population (generation 0)
  m_generation = 0;
  m_population.resize(m_population_size);

  // fill each individual randomly
  for (unsigned int i=0; i<m_population_size; i++){
    m_population[i] = new Individual();

    for (auto &var : m_variables) {
      m_population[i]->add_cut(get_random_cut(var));
    }
  }

  Int_t bins[4]; // = {10, 20};
  Double_t xmin[4]; // = {0., -5.};
  Double_t xmax[4]; // = {10., 5.};

  for (unsigned int i=0; i<m_nvariables; i++) {
    bins[i] = (m_variables[i].max - m_variables[i].min)/m_variables[i].step;
    xmin[i] = m_variables[i].min;
    xmax[i] = m_variables[i].max;
  }

  m_hist_sig = new THnSparseD("hist_sig", "Significance", m_nvariables, bins, xmin, xmax);

}

GA::~GA()
{
  for(auto& ind : m_population){
    delete ind;
  }

  delete m_signal_chain;
  delete m_background_chain;
}

void GA::read_configuration(TString configfile)
{
  TEnv env(configfile.Data());

  // GA parameters
  m_population_size = env.GetValue("GA.PopulationSize", 0);
  m_prob_mutation = env.GetValue("GA.ProbMutation", 0.0);
  m_prob_crossover = env.GetValue("GA.ProbCrossOver", 0.0);
  m_elitism_rate = env.GetValue("GA.RateElitism", 0.0);

  // Signal
  m_signal_file = env.GetValue("Signal.File", "");
  m_signal_treename = env.GetValue("Signal.TreeName", "");

  // Background
  m_background_file = env.GetValue("Background.File", "");
  m_background_treename = env.GetValue("Background.TreeName", "");

  // Variables
  m_nvariables = env.GetValue("NVariables", 0);

  for (unsigned int i=0; i<m_nvariables; i++) {
    TString tmp = Form("Variable%i", i+1);

    Variable var;
    var.name = env.GetValue(tmp+".Name", "");
    var.type = env.GetValue(tmp+".Type", ">");
    var.min  = env.GetValue(tmp+".Min", 0.0);
    var.max  = env.GetValue(tmp+".Max", 0.0);
    var.step = env.GetValue(tmp+".Step", 0.0);

    m_variables.push_back(var);
  }

  m_weight = env.GetValue("Weight", "");
  //m_lumi = env.GetValue("Lumi", -1.0);

  // Significance definition
  m_significance_def = env.GetValue("Significance", 1);
}

void GA::evolve()
{
  // loop step until condition is satisfied
  for (unsigned int i=0; i<10; i++)
    step();
}

void GA::step()
{
  pop_vector children;
  children.clear();
  children.reserve(m_population_size);

  // 1. evaluate fitness
  m_total_fitness = 0.0;
  float fitness;
  for(auto& indv : m_population){
    fitness = evaluate_fitness(indv);
    indv->set_fitness(fitness);
    m_total_fitness += fitness;
  }

  // 2. sort individuals
  std::sort(m_population.begin(), m_population.end(), sort_fitness);
  //std::reverse(m_population.begin(), m_population.end());

  debug("---")
  m_population[0]->print();
  m_population[1]->print();

  // 3. elitism
  unsigned int elite_size = m_population_size * m_elitism_rate;

  for (unsigned int i=0; i<elite_size; i++) {
    children.push_back(m_population[i]->copy());
  }

  // 4. crossover
  while (children.size() < (m_population_size - elite_size)) {

    // select parents
    int p1 = roulette();
    int p2 = roulette();

    // crossover parents
    crossover(m_population[p1], m_population[p2],
              children);
  }

  // 5. mutation
  mutate(children);

  // 6. update population
  update(children);
  m_generation++;

  // 7. log, save generation
  //print();
}

int GA::roulette()
{
  double r = get_random_float(0.0, m_total_fitness);

  double accumulate = 0.0;
  for(unsigned int i=0; i<m_population_size; i++){
    accumulate += m_population[i]->get_fitness();

    if(r < accumulate)
      return i;
  }
  return 0;
}

void GA::crossover(Individual *p1, Individual *p2, pop_vector &v)
{
  // one point crossover

  if (get_random_prob() < m_prob_crossover) {
    unsigned int gi = get_random_int(0, m_nvariables-1);

    Individual *c1 = new Individual();
    Individual *c2 = new Individual();

    for (unsigned int i=0; i<gi; i++) {
      c1->add_cut(p1->get_cut(i));
      c2->add_cut(p2->get_cut(i));
    }

    for (unsigned int i=gi; i<m_nvariables; i++) {
      c1->add_cut(p2->get_cut(i));
      c2->add_cut(p1->get_cut(i));
    }

    v.push_back(c1);
    v.push_back(c2);
  }
  else {
    v.push_back(p1->copy());
    v.push_back(p2->copy());
  }

}

void GA::mutate(pop_vector &v)
{
  for (auto &indv : v) {
    if (get_random_prob() < m_prob_mutation) {
      int idx = get_random_int(0, m_nvariables-1);
      indv->set_cut(idx, get_random_cut(m_variables[idx]));
    }
  }

  return;
}

void GA::update(pop_vector &v)
{
  m_population.swap(v);
}

TString GA::get_selection(Individual *indv)
{
  TString selection;
  for (unsigned int i=0; i<m_nvariables; i++) {
    selection += m_variables[i].name;
    selection += m_variables[i].type;
    selection += Form("%f", indv->get_cut(i));
    if(i<m_nvariables-1) selection += " && ";
  }

  if (!m_weight.IsNull()) {
      selection.Prepend("(");
      selection.Append(")");
      selection += "*" + m_weight;
    }

  return selection;
}

double GA::evaluate_fitness(Individual* indv)
{
  TString selection = get_selection(indv);

  double s = m_signal_chain->GetEntries(selection);
  double b = m_background_chain->GetEntries(selection);

  double significance = get_significance(s, b);


  Double_t bins[4];
  for (unsigned int i=0; i<m_nvariables; i++) {
    bins[i] = indv->get_cut(i);
  }
  Long64_t bin = m_hist_sig->GetBin(bins);

  m_hist_sig->SetBinContent(bin, significance);

  return significance;
}

double GA::get_random_cut(const Variable &var)
{
  int n = (var.max-var.min)/var.step;

  int rnd = get_random_int(0, n);

  return (var.min + rnd * var.step);
}

void GA::print()
{
  std::cout << "--- Generation " << m_generation << std::endl;
  for(const auto &indv : m_population){
   indv->print();
 }
  std::cout << "---" << std::endl;
}
