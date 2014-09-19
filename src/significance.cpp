#include <TMath.h>

#include "significance.h"


double get_significance(double s, double b)
{
  if (s < 0 || b < 1)
    return 0.00;

  double sig_2 =  2 * ( (s + b) * TMath::Log(1 + s/b) - s );

  if (sig_2 < 0.00 || std::isinf(sig_2))
    return 0.00;

  return TMath::Sqrt(sig_2);
}

// Get significance taking into account
// the background uncertainty
double get_significance(double s, double b, double sb)
{
  if (s < 0 || b < 1)
    return 0.00;

  sb = b * sb;   // background uncertainty (sb is given as percentage)

  double b_2  = TMath::Power(b, 2);
  double sb_2 = TMath::Power(sb, 2);

  double sig_2 = 2 * ((s + b) * TMath::Log( ((s + b) * (b + sb_2))/(b_2 + (s + b) * sb_2) ) -
                      (b_2/sb_2) * TMath::Log(1 + (s * sb_2)/(b * (b + sb_2))) );

  if (sig_2 < 0.00 || !std::isinf(sig_2))
    return 0.00;

  return TMath::Sqrt(sig_2);
}

double get_efficiency(double s, double s0)
{
  if (s > s0 || s0 < 1)
    return 0.0;

  return s/s0;
}

double get_rejection(double b, double b0)
{
  return (1 - get_efficiency(b, b0));
}
