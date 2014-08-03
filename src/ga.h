/** @file ga.h
 */

#ifndef GA_H
#define GA_H


class Population;


/** Implements the Genetic Algorithm
    - Creates and evolves population
    - Save information

 */

class GA {

 public:
  GA();
  ~GA();

  void run();

 private:
  Population *pop;

};
#endif
