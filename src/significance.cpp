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

double get_significance(double s, double b, double sb)
{
  // Get significance taking into account
  // the background uncertainty (from Cowan)

  if (s < 0 || b < 0)
    return 0.00;

  //if b == 0.: return 0.00

  //if sb is None: sb = 0.25 * b # for now we use 25% of uncertainty for the background
  sb = b * sb;

  double b_2 = TMath::Power(b, 2);
  double sb_2 = TMath::Power(sb, 2);

  double zaint = 2 * ((s + b) * TMath::Log( ((s + b) * (b + sb_2))/(b_2 + (s + b) * sb_2) ) -
                      (b_2/sb_2) * TMath::Log(1 + (s * sb_2)/(b * (b + sb_2))) );

  if (zaint <= 0.)
    return 0.00;

  double za = TMath::Sqrt(zaint);

  return za;
}

double get_efficiency(double s, double s0)
{

  if (s > s0)
    return 0.0;

  double efficiency;
  if(s0 > 0)
    efficiency = s/s0;
  else
    efficiency = 0.00;

  return efficiency;
}

double get_rejection(double b, double b0)
{
  return (1 - get_efficiency(b, b0));
}
