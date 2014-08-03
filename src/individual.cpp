/** @file individual.cpp
 */

#include <string>

#include "individual.h"

Individual::Individual(int size_int, int size_float)
{
  cuts_int.resize(size_int, 0);
  cuts_float.resize(size_float, 0.0);
}

Individual::~Individual()
{

}

// void Individual::mutate()
// {
//   // get random number between 0 and size
//   //self.chromosome[random.choice(range(4))] = random.choice([0,1])

//   // flip gen
//   //genes[random].flip();


//   return;
// }

Individual* Individual::copy()
{
  Individual *twin = new Individual(cuts_int.size(), cuts_float.size());

  for(int i=0; i< cuts_int.size(); i++)
    twin->set_cut_int(i, cuts_int[i]);

  for(int i=0; i< cuts_float.size(); i++)
    twin->set_cut_float(i, cuts_float[i]);

  twin->set_fitness(m_fitness);

  return twin;
}

// void Individual::eval_fitness()
// {
//   fitness = 5;
// }

// void Individual::create_me_random()
// {
//   if(genes.size()>0) return;

//   for(int i=0; i<size; i++){
//     Gene* new_gene = new Gene();
//     genes.push_back(new_gene);
//   }

// }

void Individual::print()
{
  for(auto &cut : cuts_int)
    std::cout << cut << " ";

  for(auto &cut : cuts_float)
    std::cout << cut << " ";

  std::cout << std::endl;
}
