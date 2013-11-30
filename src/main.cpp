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
#include "ga.h"


int main(int argc, char *argv[])
{
  //config = new Config(argv[1])

  GA ga;

  ga.run();

  return 0;
}
