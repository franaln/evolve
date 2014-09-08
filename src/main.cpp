/** @file evolve.cpp
    @brief brief comment

    @mainpage evolve.

    Este código implementa una dinámica molecular simple con el propósito de ejemplificar
    algunas características de la implementación de un modelado orientado a objetos.

    @author Francisco Alonso <franaln@gmail.com>
    @version 0.1
    @date 2013.11.17
*/

#include <iostream>

#include "ga.h"

/** @todo
    - log
    - save step. How???
    - fitness vs time plot
    - termination condition: optimal_value, max_generation
*/
int main(int argc, char *argv[])
{

  if (argc < 2) {
    std::cout << "usage: evolve [CONFIG]" << std::endl;
    return 1;
  }

  std::cout << argv[1] << std::endl;
  GA ga(argv[1]);
  ga.evolve();

  return 0;
}
