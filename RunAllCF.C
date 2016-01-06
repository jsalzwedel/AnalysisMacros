#include "MakeOutputProjections.C"
#include "MakeCFs.C"
#include "MakeCombinedCFs.C"



void RunAllCF(Bool_t isDataReal)
{
  // Project out out the signal and denominator distributions
  MakeCFProjections(isDataReal);

  // Make CFs from the (unmerged) data
  MakeCFs(isDataReal);

  // Combine data sets
  MakeCombinedCFs(isDataReal);

  // Combine centrality bins
  CombineCentralitiesForEachPairType();

  // Combine together LL and AA
  CombineLLAA();
}
