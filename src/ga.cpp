/** @file ga.cpp
 */

#include <string>
#include <iostream>
#include <algorithm>

#include <TEnv.h>
#include <TChain.h>

#include "ga.h"
#include "individual.h"
#include "random.h"

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

  print();
}

GA::~GA()
{
  for(auto& ind : m_population){
    delete ind;
  }

  delete m_signal_chain;
  delete m_background_chain;
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
  std::sort(m_population.begin(), m_population.end());
  std::reverse(m_population.begin(), m_population.end());

  // 3. elitism
  unsigned int elite_size = m_population_size * m_elitism_rate;

  for (unsigned int i=0; i<elite_size; i++) {
    children.push_back(m_population[i]->copy());
  }

  // 4. crossover
  while (children.size() < (m_population_size - elite_size)) {

    //int tsize = temp_pop.size();

    // select parents
    int p1 = roulette();
    int p2 = roulette();

    //std::cout<<p1 << " " <<p2<<std::endl;

    // crossover
    //temp_pop.push_back(p1->copy());
    //temp_pop.push_back(p2->copy());
    crossover(m_population[p1], m_population[p2],
              children);

  }

  // 5. mutation
  mutate(children);

  // 6. update population
  update(children);
  m_generation++;

  // 7. log, save generation
  print();
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
  v.push_back(p1->copy());
  v.push_back(p2->copy());

  //           if random.random() < self.crossover_p:
  //               cromSize = 4
  //               gi = random.randint(0, cromSize-2)
  //               gd = random.randint(gi+1,cromSize-1)
  //               h1 = Individual(p1.chromosome[0:gi] + p2.chromosome[gi:gd] + p1.chromosome[gd:])
  //               h2 = Individual(p2.chromosome[0:gi] + p1.chromosome[gi:gd] + p2.chromosome[gd:])
  //               self.children.append(h1)
  //               self.children.append(h2)
  //           else:
  //               self.children.append(p1)
  //               self.children.append(p2)
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

void GA::print()
{
  std::cout << "--- Generation " << m_generation << std::endl;
  for(const auto &indv : m_population){
   indv->print();
 }
  std::cout << "---" << std::endl;
}

float GA::evaluate_fitness(Individual* indv)
{
  TString selection;

  for (unsigned int i=0; i<m_nvariables; i++) {
    selection += m_variables[i].name;
    selection += m_variables[i].type;
    selection += Form("%f", indv->get_cut(i));
    if(i<m_nvariables-1) selection += " && ";
  }

  // float s = m_signal_chain->GetEntries(selection);
  // float b = m_background_chain->GetEntries(selection);
  // float significance = get_significance(s, b);

  return 1;
}


void GA::read_configuration(TString configfile)
{
  TEnv env(configfile.Data());

  // GA parameters
  m_population_size = env.GetValue("GA.PopulationSize", 0);
  m_prob_mutation = env.GetValue("GA.ProbMutation", 0.0);
  m_prob_crossover = env.GetValue("GA.ProbCrossOver", 0.0);
  m_elitism_rate = env.GetValue("GA.RateElitism", 0.0);

  std::cout << m_population_size << std::endl;

  // Signal
  m_signal_file = env.GetValue("Signal.File", "");
  m_signal_treename = env.GetValue("Signal.TreeName", "");

  // Background
  m_background_file = env.GetValue("Background.File", "");
  m_background_treename = env.GetValue("Background.TreeName", "");

  // Variables
  m_nvariables = env.GetValue("Variable.N", 0);

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

  // Significance definition
  m_significance_def = env.GetValue("Significance", 1);
}


float GA::get_random_cut(const Variable &var)
{
  int n = (var.max-var.min)/var.step;

  int rnd = get_random_int(0, n);

  return (var.min + rnd * var.step);
}
