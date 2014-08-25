/** @file ga.cpp
 */

#include <string>
#include <iostream>
#include <algorithm>

#include "config.h"
#include "ga.h"
#include "individual.h"
#include "random.h"

// struct ObservableInt {
//   std::string name;
//   int min;
//   int max;
//   int step;
// };

// struct ObservableFloat {
//   std::string name;
//   double min;
//   double max;
//   double step;
// };


GA::GA() :
  m_population_size(POPULATION_SIZE),
  m_prob_crossover(PROB_CROSSOVER),
  m_prob_mutation(PROB_MUTATION)
{

  //  std::cout << OBS1 << std::endl;

  // setup random generator
  init_random();

  // observables
  //int n_vars = 1;

  // Variable *var1 = new Variable("jet_n", "i", 0, 10, 1);
  // Variable *var2 = new Variable("rt2", "f", 0.0, 1.0, 0.1);

  //-- create initial population (generation 0)
  m_generation = 0;
  m_population.resize(m_population_size);

  // fill each individual randomly
  for(unsigned int i=0; i<m_population_size; i++){
    m_population[i] = new Individual(1, 1);
    m_population[i]->set_cut_int(0, get_random_int(0, 10));
    m_population[i]->set_cut_float(0, get_random_float(0.0, 1.0));
  }

  print();
}

GA::~GA()
{
  for(auto& ind : m_population){
    delete ind;
  }
}

void GA::evolve()
{
  // loop step until condition is satisfied
  for(int i=0; i<25; i++)
    step();

}

void GA::step()
{
  pop_vector children;
  children.clear();
  children.reserve(m_population_size);

  // 1. evaluate fitness
  for(auto& indv : m_population){
    indv->set_fitness(indv->cuts_int[0]+indv->cuts_float[0]);
  }

  m_total_fitness = 0.0;
  for(auto& indv : m_population){
    m_total_fitness += indv->get_fitness();
  }

  // 2. sort individuals
  std::sort(m_population.begin(), m_population.end()); //, sort_by_fitness);
  std::reverse(m_population.begin(), m_population.end());

  // 3. elitism
  unsigned int elite_size = m_population_size * ELITISM_RATE;

  for(unsigned int i=0; i< elite_size; i++){
    children.push_back(m_population[i]->copy());
  }



  // 4.
  while(children.size() < (m_population_size - elite_size)){

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

  // 3. mutation
  mutate(children);

  // 4. update population
  update(children);
  m_generation++;

  // 5. log, save generation
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
  //for(auto &indv : v){
  //if(get_random_prob() < m_prob_mutation) {
      //indv->mutate();
      //std::cout << "mutate" << std::endl;     //indv->mutate();
      //int index = get_random_int(1, 2);
      //std::cout << index << std::endl;
  //}

  //}

  return;
}

void GA::update(pop_vector &v)
{
  m_population.swap(v);

  // for child in self.children:
  //           child.evaluate()
  //       self.children.sort()
  //       nroReemplazo = int(self.reemp * self.size)
  //       for j in range(nroReemplazo, self.size):
  //           self.population[j] = self.children[j - nroReemplazo]
  //       self.population.sort()
}

void GA::print()
{
  std::cout << "--- Generation " << m_generation << std::endl;
  for(const auto &indv : m_population){
   indv->print();
 }
  std::cout << "---" << std::endl;
}

// Individual Population::choice(pop_vector v)
// {
//   int idx = m_rnd->rnd_int(0, v.size());
//   return v[idx];
// }

// int  Population::get_parent_index()
// {
//   // choose k (the tournament size) individuals from the population at random
//   pop_vector competitors(3);

//   for(int i=0; i<3; i++){

//     indv = choice(population);

//     competitors[i] = indv.copy();
//   }

//   std::sort(competitors.begin(), competitors.end(), sort_by_fitness);

//   rnd.rnd_float(0.0, 1.0);

//   // choose the best individual from pool/tournament with probability p
//   // choose the second best individual with probability p*(1-p)
//   // choose the third best individual with probability p*((1-p)^2)


//     // def torneo(self, size = 3, goliat = 0.9):
//     //     competidores = [random.choice(self.population) for i in range(size)]
//     //     competidores.sort()
//     //     if random.random() < goliat:
//     //         return competidores[0]
//     //     else:
//     //         return random.choice(competidores[1:])

// }

// void Population::sort_by_fitness(Individual* a, Individual* b)
// {
//   return (a->get_fitness() < b->get_fitness());
// }


// void Population::select_parents()
// {
//   // vector<Chromosome*> parents;

//   // while (parents.size() < size) {
//   //   parents.push_back(torneooo);
//   // }
// }
