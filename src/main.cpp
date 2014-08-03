/** @file evolve.cpp
    @brief brief comment

    @mainpage evolve.

    Este código implementa una dinámica molecular simple con el propósito de ejemplificar
    algunas características de la implementación de un modelado orientado a objetos.

    @author Francisco Alonso <franaln@gmail.com>
    @version 0.1
    @date 2013.11.17
*/


//Config *config;

//#include <libconfig.hh>
//using namespace libconfig;

#include "population.h"
//#include "ga.h"


/** @todo
    - log
    - save step. How???
    - fitness vs time plot
    - termination condition: optimal_value, max_generation
*/

int main(int argc, char *argv[])
{
  //config = new Config(argv[1])

  //  GA ga;
  //ga.run();


  Population pop(6, 0.9, 0.1);

  pop.evolve();

  return 0;
}
