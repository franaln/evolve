/** @file ga.cpp
 */

#include <string>
#include <iostream>
#include <algorithm>

#include <TROOT.h>
#include <TEnv.h>
#include <TFile.h>
#include <TH1D.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TLegend.h>

#include "ga.h"
#include "individual.h"
#include "random.h"
#include "significance.h"

#define ZERO 0.001

GA::GA(TString configfile)
{
  read_configuration(configfile);
  print_configuration();

  init_random();

  // output file
  output.open(m_name+".log");

  // chains
  m_signal_chain = new TChain(m_signal_treename);
  for (auto signal_file : m_signal_files)
    m_signal_chain->Add(signal_file);

  m_background_chain = new TChain(m_background_treename);
  for (auto bkg_file : m_background_files)
    m_background_chain->Add(bkg_file);

  // init some variables
  m_stall_generation = 0;

  // histograms
  Int_t bins[m_nvars];
  Double_t xmin[m_nvars];
  Double_t xmax[m_nvars];

  for (unsigned int i=0; i<m_nvars; i++) {
    bins[i] = (m_variables[i].max - m_variables[i].min)/m_variables[i].step;
    xmin[i] = m_variables[i].min;
    xmax[i] = m_variables[i].max;
  }

  hist_s = new THnSparseD("hist_s", "Signal", m_nvars, bins, xmin, xmax);
  hist_b = new THnSparseD("hist_b", "Background", m_nvars, bins, xmin, xmax);
  hist_z = new THnSparseD("hist_z", "Significance", m_nvars, bins, xmin, xmax);

}

GA::~GA()
{
  for(auto& indv : m_population){
    delete indv;
  }

  for(auto& indv : m_last_population){
    delete indv;
  }

  delete m_signal_chain;
  delete m_background_chain;
}

std::vector<TString> GA::split_string(TString line)
{

  std::vector<TString> vtokens;
  TObjArray* tokens = TString(line).Tokenize(",");

  if(tokens->GetEntriesFast()) {
    TIter iString(tokens);
    TObjString* os=0;
    while ((os=(TObjString*)iString())) {
      vtokens.push_back( os->GetString() );
    }
  }
  delete tokens;
  return vtokens;
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

  // Fitness options
  m_opt_type = env.GetValue("Opt.SignificanceType", 3);
  m_opt_significance_target = env.GetValue("Opt.SignificanceTarget", -1.0);
  m_opt_background_syst = env.GetValue("Opt.BackgroundSystUnc", 0.01);
  m_opt_background_min  = env.GetValue("Opt.BackgroundMin", 0.0);
  m_opt_background_max  = env.GetValue("Opt.BackgroundMax", 99999999999.0);
  m_opt_efficiency_min  = env.GetValue("Opt.EfficiencyMin", 0.0);

  // Files/Trees
  m_signal_files = split_string(env.GetValue("Input.SignalFile", ""));
  m_signal_treename = env.GetValue("Input.SignalTree", "");

  m_background_files = split_string(env.GetValue("Input.BackgroundFile", ""));
  m_background_treename = env.GetValue("Input.BackgroundTree", "");


  // Variables
  m_nvars = env.GetValue("Variable.N", 0);
  m_weight = env.GetValue("Variable.Weight", "");
  m_basesel = env.GetValue("Variable.BaseSelection", "");

  for (unsigned int i=0; i<m_nvars; i++) {
    TString tmp = Form("Variable%i", i+1);

    Variable var;
    var.name = env.GetValue(tmp+".Name", "");
    var.type = env.GetValue(tmp+".Type", "F");
    var.cut  = env.GetValue(tmp+".Cut", ">");
    var.min  = env.GetValue(tmp+".Min", 0.0);
    var.max  = env.GetValue(tmp+".Max", 0.0);
    var.step = env.GetValue(tmp+".Step", 0.0);
    var.bins = int((var.max-var.min)/var.step);

    m_variables.push_back(var);
  }

}

