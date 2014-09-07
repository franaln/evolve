#include <TMath.h>

#include "significance.h"


double get_significance(double s, double b)
{
  if (s < 0 || b < 0){
    //error("Error! The number of events of signal or background is < 0")
    return 0.00;
  }

  //if (b == 0):
  //    return 0.00

  double temp =  2 * ( (s + b) * TMath::Log(1 + s/b) - s );

  if (temp < 0.0)
    return 0.00;

  double significance = TMath::Sqrt(temp);

  if (significance > 0.0 && !std::isinf(significance))
    return significance;
  else
    return 0.0;
}
