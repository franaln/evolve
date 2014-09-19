/** @file evolve.cpp

    @mainpage evolve.

    @author Francisco Alonso <franaln@gmail.com>
    @version 0.1
    @date 2013.11.17
*/

#include <iostream>

#include "ga.h"

int main(int argc, char *argv[])
{

  if (argc < 2) {
    std::cout << "usage: evolve [CONFIG]" << std::endl;
    return 1;
  }

  GA ga(argv[1]);
  ga.evolve();

  return 0;
}