void GA::print_configuration()
{

  std::cout << std::endl << std::endl;
  std::cout << "-----------------------" << std::endl;
  std::cout << "-- Configuration" << std::endl;
  std::cout << "AnalysisName: " << m_name << std::endl;

  // GA parameters
  std::cout << "-----------" << std::endl;
  std::cout << "GA.PopulationSize: " << m_population_size << std::endl;
  std::cout << "GA.GenerationMax: "  << m_generation_max  << std::endl;
  std::cout << "GA.ProbMutation: "   << m_prob_mutation   << std::endl;
  std::cout << "GA.ProbCrossOver: "  << m_prob_crossover  << std::endl;
  std::cout << "GA.RateElitism: "    << m_elitism_rate    << std::endl;

  // Fitness options
  std::cout << "-----------" << std::endl;
  std::cout << "Opt.BackgroundMin: "     << m_opt_background_min  << std::endl;
  std::cout << "Opt.BackgroundMax: "     << m_opt_background_max  << std::endl;
  std::cout << "Opt.BackgroundSystUnc: " << m_opt_background_syst << std::endl;
  std::cout << "Opt.EfficiencyMin: "     << m_opt_efficiency_min  << std::endl;

  // Signal
  std::cout << "-----------" << std::endl;
  for (auto signal_file : m_signal_files)
    std::cout << "Input.SignalFile:     " << signal_file << std::endl;
  std::cout << "Input.SignalTree:     " << m_signal_treename << std::endl;
  for (auto bkg_file : m_background_files)
    std::cout << "Input.BackgroundFile: " << bkg_file << std::endl;
  std::cout << "Input.BackgroundTree: " << m_background_treename << std::endl;
  std::cout << "-----------------------" << std::endl;
  std::cout << std::endl << std::endl;

  // Variables
  // std::cout << "-----------" << std::endl;
  // m_nvars = env.GetValue("Variable.N", 0);
  // m_weight = env.GetValue("Variable.Weight", "");
  // m_basesel = env.GetValue("Variable.BaseSelection", "");

  // for (unsigned int i=0; i<m_nvars; i++) {
  //   TString tmp = Form("Variable%i", i+1);

  //   Variable var;
  //   var.name = env.GetValue(tmp+".Name", "");
  //   var.type = env.GetValue(tmp+".Type", ">");
  //   var.min  = env.GetValue(tmp+".Min", 0.0);
  //   var.max  = env.GetValue(tmp+".Max", 0.0);
  //   var.step = env.GetValue(tmp+".Step", 0.0);
  //   var.bins = int((var.max-var.min)/var.step);

  //   m_variables.push_back(var);
  // }

}

void GA::evolve()
{
  // create initial population (generation 0)
  std::cout << "-- Generation 0 of " <<  m_generation_max << " ..." << std::endl;
  m_generation = 0;
  m_population.resize(m_population_size);
  m_last_population.resize(m_population_size);

  for (unsigned int i=0; i<m_population_size; i++){
    m_population[i] = new Individual();

    for (auto &var : m_variables) {
      m_population[i]->add_cut(get_random_cut(var));
    }
  }
  evaluate_fitness();
  show_best();

  // loop step until condition is satisfied
  while (check_end_condition()) {
    m_generation++;
    std::cout << "-- Generation " << m_generation << " of " <<  m_generation_max << " ..." << std::endl;
    step();
    show_best();
  }

  std::cout << std::endl;

  long total_calc = 1;
  for (auto &var : m_variables) {
    total_calc *= var.bins;
  }
  std::cout << "-- Number of calculations = " << hist_z->GetNbins() << " of " << total_calc << std::endl;

  output.close();

  // make plots
  std::cout << "-- Doing some plots..." << std::endl;
  plots();


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

  // update population and save last population
  update(children);

  // evaluate fitness and sort
  evaluate_fitness();

  // log, save generation
  log();

  save_histograms();
}

bool GA::check_end_condition()
{
  // do at least one evolution
  if (m_generation < 1)
    return true;

  // two following generations with best Z=0
  if (m_population[0]->get_fitness() < ZERO && m_last_population[0]->get_fitness() < ZERO) {
    std::cout << "-- End because two following generations have Z=0. Check initial conditions" << std::endl;
    return false;
  }

  // max number of generations
  if (m_generation > m_generation_max) {
    std::cout << "-- End because we reached the maximum number of generations" << std::endl;
    return false;
  }

  // significance above number
  if (m_opt_significance_target > 0 && m_population[0]->get_fitness() > m_opt_significance_target) {
    std::cout << "-- End because we reached the target significance" << std::endl;
    return false;
  }

  // stall generation
  if (m_stall_generation > 5) {
    std::cout << "-- End because we reached a significance plateau" << std::endl;
    return false;
  }

  return true;
}

void GA::evaluate_fitness()
{
  // evaluate fitness for all individuals
  m_total_fitness = 0.0;
  float fitness;

  unsigned int indv_idx = 0;
  for(auto& indv : m_population){
    fitness = evaluate_individual_fitness(indv);
    m_total_fitness += fitness;

    indv_idx++;
    show_progress(indv_idx, m_population.size());
  }

  // sort individuals by fitness
  std::sort(m_population.begin(), m_population.end(), sort_fitness);

  // check if evolution stuck
  if (m_generation > 1) {
    int same_first_fitness = 0;
    for (int indv_idx=0; indv_idx<5; indv_idx++) {
      if ( fabs(m_population[indv_idx]->get_fitness() - m_last_population[indv_idx]->get_fitness()) < ZERO )
        same_first_fitness += 1;
    }
    if (same_first_fitness > 4)
      m_stall_generation += 1;
    else
      m_stall_generation = 0;
  }

  // save for fitness vs generation plot
  g_gen.push_back(m_generation);
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
  m_last_population.swap(m_population);
  m_population.swap(v);
}

