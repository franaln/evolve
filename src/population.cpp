/** @file population.cpp
 */

#include <string>
#include <iostream>
#include <algorithm>

#include "population.h"
#include "chromosome.h"
#include "random.h"
#include "variable.h"

Population::Population(float p_crossover, float p_mutate) :
  pop_size(10)
{

  // setup random generator
  rnd = new Random();

  // variables
  int n_vars = 1;
  Variable *var = new Variable("jet_n", 0, 10, 1);


  //-- create population (generation 0)
  generation = 0;
  chromosomes.reserve(pop_size);

  // fill each chromosome randomly
  for(int i=0; i<pop_size; i++){
    std::vector<int> *genes = new std::vector<int>;
    for(int j=0; j<n_vars; j++){
      genes->push_back(rnd->next(0, (var->max()-var->min())/var->step()));
    }
    chromosomes.push_back(genes);
  }

}

Population::~Population()
{
  for(auto& c : chromosomes){
    delete c;
  }
}



void Population::step()
{
  // 1. evaluate fitness
  int i = 0;
  for(const auto& indiv : chromosomes){
    std::cout << i << " " << (*indiv)[0] << std::endl;
    //indiv->eval_fitness();
    i++;
  }

  // 2. sort individuals
  std::sort(chromosomes.begin(), chromosomes.end());

  // 3. select parents
  select_parents();

  // 2. crossover
  crossover();

  // 3. mutation
  mutate();
  // 4. update population
  update();
  generation++;

  // 5. log, save generation

}

void Population::evolve()
{
  // loop step until condition is satisfied
  for(int i=0; i<1; i++)
    step();

}

void Population::select_parents()
{
  // vector<Chromosome*> parents;

  // while (parents.size() < size) {
  //   parents.push_back(torneooo);
  // }
}

void Population::crossover()
{
  // self.children = []
  //   while len(self.children) < self.size:
  // p1 = random.choice(self.parents)
  //                              p2 = random.choice(self.parents)
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

void Population::mutate()
{
  // for(auto &indiv : indivuals){
  //   if(random() < prob_mutation)
  //     indiv.mutate();
  // }

  return;
}

void Population::update()
{
  // for child in self.children:
  //           child.evaluate()
  //       self.children.sort()
  //       nroReemplazo = int(self.reemp * self.size)
  //       for j in range(nroReemplazo, self.size):
  //           self.population[j] = self.children[j - nroReemplazo]
  //       self.population.sort()

}


    // def torneo(self, size = 3, goliat = 0.9):
    //     competidores = [random.choice(self.population) for i in range(size)]
    //     competidores.sort()
    //     if random.random() < goliat:
    //         return competidores[0]
    //     else:
    //         return random.choice(competidores[1:])
