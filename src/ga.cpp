/** @file ga.cpp
 */

#include <string>
#include <iostream>
#include <algorithm>

#include <TROOT.h>
#include <TEnv.h>
#include <TFile.h>
#include <TH1D.h>

#include "ga.h"
#include "individual.h"
#include "random.h"
#include "significance.h"

GA::GA(TString configfile)
{
  read_configuration(configfile);

  init_random();

  // output file
  output.open(m_name+".log");

  // chains
  m_signal_chain = new TChain(m_signal_treename);
  m_signal_chain->AddFile(m_signal_file);

  m_background_chain = new TChain(m_background_treename);
  m_background_chain->AddFile(m_background_file);

  // histograms
  Int_t bins[m_nvars];
  Double_t xmin[m_nvars];
  Double_t xmax[m_nvars];

  for (unsigned int i=0; i<m_nvars; i++) {
    bins[i] = (m_variables[i].max - m_variables[i].min)/m_variables[i].step;
    xmin[i] = m_variables[i].min;
    xmax[i] = m_variables[i].max;
  }

  hist_s   = new THnSparseD("hist_s", "Signal", m_nvars, bins, xmin, xmax);
  hist_b   = new THnSparseD("hist_b", "Background", m_nvars, bins, xmin, xmax);
  hist_sig = new THnSparseD("hist_sig", "Significance", m_nvars, bins, xmin, xmax);

}

GA::~GA()
{
  save_histograms();

  output.close();

  for(auto& ind : m_population){
    delete ind;
  }

  delete m_signal_chain;
  delete m_background_chain;
}

void GA::read_configuration(TString configfile)
{
  TEnv env(configfile.Data());

  m_name = env.GetValue("AnalysisName", "output");

  // GA parameters
  m_population_size = env.GetValue("GA.PopulationSize", 0);
  m_generation_max  = env.GetValue("GA.GenerationMax", 10);
  m_prob_mutation   = env.GetValue("GA.ProbMutation", 0.0);
  m_prob_crossover  = env.GetValue("GA.ProbCrossOver", 0.0);
  m_elitism_rate    = env.GetValue("GA.RateElitism", 0.0);

  // Signal
  m_signal_file = env.GetValue("Signal.File", "");
  m_signal_treename = env.GetValue("Signal.TreeName", "");

  // Background
  m_background_file = env.GetValue("Background.File", "");
  m_background_treename = env.GetValue("Background.TreeName", "");
  m_background_syst = env.GetValue("Background.Syst", -1.0);

  // Variables
  m_nvars = env.GetValue("Variable.N", 0);
  m_weight = env.GetValue("Variable.Weight", "");
  m_basesel = env.GetValue("Variable.BaseSelection", "");

  for (unsigned int i=0; i<m_nvars; i++) {
    TString tmp = Form("Variable%i", i+1);

    Variable var;
    var.name = env.GetValue(tmp+".Name", "");
    var.type = env.GetValue(tmp+".Type", ">");
    var.min  = env.GetValue(tmp+".Min", 0.0);
    var.max  = env.GetValue(tmp+".Max", 0.0);
    var.step = env.GetValue(tmp+".Step", 0.0);
    var.bins = int((var.max-var.min)/var.step);

    m_variables.push_back(var);
  }

}

void GA::evolve()
{
  // create initial population (generation 0)
  std::cout << "-- Generation 0 of " <<  m_generation_max << " ..." << std::endl;
  m_generation = 0;
  m_population.resize(m_population_size);

  for (unsigned int i=0; i<m_population_size; i++){
    m_population[i] = new Individual();

    for (auto &var : m_variables) {
      m_population[i]->add_cut(get_random_cut(var));
    }
  }
  evaluate_fitness();

  // loop step until condition is satisfied
  for (unsigned int i=1; i<m_generation_max; i++){

    // check if

    std::cout << "-- Generation " << i << " of " <<  m_generation_max << " ..." << std::endl;
    show_best();
    step();
  }

  long total_calc = 1;
  for (auto &var : m_variables) {
    total_calc *= var.bins;
  }
  std::cout << "Number of calculations = " << hist_sig->GetNbins() << " of " << total_calc << std::endl;
}

