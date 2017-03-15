#include <TMath.h>
#include "RooStats/NumberCountingUtils.h"

#include "significance.h"

// S/B or S/(S+B)
double get_s_over_b(double s, double b)
{
  return b>0. ? s/b : 0.;
}

double get_s_over_total(double s, double b)
{
  return (s+b)>0 ? s/(s+b) : 0.;
}

// Get significance taking into account
// the background uncertainty
double get_significance(double s, double b, double sb)
{
  double sig = RooStats::NumberCountingUtils::BinomialExpZ(s, b, sb);
  return sig;
}

// double get_significance(double s, double b, double sb)
// {
//   if (s < 0. || b < 0.)
//     return 0.00;

//   sb = b * sb;   // background uncertainty (sb is the fraction of b)

//   double b_2  = TMath::Power(b, 2);
//   double sb_2 = TMath::Power(sb, 2);

//   double sig_2 = 2 * ((s + b) * TMath::Log( ((s + b) * (b + sb_2))/(b_2 + (s + b) * sb_2) ) -
//                       (b_2/sb_2) * TMath::Log(1 + (s * sb_2)/(b * (b + sb_2))) );

//   if (sig_2 < 0.00 || std::isinf(sig_2) || std::isnan(sig_2))
//     return 0.00;

//   return TMath::Sqrt(sig_2);
// }

double get_efficiency(double s, double s0)
{
  if (s > s0 || s0 < 0.1)
    return 0.0;

  return s/s0;
}

double get_rejection(double b, double b0)
{
  return (1 - get_efficiency(b, b0));
}
