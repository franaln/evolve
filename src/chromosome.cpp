/** @file chromosome.cpp
 */

#include "chromosome.h"

Chromosome::Chromosome(int size)
{
  genes.reserve(size);
}

Chromosome::~Chromosome()
{

}



// void Chromosome::mutate()
// {
//   // get random number between 0 and size
//   //self.chromosome[random.choice(range(4))] = random.choice([0,1])

//   // flip gen
//   //genes[random].flip();


//   return;
// }

// Chromosome* Chromosome::copy()
// {
//   Chromosome *twin = new Chromosome();

//   //twin.fitness = self.fitness
//   return twin;
// }

// void Chromosome::eval_fitness()
// {
//   fitness = 5;
// }

// void Chromosome::create_me_random()
// {
//   if(genes.size()>0) return;

//   for(int i=0; i<size; i++){
//     Gene* new_gene = new Gene();
//     genes.push_back(new_gene);
//   }

// }
