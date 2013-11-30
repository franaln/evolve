/** @file ga.h
 */

#ifndef GA_H
#define GA_H

/** @todo
    - log
    - save step. How???
    - fitness vs time plot
    - termination condition: optimal_value, max_generation
*/

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