double GA::evaluate_individual_fitness(Individual* indv)
{
  double* cuts = indv->get_cuts();

  // if it is already calculated, return significance from histogram
  long bin_z = hist_z->GetBin(cuts, false);
  if (bin_z >= 0) {

    double s = hist_s->GetBinContent(bin_z);
    indv->set_signal(s);

    double b = hist_b->GetBinContent(bin_z);
    indv->set_background(b);

    double significance = 0.;
    if (s < ZERO || b < ZERO || b < m_opt_background_min || b > m_opt_background_max)
      significance = 0.;
    else
      significance = hist_z->GetBinContent(bin_z);

    indv->set_fitness(significance);

    return significance;
  }

  // Calculate significance
  TString selection = get_selection(indv);

  double s = get_events(m_signal_chain,     selection);
  double b = get_events(m_background_chain, selection);

  if (s < 0)
    s = 0.;
  if (b < 0)
    b = 0.;

  indv->set_signal(s);
  indv->set_background(b);

  double significance = 0.;
  if (m_opt_type == 1)
    significance = get_s_over_b(s, b);
  else if (m_opt_type == 2)
    significance = get_s_over_total(s, b);
  else if (m_opt_type == 3)
    significance = get_significance(s, b, m_opt_background_syst);


  if (s < ZERO || b < ZERO || b < m_opt_background_min || b > m_opt_background_max)
    significance = 0.;

  hist_s->SetBinContent(hist_s->GetBin(cuts), s);
  hist_b->SetBinContent(hist_b->GetBin(cuts), b);
  hist_z->SetBinContent(hist_z->GetBin(cuts), significance);

  indv->set_fitness(significance);

  return significance;
}

TString GA::get_selection(Individual *indv)
{
  TString selection;

  if (!m_basesel.IsNull())
    selection = m_basesel + "&&";

  int index = 0;
  for (auto var : m_variables) {
    selection += var.name;
    selection += var.cut;
    if (var.type == "F")
      selection += Form("%f", indv->get_cut(index));
    else
      selection += Form("%i", int(indv->get_cut(index)));
    if(index<m_nvars-1) selection += " && ";
    index ++;
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
  std::cout << "Best: ";
  for (unsigned int i=0; i<m_nvars; i++) {
    std::cout << m_population[0]->get_cut(i) << " | ";
  }
  std::cout << "S/B = " <<  m_population[0]->get_signal() << "/" << m_population[0]->get_background();
  std::cout << ", Z = " << m_population[0]->get_fitness() << " " << std::endl;

}


void GA::show_progress(int progress, int total)
{
  int bar_width = 80;

  float perc = progress/float(total);

  std::cout << "[";
  int pos = bar_width * perc;
  for (int i = 0; i < bar_width; ++i) {
    if (i < pos) std::cout << "=";
    else if (i == pos) std::cout << ">";
    else std::cout << " ";
  }
  std::cout << "] " << int(perc * 100.0) << " %\r";
  std::cout.flush();

  if (progress == total)
    std::cout << std::endl;
}

void GA::log()
{
  output << "--- Generation " << m_generation << std::endl;
  for (const auto &indv : m_population) {

    for (unsigned int i=0; i<m_nvars; i++) {
      output << indv->get_cut(i) << " | ";
    }
    output << "S/B = " <<  indv->get_signal() << "/" << indv->get_background();
    output << ", Z = " << indv->get_fitness() << std::endl;
  }

  output << "---" << std::endl;
}

void GA::save_histograms()
{
  TFile f(m_name+".root", "recreate");

  hist_s->Write("signal");
  hist_b->Write("background");
  hist_z->Write("significance");

  f.Close();
}

void GA::plots()
{
  plot_significance_vs_generation();
}


void GA::plot_significance_vs_generation()
{
  TGraph *gr_best = new TGraph(g_gen.size(), &g_gen[0], &g_best[0]);
  TGraph *gr_mean = new TGraph(g_gen.size(), &g_gen[0], &g_mean[0]);

  gr_best->SetLineColor(kRed);
  gr_mean->SetLineColor(kBlue);
  gr_best->SetMarkerColor(kRed);
  gr_mean->SetMarkerColor(kBlue);

  gr_best->SetLineWidth(2);
  gr_mean->SetLineWidth(2);
  gr_best->SetFillColor(0);
  gr_mean->SetFillColor(0);
  gr_best->SetMarkerStyle(20);
  gr_mean->SetMarkerStyle(20);

  gr_best->SetTitle("");
  gr_best->GetYaxis()->SetTitle("Significance");
  gr_best->GetXaxis()->SetTitle("Generation");

  TCanvas *c1 = new TCanvas();

  gr_best->Draw("PAL");
  gr_mean->Draw("PL");

  TLegend *leg = new TLegend(0.75, 0.75, 0.88, 0.88);
  leg->SetBorderSize(0);
  leg->AddEntry(gr_best, "Best");
  leg->AddEntry(gr_mean, "Mean");
  leg->Draw();

  c1->Print("sig_vs_generation.pdf");
}
