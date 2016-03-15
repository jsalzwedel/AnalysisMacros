#include "MakeOutputProjections.C"
#include "MakeCFs.C"
#include "MakeCombinedCFs.C"
#include "RebinNumDen.C"



void RunAllCF(Bool_t isDataCompact, Bool_t isTrainResult)
{
  // Project out out the signal and denominator distributions
  MakeCFProjections(isDataCompact, isTrainResult);

  // Make CFs from the (unmerged) data
  MakeCFs(isDataCompact, isTrainResult);

  // Combine fields, centralities and LLAA for each cut
  MakeCombinedCFs(isDataCompact, isTrainResult);

  // Rebin the numerator and denominator distributions
  // for use with log fitting
  RebinNumDen(isDataCompact, isTrainResult);
}