void GA::step()
{
  pop_vector children;
  children.clear();
  children.reserve(m_population_size);

  // elitism
  unsigned int elite_size = m_population_size * m_elitism_rate;

  for (unsigned int i=0; i<elite_size; i++) {
    children.push_back(m_population[i]->copy());
  }

  // crossover
  while (children.size() < (m_population_size - elite_size)) {

    // select parents
    int p1 = roulette();
    int p2 = roulette();

    // crossover parents
    crossover(m_population[p1], m_population[p2], children);
  }

  // mutation
  mutate(children);

  // update population
  update(children);
  m_generation++;

  // evaluate fitness and sort
  evaluate_fitness();

  // log, save generation
  log();
}

void GA::evaluate_fitness()
{
  // evaluate fitness for all individuals
  m_total_fitness = 0.0;
  float fitness;
  for(auto& indv : m_population){
    fitness = evaluate_individual_fitness(indv);
    indv->set_fitness(fitness);
    m_total_fitness += fitness;
  }

  // sort individuals by fitness
  std::sort(m_population.begin(), m_population.end(), sort_fitness);

  g_best.push_back(m_population[0]->get_fitness());
  g_mean.push_back(m_total_fitness/m_population_size);
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
    unsigned int gi = get_random_int(0, m_nvars-1);

    Individual *c1 = new Individual();
    Individual *c2 = new Individual();

    for (unsigned int i=0; i<gi; i++) {
      c1->add_cut(p1->get_cut(i));
      c2->add_cut(p2->get_cut(i));
    }

    for (unsigned int i=gi; i<m_nvars; i++) {
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
      int idx = get_random_int(0, m_nvars-1);
      indv->set_cut(idx, get_random_cut(m_variables[idx]));
    }
  }
}

void GA::update(pop_vector &v)
{
  m_population.swap(v);
}

double GA::evaluate_individual_fitness(Individual* indv)
{
  double* cuts = indv->get_cuts();

  // if it is already calculated, return significance from histogram
  long bin_sig = hist_sig->GetBin(cuts, false);
  if (bin_sig >= 0) {
    return hist_sig->GetBinContent(bin_sig);
  }

  TString selection = get_selection(indv);

  double s = get_events(m_signal_chain, selection);
  double b = get_events(m_background_chain, selection);

  double significance = 0.;
  if (m_background_syst > 0.)
    significance = get_significance(s, b, m_background_syst);
  else
    significance = get_significance(s, b);

  hist_s->SetBinContent(hist_s->GetBin(cuts), s);
  hist_b->SetBinContent(hist_b->GetBin(cuts), b);
  hist_sig->SetBinContent(hist_sig->GetBin(cuts), significance);

  return significance;
}

TString GA::get_selection(Individual *indv)
{
  TString selection;

  if (!m_basesel.IsNull())
    selection = m_basesel + "&&";

  for (unsigned int i=0; i<m_nvars; i++) {
    selection += m_variables[i].name;
    selection += m_variables[i].type;
    selection += Form("%f", indv->get_cut(i));
    if(i<m_nvars-1) selection += " && ";
  }

  return selection;
}

double GA::get_events(TChain *chain, TString selection)
{
  TH1D *h_tmp = new TH1D("htmp", "htmp", 1, 0.5, 1.5);
  chain->Project("htmp", selection, m_weight);

  double n = h_tmp->Integral(1, h_tmp->GetNbinsX());

  h_tmp->Delete();

  return n;
}

double GA::get_random_cut(const Variable &var)
{
  int rnd = get_random_int(0, var.bins);

  return (var.min + rnd * var.step);
}

void GA::show_best()
{
  for (unsigned int i=0; i<m_nvars; i++) {
    std::cout << m_population[0]->get_cut(i) << " | ";
  }
  std::cout << "Z = " << m_population[0]->get_fitness() << std::endl;

}


void GA::log()
{
  output << "--- Generation " << m_generation << std::endl;
  for (const auto &indv : m_population) {

    for (unsigned int i=0; i<m_nvars; i++) {
      output << indv->get_cut(i) << " | ";
    }
    output << "Z = " << indv->get_fitness() << std::endl;
  }

  output << "---" << std::endl;
}

void GA::save_histograms()
{
  TFile f(m_name+".root", "recreate");

  hist_s->Write("signal");
  hist_b->Write("background");
  hist_sig->Write("significance");

  f.Close();
}
