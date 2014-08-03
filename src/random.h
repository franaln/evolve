#ifndef RANDOM_H
#define RANDOM_H

#include <random> // random generators

typedef std::mt19937 generator_t;

void init_random();
int get_random_int(int, int);
double get_random_float(double, double);
double get_random_prob();

#endif
